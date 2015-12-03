#ifndef PALETTEVIEW_H
#define PALETTEVIEW_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <vector>
#include "point.h"
#include "color.h"
#include <map>
#include <QLabel>
#include <QEvent>
#include "airodump_client.h"
#include <QProgressBar>
#include <QThread>
#include "widget.h"

class paletteView : public QGraphicsView
{
    Q_OBJECT
public:
    paletteView(QWidget *parent = 0);

    int smooth_palette_flag;
    std::map <int, Color> db_color_map;
    QGraphicsScene *ctd_scene;
    QPixmap* ctd;

    void set_geom(uint32 new_width, uint32 new_height);
    void draw_palette();
    void fill_palette();

    ~paletteView();
public slots:
public:
    void mousePressEvent(QMouseEvent * e);
signals:
    void palette_updated();
private:
    uint32 width, height;
};

#endif // PALETTEVIEW_H

