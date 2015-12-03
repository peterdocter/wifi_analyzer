#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>
#include "widget.h"
#include <unistd.h>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(geteuid() != 0)
    {
        QMessageBox* pmbx = new QMessageBox(NULL);
        pmbx->setText("Root required");
        pmbx->setWindowTitle("Fail");
        pmbx->addButton(QMessageBox::Cancel);
        pmbx->exec();
        delete pmbx;
        return 0;
    }

    MainWindow *w = new MainWindow(NULL);
    w->setWindowTitle("Wi-Fi Analyzer");
    w->show();
    return a.exec();
}
