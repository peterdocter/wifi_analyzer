#include "widget.h"
#include "color.h"

DrawRasterWidget::DrawRasterWidget(QWidget *parent) // конструктор
    : QGLWidget(parent)
{
    resize(600, 600); // задаем размеры окна
}

void DrawRasterWidget::initializeGL()
{
   qglClearColor(Qt::white); // заполняем экран белым цветом
   /*
   //glEnable(GL_DEPTH_TEST); // задаем глубину проверки пикселей
   glShadeModel(GL_FLAT); // убираем режим сглаживания цветов
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);
   // Сглаживание точек
   glEnable(GL_POINT_SMOOTH);
   glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
   // Сглаживание линий
   glEnable(GL_LINE_SMOOTH);
   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
   // Сглаживание полигонов
   glEnable(GL_POLYGON_SMOOTH);
   //glShadeModel(GL_FLAT);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
   glEnable(GL_MULTISAMPLE);
   //glEnable(GL_CULL_FACE); // говорим, что будем строить только внешние поверхности
   //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); // фигуры будут закрашены с обеих сторон
   */
   glEnable(GL_TEXTURE_1D);
   int i;
   Color color;
   texture = (uint8_t*)malloc(sizeof(uint8_t)*3*100);
   for(i = 0; i < 100; ++i){
       color = (*db_color_map)[-i];
       texture[i*3] = color.red;
       texture[i*3+1] = color.green;
       texture[i*3+2] = color.blue;
   }
   GLuint textureID;
   glGenTextures(1, &textureID);
   glBindTexture(GL_TEXTURE_1D, textureID);
   glTexImage1D(GL_TEXTURE_1D, 0, 3, 100, 0, GL_RGB, GL_UNSIGNED_BYTE,
                    texture);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void DrawRasterWidget::resizeGL(int nWidth, int nHeight)
{
    glViewport(0, 0, nWidth, nHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void DrawRasterWidget::paintGL(){
    int i, j;

    /*
    Color c_lt;
    Color c_rt;
    Color c_rb;
    Color c_lb;
    */

    glOrtho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    double value;
    for(i = scale/2; i+scale < height; i += scale){
        for(j = scale/2; j+scale < width; j += scale){
            /*
            c_lt = (*db_color_map)[((int32_t*)raster)[i*width+j]];
            c_rt = (*db_color_map)[((int32_t*)raster)[i*width+j+scale]];
            c_rb = (*db_color_map)[((int32_t*)raster)[(i+scale)*width+j+scale]];
            c_lb = (*db_color_map)[((int32_t*)raster)[(i+scale)*width+j]];
            */

            glBegin(GL_QUADS);

            value = -((int32_t*)raster)[i*width+j];
            glTexCoord1f(value/100);
            //qglColor(QColor(c_lt.red, c_lt.green, c_lt.blue));
            glVertex2f(j, height-i);

            value = -((int32_t*)raster)[i*width+j+scale];
            glTexCoord1f(value/100);
            //qglColor(QColor(c_rt.red, c_rt.green, c_rt.blue));
            glVertex2f(j+scale, height-i);

            value = -((int32_t*)raster)[(i+scale)*width+j+scale];
            glTexCoord1f(value/100);
            //qglColor(QColor(c_rb.red, c_rb.green, c_rb.blue));
            glVertex2f(j+scale, height-i-scale);

            value = -((int32_t*)raster)[(i+scale)*width+j];
            glTexCoord1f(value/100);
            //qglColor(QColor(c_lb.red, c_lb.green, c_lb.blue));
            glVertex2f(j, height-i-scale);
            glEnd();
        }
    }
}

void DrawRasterWidget::setGL(int32_t* rasterA, int scaleA, int heightA, int widthA, std::map <int, Color> *db_color_mapA){
    raster = rasterA;
    scale = scaleA;
    height = heightA;
    width = widthA;
    db_color_map = db_color_mapA;
}

DrawRasterWidget::~DrawRasterWidget(){
    free(texture);
}
