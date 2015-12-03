#include "settings.h"
#include <QInputDialog>
#include <QFileDialog>

Settings::Settings(QWidget* parent)
{
    mainwindow = parent;
    QString a = "Plan loading";
    QString b = "Path to plan:";
    QString c = "/home/denis/test_qt_notebook/plan2.tif";
    QString d = "Open plan";
    PATH_TO_PLAN = QFileDialog::getOpenFileName(mainwindow, d, QDir::currentPath());

    FINE_WIDTH = 600; // mainwindow.h
    FINE_HEIGHT = 600;

    SCALE = 5; //customview.cpp
    BORDER = 0;
}
