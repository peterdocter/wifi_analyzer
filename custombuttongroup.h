#ifndef CUSTOMBUTTONGROUP_H
#define CUSTOMBUTTONGROUP_H

#include <QButtonGroup>
#include <QLabel>
#include "airodump_client.h"

class customButtonGroup : public QButtonGroup
{
    Q_OBJECT
public:
    customButtonGroup(airodump_client* a_cl, QLabel* n_info, QFrame* cur_sig_f, QLabel* cur_sig_l);
    //указатель на airodump_client из customGraphicsView, 3 указателя для вывода информации на г.о.

    airodump_client* a_client;
    QLabel* network_info;
    QFrame* current_signal_frame;
    QLabel* current_signal_label;
signals:
    void update_current_network_aps_label(int);
    void buttonClicked(int);
public slots:
    void handleCheckingRadio();
};

#endif // CUSTOMBUTTONGROUP_H
