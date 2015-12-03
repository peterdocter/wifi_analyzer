#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <cstdio>
#include <QImage>
#include <QBitmap>
#include <QPixmap>
#include "paletteview.h"
#include <vector>
#include <airodump-lib.h>
#include "airodump_client.h"
#include <math.h>
#include <ctime>
#include <signal.h>
#include <unistd.h>
#include <widget.h>

#define max(a,b) (a>b? a : b)
#define min(a,b) (a<b? a : b)

paletteView::paletteView(QWidget *parent) :
    QGraphicsView(parent)
{
    smooth_palette_flag = 1;
    width = 50;
    height = 500;
    scene = new QGraphicsScene();
    this->setScene(scene);
    ctd_scene = new QGraphicsScene();
    ctd = new QPixmap();
    fill_palette();
}

void paletteView::fill_palette(){
    int r,g,b;
    r=0;
    g=0;
    int step;
    int i, j;
    db_color_map.clear();
    if(!smooth_palette_flag){
        step = 40;
        b = 255;
        for (i = 0; i > -20; i--)
        {
            for(j=0;j<5;++j){
                db_color_map.insert ( std::pair<int,Color>(i*5-j, Color((uint8_t)r,(uint8_t)g,(uint8_t)b)) );
            }
            if(b > 0) {
                b -= step;
                if(b <= 55) b=0;
                g+=step;
                if(g > 200 || b==0) g=255;
            }else if(b==0 && g==255 && r!=255){
                r+=step;
                if(r>255) r=255;
            }else if(b==0 && g <= 255 && r==255){
                g-=step;
                if(g<0) g=0;
            }
        }
    }else{
        b=255;
        step = 8;

        for (i = 0; i > -100; i--)
        {
            db_color_map.insert ( std::pair<int,Color>(i, Color((uint8_t)r,(uint8_t)g,(uint8_t)b)) );
            if(b>0) {
                b-=step;
                if(b<0) b=0;
                g+=step;
                if(g>255 || b==0) g=255;
            }else if(b==0 && g==255 && r!=255){
                r+=step;
                if(r>255) r=255;
            }else if(b==0 && g<=255 && r==255){
                g-=step;
                if(g<0) g=0;
            }
        }
    }
    db_color_map.insert ( std::pair<int,Color>(-100, Color((uint8_t)255,(uint8_t)255,(uint8_t)255)) );
    draw_palette();
    emit palette_updated();
}

void paletteView::draw_palette(){
    unsigned int i, j, k;
    uchar *raster;
    raster = (uchar*)malloc(width*height*sizeof(uchar)*4);
    Color tmp;
    for(i = 0; i < height/5; ++i){
        for(j = 0; j < 5; ++j){
            for(k = 0; k < width; ++k){
                tmp = db_color_map[-i];
                raster[i*width*5*4 + j*width*4 + k*4] = tmp.red;
                raster[i*width*5*4 + j*width*4 + k*4 + 1] = tmp.green;
                raster[i*width*5*4 + j*width*4 + k*4 + 2] = tmp.blue;
                raster[i*width*5*4 + j*width*4 + k*4 + 3] = 255;
            }
        }
    }
    *ctd = QPixmap::fromImage(
                QImage(
                    (unsigned char *) raster,
                    width,
                    height,
                    QImage::Format_RGBA8888
                )
            );
    ctd_scene->addPixmap((*ctd));
    setScene(ctd_scene);
    free(raster);
}

void paletteView::mousePressEvent(QMouseEvent *event){
    event->setAccepted(true);
    smooth_palette_flag = !smooth_palette_flag;
    fill_palette();
}

void paletteView::set_geom(uint32 new_width, uint32 new_height){
    width=new_width;
    height=new_height;
}


paletteView::~paletteView(){
}
