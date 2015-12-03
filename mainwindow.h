#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <cstdio>
#include <QApplication>
#include <QPushButton>
#include <QProcess>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QButtonGroup>
#include "custombuttongroup.h"
#include "settings.h"
#include <QInputDialog>
#include "airodump_client.h"
#include "paletteview.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    Settings* settings;
    QPixmap* cur_sig_pixmap;
    QGraphicsScene* cur_sig_scene;
    customButtonGroup* buttons;
    QGraphicsScene *scene;
    QGraphicsScene *ctd_scene;
    QPixmap* pixmap;
    QPixmap* ctd;

    Ui::MainWindow* get_ui();

    ~MainWindow();
public slots:
    void load_new_plan();
    void block_activity();
    void free_activity();
    void save_picture();
    void handle_new_network(int id, char* name);
    void handle_refresh_button();
    void update_current_signal(int x);
    void update_current_network_aps_label(int amount);
    void my_exit();
private:
    QVBoxLayout *for_scroll;
    QVBoxLayout *vbox;
    Ui::MainWindow *ui;
};



#endif // MAINWINDOW_H
