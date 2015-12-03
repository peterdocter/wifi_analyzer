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
#include <QVBoxLayout>
#include "custombuttongroup.h"
#include <QRadioButton>
#include <QScrollArea>
#include <QFileDialog>
#include <QScrollBar>
#include <QMessageBox>
#include <QErrorMessage>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    cur_sig_pixmap = new QPixmap();
    cur_sig_scene = new QGraphicsScene();
    settings = new Settings((QWidget*)this);
    this->setObjectName("Wi-Fi Area Analyzer");
    ui->setupUi(this);
    if(!ui->graphicsView->a_client->init()) exit(0);
    ui->current_signal_frame->hide();
    connect(ui->graphicsView->a_client, SIGNAL (update_networks_list(int, char*)), this, SLOT (handle_new_network(int, char*)));
    connect(ui->refresh_button, SIGNAL (pressed()), this, SLOT (handle_refresh_button()));
    scene = new QGraphicsScene();
    ctd_scene = new QGraphicsScene();
    pixmap = new QPixmap();
    ctd = new QPixmap();
    ui->graphicsView->db_color_map = &(ui->color_to_db->db_color_map);

    //загрузка плана
    pixmap->load(settings->PATH_TO_PLAN);
    QBitmap* mask = new QBitmap();
    *mask = pixmap->createMaskFromColor(QColor(255, 255, 255));
    pixmap->setMask(*mask);
    delete mask;
    if(!pixmap->isNull()) scene->addPixmap((*pixmap).scaled(settings->FINE_WIDTH, settings->FINE_HEIGHT, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->graphicsView->set_geom(settings->FINE_WIDTH, settings->FINE_HEIGHT);
    ui->graphicsView->setScene(scene);

    //настройки, коннекты
    ui->graphicsView->verticalScrollBar()->blockSignals(true);
    ui->graphicsView->horizontalScrollBar()->blockSignals(true);
    ui->color_to_db->verticalScrollBar()->blockSignals(true);
    ui->color_to_db->horizontalScrollBar()->blockSignals(true);
    ui->color_to_db->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    ui->color_to_db->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    connect(ui->graphicsView->a_client, SIGNAL(point_added_message(QString)), ui->graphicsView, SLOT(draw_heat()));
    connect(ui->graphicsView->a_client, SIGNAL(point_added_message(QString)), ui->label, SLOT(setText(QString)));
    connect(ui->graphicsView, SIGNAL(signal_block_activity()), this, SLOT(block_activity()));
    connect(ui->graphicsView->a_client, SIGNAL(point_added_message(QString)), this, SLOT(free_activity()));
    ui->actionSave_picture->setAutoRepeat(false);
    connect(ui->actionSave_picture, SIGNAL(triggered()), this, SLOT(save_picture()));
    connect(ui->actionLoad_new_plan, SIGNAL(triggered()), this, SLOT(load_new_plan()));
    connect(ui->graphicsView->a_client, SIGNAL(update_current_signal(int)), this, SLOT(update_current_signal(int)));
    connect(ui->color_to_db, SIGNAL(palette_updated()), ui->graphicsView, SLOT(draw_heat()) );

    //старт Wi-Fi потока
    ui->graphicsView->airodump_thread->start();

    //создание виджета списка сетей (слева внизу на экране)
    buttons = NULL;
    ui->groupBox = new QGroupBox(this);
    ui->groupBox->setTitle(" Available networks");
    vbox = new QVBoxLayout();
    vbox->setAlignment(Qt::AlignTop);
    buttons = new customButtonGroup(ui->graphicsView->a_client, NULL, ui->current_signal_frame, ui->current_signal_label);
    connect(buttons, SIGNAL(buttonClicked(int)), buttons, SLOT (handleCheckingRadio()));
    connect(buttons, SIGNAL(buttonClicked(int)), ui->graphicsView, SLOT (clear_points()));
    connect(buttons, SIGNAL(update_current_network_aps_label(int)), this, SLOT(update_current_network_aps_label(int)));
    connect(ui->graphicsView->a_client, SIGNAL(update_current_network_aps_label(int)), this, SLOT(update_current_network_aps_label(int)));
    ui->groupBox->setLayout(vbox);
    ui->scrollArea->setWidget(ui->groupBox);
    ui->scrollArea->show();
}

void MainWindow::update_current_network_aps_label(int amount){
    char x[] = "xxxxx";
    if(ui->graphicsView->a_client->current_network.id != NETWORK_NOT_SELECTED){
        sprintf(x, "%d", amount);
        ui->aps_in_current_network_label->setText(x);
    }
}

void MainWindow::save_picture(){
    QString output_file = QInputDialog::getText(this, "Save picture",
                                               "Path for saving:", QLineEdit::Normal, ".png");
    if(output_file.length()==0) return;
    QPixmap pixmap = ui->graphicsView->grab();
    if(!pixmap.save(output_file)) {
        QMessageBox::critical(NULL, "Error", "Plan was not saved");
    }
    return;
}

void MainWindow::load_new_plan(){
    QString d = "Open plan";
    QString output_file;
    //output_file = QInputDialog::getText(NULL, a, b, QLineEdit::Normal, c);
    output_file = QFileDialog::getOpenFileName(NULL, d, QDir::currentPath());;
    if(output_file==0) return;
    settings->PATH_TO_PLAN = output_file;
    pixmap->load(settings->PATH_TO_PLAN);
    scene->clear();
    scene->addPixmap(*pixmap);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->draw_heat();
    return;
}

void MainWindow::block_activity(){
    this->setDisabled(true);
}

void MainWindow::free_activity(){
    this->setDisabled(false);
}

void MainWindow::update_current_signal(int current){
    char tmp2[16];
    if(current == -MAX_ABS_SIG){
        sprintf(tmp2, "No signal");
    }else{
        sprintf(tmp2, "%d", current);
    }
    ui->current_signal_label->setText(tmp2);
    ui->current_signal_frame->show();
    if(-current <= MAX_ABS_SIG-3) {
        ui->current_signal_frame->setGeometry(20, ((-current))*5+20, 51, 21);
    }else{
        ui->current_signal_frame->setGeometry(20, ((MAX_ABS_SIG-3))*5+20, 51, 21);
    }
}

void MainWindow::handle_new_network(int id, char* name){
    QRadioButton *network_button = new QRadioButton();
    network_button->setText((const char*)(name));
    vbox->addWidget(network_button);
    buttons->addButton(network_button, id);
    connect(network_button, SIGNAL(clicked(bool)), buttons, SLOT(handleCheckingRadio()));
    connect(network_button, SIGNAL(clicked(bool)), ui->graphicsView, SLOT (clear_points()));
    connect(buttons, SIGNAL(buttonClicked(int)), buttons, SLOT (handleCheckingRadio()));
    connect(buttons, SIGNAL(buttonClicked(int)), ui->graphicsView, SLOT (clear_points()));
    ui->groupBox->setLayout(vbox);
    ui->scrollArea->setWidget(ui->groupBox);
    ui->scrollArea->show();
    char x[] = "xxxxx";
    sprintf(x, "%d", ui->graphicsView->a_client->amount_of_found_networks);
    ui->amount_of_networks_label->setText(x);
    update_current_network_aps_label(ui->graphicsView->a_client->available_networks[ui->graphicsView->a_client->current_network.id].aps.size());
    return;
}

void MainWindow::handle_refresh_button(){
    scene->clear();
    if(!settings->PATH_TO_PLAN.isEmpty()){
        pixmap->load(settings->PATH_TO_PLAN);
        QBitmap* mask = new QBitmap();
        *mask = pixmap->createMaskFromColor(QColor(255,255,255));
        pixmap->setMask(*mask);
        if(!pixmap->isNull()) scene->addPixmap((*pixmap).scaled(settings->FINE_WIDTH, settings->FINE_HEIGHT, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    ui->graphicsView->setScene(scene);
    ui->label->setText("");
    ui->graphicsView->clear_points();
}

Ui::MainWindow* MainWindow::get_ui(){
    return ui;
}

void MainWindow::my_exit(){
    exit(0);
}

MainWindow::~MainWindow()
{
    ui->graphicsView->a_client->close_client();
    delete settings;
    delete cur_sig_pixmap;
    delete cur_sig_scene;
    delete buttons;
    delete scene;
    delete ctd_scene;
    delete ui->graphicsView;
    delete ui;
}
