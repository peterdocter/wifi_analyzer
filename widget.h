#ifndef WIDGET_H
#define WIDGET_H
#include "color.h"
#include <QColor>
#include <QGLWidget>

class DrawRasterWidget : public QGLWidget
{
public:
    DrawRasterWidget(QWidget* parent);

    int32_t* raster;
    int scale;
    int height;
    int width;
    std::map <int, Color> *db_color_map;
    uint8_t * texture;

    void initializeGL();
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    void setGL(int32_t* raster, int scale, int height, int width, std::map <int, Color> *db_color_map);
    ~DrawRasterWidget();
};

#endif // WIDGET_H
