#ifndef CUSTOMVIEW_H
#define CUSTOMVIEW_H
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
#include "settings.h"

extern "C" void dgetrs_(char* TRANS, int* N, int* NRHS, double* A, int* LDA, int* IPIV, double* B, int* LDB, int* INFO);
extern "C" void dgetrf_(int* M, int* N, double* A, int* LDA, int* IPIV, int* INFO);
extern "C" void dgesv_(int* N, int* NRHS, double* A, int* LDA, int* IPIV, double* B, int* LDB, int* INFO);
extern "C" void dgbsv_(int* N, int* KL, int* KU, int* NRHS, double* AB, int* LDAB, int* IPIV, double* B, int* LDB, int* INFO);
extern "C" void dpbsv_(char* UPLO, int* N, int* KD, int* NRHS, double* AB, int* LDAB, double* B, int* LDB, int* INFO);
extern "C" void spbsv_(char* UPLO, int* N, int* KD, int* NRHS, float* AB, int* LDAB, float* B, int* LDB, int* INFO);
extern "C" void dpbtrf_(char* UPLO, int* N, int* KD, double* AB, int* LDAB, int* INFO);
extern "C" void dpbtrs_(char* UPLO, int* N, int* KD, int* NRHS, double* AB, int* LDAB, double* B, int* LDB, int* INFO);


class customView : public QGraphicsView
{
    Q_OBJECT
public:
    customView(QWidget *parent = 0);

    QPen* pen;
    Settings* settings;
    airodump_client* a_client;
    QPixmap* pixmap;
    QPixmap* pixmap_tiff;
    std::map <int, Color> *db_color_map;
    QThread* airodump_thread;
    DrawRasterWidget* opengl;
    QPixmap* gl_pixmap;
    QBitmap* mask;

    void add_point(Point p);
    void set_geom(uint32 new_width, uint32 new_height);
    ~customView();
public slots:
    void clear_points();
    /*
     * составление и решение уравнения теплопроводности, создание и вывод на экран результата с маской (план
     * аргументы - указатель на map с палитрой
     */
    void draw_heat();
public:
    void mousePressEvent(QMouseEvent * e);
signals:
    void signal_block_activity(); //сигнал для блокировки г.о. (во время определения сигнала в точке, например)
private:
    uint32 width, height;
    QGraphicsScene *scene;
    std::vector<Point> points;
    unsigned int amount_of_points;
};

#endif // CUSTOMVIEW_H

