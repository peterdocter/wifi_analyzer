#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <cstdio>
#include <QImage>
#include <QBitmap>
#include <QPixmap>
#include "customview.h"
#include <vector>
#include <airodump-lib.h>
#include "airodump_client.h"
#include <math.h>
#include <ctime>
#include <signal.h>
#include <unistd.h>
#include <widget.h>

#define IRONS_PAIN_CONST 1000000

#define max(a,b) (a>b? a : b)
#define min(a,b) (a<b? a : b)

customView::customView(QWidget *parent) :
    QGraphicsView(parent)
{
    settings = ((MainWindow*)((MainWindow*)parentWidget())->parentWidget())->settings;
    pen = new QPen();
    amount_of_points = 0;
    scene = new QGraphicsScene();
    a_client = new airodump_client(settings);
    a_client->points = &points;
    this->setScene(scene);
    airodump_thread = new QThread;
    a_client->moveToThread(airodump_thread);
    connect(airodump_thread, SIGNAL(started()), a_client, SLOT(scan()));
}

void customView::mousePressEvent(QMouseEvent *event){
    event->setAccepted(true);
    if(a_client->mouse_pressed == 0){
        a_client->mouse_pressed=0;
        if(a_client->current_network.id == NETWORK_NOT_SELECTED) {
            char s[] = "Please, choose network\nfor analyzing";
            emit a_client->point_added_message(QString(s));
            return;
        }
        QPoint mousePoint = event->pos();
        int x,y;
        x=mousePoint.x();
        if(x < 0) x=0+settings->BORDER;
        if(x > settings->FINE_WIDTH) x = settings->FINE_WIDTH-settings->BORDER;
        y=mousePoint.y();
        if(y < 0) y=0+settings->BORDER;
        if(y > settings->FINE_HEIGHT) y = settings->FINE_HEIGHT-settings->BORDER;
        unsigned int i;
        // проверяем, не было ли уже добавлено значение в этой точке (и в квадрате SCALExSCALE, в котором лежит новая точка)
        // квадрат нужно проверять, т.к. мы уменьшаем разрешение в дальнейшем
        if(points.size() > 0){
            for(i = 0; i < points.size(); ++i){
                if(x >= (points[i].x/settings->SCALE)*settings->SCALE && x <= (points[i].x/settings->SCALE)*settings->SCALE + settings->SCALE &&
                   y >= (points[i].y/settings->SCALE)*settings->SCALE && y <= (points[i].y/settings->SCALE)*settings->SCALE + settings->SCALE){
                    return;
                }
            }
        }
        add_point(Point(x, y, -MAX_ABS_SIG));
        // главное окно блокируется на время поиска сигнала в этой точке
        emit signal_block_activity();
        a_client->mouse_pressed = 1;
    }
}

void customView::set_geom(uint32 new_width, uint32 new_height){
    width=new_width;
    height=new_height;
}

void customView::add_point(Point p){
    points.push_back(p);
}

void customView::clear_points(){
    points.clear();
    amount_of_points = 0;
}

int convert_to_band_sym_index(int i, int j, int kd){
    int ret;
    ret = j*(kd+1) + (kd+i-j);
    if(i >= max(0,j-kd) && j>=max(0,j-kd) && j>=i){
        return ret;
    }
    return -1;
}

