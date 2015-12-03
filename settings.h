#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QWidget>

class Settings
{
public:
    QWidget* mainwindow;
    Settings(QWidget* parent);
    QString PATH_TO_PLAN;

    int FINE_WIDTH; // mainwindow.h
    int FINE_HEIGHT;


    int SCALE; //customview.cpp
    int BORDER;


};

#endif // SETTINGS_H
