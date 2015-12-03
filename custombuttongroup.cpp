#include "custombuttongroup.h"
#include "mainwindow.h"
#include "airodump_client.h"

customButtonGroup::customButtonGroup(airodump_client* a_cl, QLabel* n_info, QFrame* cur_sig_f, QLabel* cur_sig_l) : QButtonGroup()
{
    a_client=a_cl;
    network_info = n_info;
    current_signal_frame = cur_sig_f;
    current_signal_label = cur_sig_l;
}

void customButtonGroup::handleCheckingRadio(){
    char *s;
    s=(char*)malloc(500*sizeof(char));
    current_signal_frame->hide();
    sprintf(s, "...");
    current_signal_label->setText(s);
    free(s);
    int i;
    for(i=1;i<=13;i++){
        airodump_enable_channel(a_client->current_device_id, i, 0);
    }
    // блокировка потока Wi-Fi
    a_client->pause = THREAD_LOCK_REQUEST;
    // ждём, пока поток Wi-Fi заблокируется
    while(a_client->pause != THREAD_LOCKED_ACKNOWLEDGE);
    // меняем данные
    a_client->current_network.bssid = a_client->available_networks[checkedId()].bssid;
    a_client->current_network.name = a_client->available_networks[checkedId()].name;
    a_client->current_network.channel = a_client->available_networks[checkedId()].channel;
    a_client->current_network.id = a_client->available_networks[checkedId()].id;

    a_client->enable_channels_for_current_network();
    a_client->enable_disjoint_channels();
    emit update_current_network_aps_label(a_client->available_networks[checkedId()].aps.size());
    // выпускаем поток
    a_client->pause = THREAD_FREE;
    return;
}