void customView::draw_heat(){
    if(a_client->current_network.id == NETWORK_NOT_SELECTED) return;
    if(points.size()==0) return;

    gl_pixmap = new QPixmap();
    pixmap = new QPixmap();
    pixmap_tiff = new QPixmap();
    opengl = new DrawRasterWidget(NULL);
    mask = new QBitmap();

    unsigned int i = 0;
    uint32 width, height;

    //деление на settings->SCALE : сжимаем изображения, чтобы уравнение теплопроводности быстрее решалось и требовало меньше памяти
    unsigned int kd = (settings->FINE_WIDTH)/settings->SCALE;
    unsigned int m = (settings->FINE_WIDTH)/settings->SCALE;
    unsigned int k = (settings->FINE_WIDTH)/settings->SCALE;
    width=k;
    height=m;
    unsigned int n = points.size();

    int *nr = (int*)malloc(n*sizeof(int));
    int *nc = (int*)malloc(n*sizeof(int));

    double *nv = (double*)malloc(n*sizeof(double));
    double *b = (double*)malloc(m*k*sizeof(double));
    double *ab=(double*)malloc((kd+1)*m*k*sizeof(double));

    memset(b, 0, m*k*sizeof(double));
    memset(ab, 0, (kd+1)*m*k*sizeof(double));

    for(i = 0; i < n; ++i){
        nr[i]=points[i].y/settings->SCALE;
        nc[i]=points[i].x/settings->SCALE;
        nv[i]=(double)points[i].sig;
    }

    unsigned int row, col;
    int lapack_info, lapack_nrhs, lapack_n, lapack_ldb, lapack_kd, lapack_ldab;

    int index;
    double dec = 0;

    //заполняем матрицу:
    // dec - для стационарности (когда у элемента нет 4-ёх соседей, нельзя ставить для него коэффициент 4)
    for(i = 0; i < m*k; ++i){
        row = i/(m);
        col = i%(k);
        dec=0;

        if(col==0 || col==k-1) dec++;
        if(row==0 || row==m-1) dec++;

    //вычисляем индекс элемента для оптимизации симметрической ленточной матрицы (http://www.netlib.org/lapack/lug/node124.html) и значения коэффициентов
        index = convert_to_band_sym_index(i, i, kd);
        if(index>=0) ab[index]=4.0-dec;

        index = convert_to_band_sym_index(i, i+1, kd);
        if(i+1<m*k && col!=(k-1) && index>=0)   ab[index] = -1.0;

        index = convert_to_band_sym_index(i, i-1, kd);
        if(i>=1 && col!=(0) && index>=0)        ab[index] = -1.0;

        index = convert_to_band_sym_index(i, -m+row*m+col, kd);
        if(row!=0 && index>=0)                  ab[index] = -1.0;

        index = convert_to_band_sym_index(i, m+row*m+col, kd);
        if(row!=(m-1) && index>=0)              ab[index] = -1.0;
    }

    //для всех известных точек вставляем в матрицу значение методом Айронса-Пейна (http://reffan.ru/referat_jgejgeatyotrqasyfsujg.html)
    for(i = 0; i < n; ++i){
        row = nr[i];
        col = nc[i];
        index = convert_to_band_sym_index(row*k+col, row*k+col, kd);
        if(index>=0) ab[index] = IRONS_PAIN_CONST;
        b[row*k+col] = IRONS_PAIN_CONST*nv[i];
    }

    free(nr);
    free(nc);
    free(nv);


    //решаем с помощью LAPACK-а
    lapack_info = 0;
    lapack_nrhs = 1;
    lapack_n = m*k;
    lapack_ldb = m*k;
    lapack_kd = kd;
    lapack_ldab = lapack_kd+1;

    dpbsv_((char*)"U", &lapack_n, &lapack_kd, &lapack_nrhs, ab, &lapack_ldab, b, &lapack_ldb, &lapack_info);

    free(ab);

    //восстанавливаем изначальный размер изображения и заполняем растр
    width = settings->FINE_WIDTH;
    height = settings->FINE_HEIGHT;
    int npixels=width*height;

    uint32* raster=(uint32 *)malloc(npixels *sizeof(uint32));

    memset(raster, 0, npixels*sizeof(uint32));

    width = settings->FINE_WIDTH;
    height = settings->FINE_HEIGHT;
    for(row = settings->SCALE/2; row < height; row+=settings->SCALE){
        for(i = settings->SCALE/2; i < width; i+=settings->SCALE){
            ((int32_t*)raster)[row*width+i] =
                    (int32_t)b[((row-settings->SCALE/2)/settings->SCALE)*width/settings->SCALE+(i-settings->SCALE/2)/settings->SCALE];
        }
    }

    //для сглаживания полигонов в OpenGL
    QGLFormat fmt;
    fmt.setSampleBuffers(true);
    fmt.setSamples(16);
    QGLFormat::setDefaultFormat(fmt);

    //с помощью OpenGL (widget.h, widget.cpp) нормально заполняем растр - преобразуем уровень сигнала в цвет, интерполируем
    //значения на маленькие квадраты (SCALE x SCALE)
    opengl->setGL((int32_t*)raster, settings->SCALE, settings->FINE_HEIGHT, settings->FINE_WIDTH, db_color_map);
    opengl->paintGL();
    *gl_pixmap = opengl->renderPixmap();

    //добавляем пиксмапы в сцену, генерируем и накладываем маску из плана (если он есть), добавляем "кружки" в точки, где
    //получили сигнал
    scene->clear();
    scene->addPixmap(*gl_pixmap);

    if(!settings->PATH_TO_PLAN.isEmpty()){
        pixmap_tiff->load(settings->PATH_TO_PLAN);
        *mask = pixmap_tiff->createMaskFromColor(QColor(255, 255, 255));
        pixmap_tiff->setMask(*mask);
        if(!pixmap_tiff->isNull()) scene->addPixmap((*pixmap_tiff).scaled(settings->FINE_WIDTH, settings->FINE_HEIGHT, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    for(i=0;i<points.size();i++){
        scene->addEllipse(qreal((points[i].x/settings->SCALE)*settings->SCALE),
                          qreal((points[i].y/settings->SCALE)*settings->SCALE),
                          qreal(settings->SCALE),
                          qreal(settings->SCALE),
                          *pen,
                          QBrush(Qt::white));
    }
    setScene(scene);
    free(b);
    free(raster);
    delete gl_pixmap;
    delete pixmap;
    delete pixmap_tiff;
    delete opengl;
    delete mask;
}

void swap(Point* a, Point* b){
    Point tmp;
    tmp = (*a);
    *a=*b;
    *b=tmp;
    return;
}

int sign(int x){
    if(x>0) return 1;
    if(x<0) return -1;
    return 0;
}

customView::~customView(){
    delete opengl;
    delete pen;
    delete a_client;
    delete pixmap;
    delete pixmap_tiff;
    airodump_thread->terminate();
    delete airodump_thread;
}
