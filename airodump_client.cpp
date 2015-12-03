#include "airodump_client.h"
#include <unistd.h>
#include <QtDebug>
#include <string>
#include <QButtonGroup>
#include <QRadioButton>
#include <QInputDialog>
#include <QProcess>

void sighandler(int sig)
{
    if(sig) puts("interrupted");
}

int airodump_client::equal_macs(unsigned char* a, unsigned char* b){
    int i;
    for(i=0;i<6;i++){
        if((unsigned int)a[i] != (unsigned int)b[i]) {
            return 0;
        }
    }
    return 1;
}

airodump_client::airodump_client(Settings *s){
    srand(time(NULL));
    //tv0 = {0, 100000};
    //tv = {0, 0};
    tv0.tv_sec = 0;
    tv0.tv_usec = 100000;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    signal(SIGINT, sighandler);
    airodump_init();
    airodump_set_debug_level(0);
    settings = s;
/*
 * ===========================================================
 * подключение устройства
 * поиск доступных устройств с помощью ifconfig
 */
    QString a = "Device initialization";
    QString b = "Device:";
    QString c = "wlan1";
    device = "";
    QProcess ifconfig_grep;
    ifconfig_grep.start("/bin/sh", QStringList() << "-c" << "ifconfig | grep wlan");
    ifconfig_grep.waitForFinished();
    QByteArray devices = ifconfig_grep.readAll();
    QStringList devs;
    QRegExp regex("wlan\\w+");
    int pos = 0;
    b = "Available devices:";
    while(pos >= 0){
        b+="\n";
        pos = regex.indexIn(devices, pos);
        devs = regex.capturedTexts();
        b+="\t";
        b+=devs.at(0);
        if( pos>=0 ) pos++;
    }
    int valid_device_flag = 0;
    while(!valid_device_flag) {
        device = QInputDialog::getText(settings->mainwindow, a, b, QLineEdit::Normal, c);
        if(device.isEmpty()) {
            a = "Not valid device";
            valid_device_flag = 0;
        }
        ifconfig_grep.start("/bin/sh", QStringList() << "-c" << "ifconfig | grep wlan");
        ifconfig_grep.waitForFinished();
        devices = ifconfig_grep.readAll();
        pos = 0;
        b = "Available devices:";
        while(pos >= 0){
            b+="\n";
            pos = regex.indexIn(devices, pos);
            devs = regex.capturedTexts();
            b+="\t";
            b+=devs.at(0);
            if(device == devs.at(0)) valid_device_flag = 1;
            if( pos>=0 ) pos++;
        }
        if (device.isEmpty()) valid_device_flag = 0;
    }
    current_device_id = airodump_add_device(device.toLatin1().data(), 13,
       AIRODUMP_FLAG_ENABLE_ALL_CHANNELS
    );
    r = airodump_switch_channels();
/*
 * ===========================================================
 */
    pause = THREAD_FREE;
    amount_of_found_networks = 0;
    stop = 0;
    start_getting_signal = 0;
    mouse_pressed = 0;
    current_avg_signal = -MAX_ABS_SIG;
    current_network.id = NETWORK_NOT_SELECTED;
}

/*
 *  включение непересекающихся каналов 1, 6, 11
 */
void airodump_client::enable_disjoint_channels(){
    airodump_enable_channel(current_device_id, 1, 1);
    airodump_enable_channel(current_device_id, 6, 1);
    airodump_enable_channel(current_device_id, 11, 1);
}

/*
 * проверка пакета (можно ли его использовать)
 */
int airodump_client::valid_packet(AIRODUMP_CAPTURE_INFO* ci){
    if(ci->ap.active == 1 && ci->ap.channel > 0 && ci->ap.channel <= 13) return 1;
    return 0;
}

/*
 * включение каналов, на которых вещают AP(Access Points - точки доступа) текущей сети (тек. сеть определяется через структуру current_network класса)
 */
void airodump_client::enable_channels_for_current_network(){
    unsigned int k;
    for(k=0;k<available_networks[current_network.id].aps.size();k++){
        airodump_enable_channel(current_device_id, available_networks[current_network.id].aps[k].channel, 1);
    }
}

/*
 * включение всех каналов
 */
void airodump_client::enable_all_channels(){
    int k;
    for(k=1;k<=13;k++){
        airodump_enable_channel(current_device_id, k, 1);
    }
}

/*
 * проверка имени (не пустая ли строка и не NULL)
 */
int airodump_client::valid_name(const unsigned char* name){
    return name != NULL && strlen((const char*)name) != 0;
}

/*
 * добавление новой AP к списку AP сети с номером id
 */
void airodump_client::add_new_ap_to_existing_network(int id){
    available_networks[id].aps.push_back(ci.ap);
    airodump_enable_channel(current_device_id, ci.ap.channel, 1);
    if(id == current_network.id) emit update_current_network_aps_label(available_networks[id].aps.size());
}

/*
 * добавление новой сети network к списку сетей
 */

void airodump_client::add_new_network(NETWORK_ID network){
    available_networks.push_back(network);
    amount_of_found_networks++;
    emit update_networks_list(available_networks[available_networks.size()-1].id, (char*)available_networks[available_networks.size()-1].name);
}

/*
 * попытка переключить каналы (если не получилось - пытаемся перезагрузить устройство)
 * были ситуации, когда в какой-то момент по неизвестной причине устройство переставало переключать каналы -
 * пришлось сделать так
 */
void airodump_client::try_to_switch_channels(){
    int r;
    char message[] = "Point xxxxxx was added\nAverage singal: xxx\n";
    if((r = airodump_switch_channels()) > 0){
        //перезагрузка устройства, если не получилось поменять канал
        airodump_del_device(current_device_id);
        current_device_id = airodump_add_device(device.toLatin1().data(), 13, 0);
        if(current_device_id == -1){
            //не получилось добавить устройство - вывод сообщения и попытки добавить в цикле
            sprintf(message, "No device\n");
            current_signal_level = -MAX_ABS_SIG;
            emit update_current_signal(-MAX_ABS_SIG);
            while(current_device_id == -1){
                current_device_id = airodump_add_device(device.toLatin1().data(), 13, 0);
            }
        }
        //сеть выбрана - включаем каналы, на которых висят точки доступа выбранной сети + каналы 1,6,11
        //иначе - включаем все каналы
        if(current_network.id != NETWORK_NOT_SELECTED){
            enable_channels_for_current_network();
            enable_disjoint_channels();
        }else{
            enable_all_channels();
        }
    }
}

/*
 * основной цикл работы
 * принимает пакет, по нему
 *      - определяется новые сети и AP
 *      - если выбрана сеть
 *          - определяет текущий сигнал (есть таймаут для текущей сети)
 *          - если требовалось определить сигнал для карта (было нажатие на карту)
 *              - пытается получить пакет от всех AP текущей сети (есть таймаут для каждой AP)
 *              - из них выбирает максимальный
 */

void airodump_client::scan(){
    stop = 0;
    n = 0;
    int i = 0;
    time_t duration_getting_signal = 0;
    start_getting_signal = 0;
    int new_network_flag = 1;
    char message[] = "Point xxxxxx was added\nAverage singal: xxx\n";
    while(!stop){
        // если стоит флаг блокировки потока - информируем, что поток заблокирован
        if (pause == THREAD_LOCK_REQUEST) pause = THREAD_LOCKED_ACKNOWLEDGE;
        while(pause){
        }
        if(start_getting_signal != 0){
            duration_getting_signal = time(NULL);
            if(difftime(duration_getting_signal, start_getting_signal) > 3.0){
                duration_getting_signal = 0;
                start_getting_signal = 0;
                sprintf(message, "No singal\n");
                current_signal_level = -MAX_ABS_SIG;
                emit update_current_signal(current_signal_level);
                continue;
            }
        }
        new_network_flag = 1;
        if (!tv.tv_sec && !tv.tv_usec)
        {
            tv = tv0;
            ++ n;
            if (!(n % 2)) try_to_switch_channels();
        }
        r = airodump_receive(&ci, buf, sizeof(buf), &tv);
        if(!r){
            r = airodump_parse_packet(&ci, 0);
            if(!valid_packet(&ci)) {
                continue;
            }
            //если новая точка доступа
            if(ci.event_mask & AIRODUMP_EVENT_NEW_AP) {
                airodump_set_updated(&ci, AIRODUMP_EVENT_NEW_AP);
                //проверяем, принадлежит ли эта точка доступа уже найденной сети или нет
                //если да - добавляем её к списку точек доступа сети и включаем канал этой точки доступа
                //если нет - добавляем новую сеть
                new_network_flag = 1;
                for(i = 0; i < amount_of_found_networks; ++i){
                    if(valid_name(ci.ap.name) &&
                            strcmp((const char*)ci.ap.name, (const char*)available_networks[i].name)==0){
                        new_network_flag = 0;
                        add_new_ap_to_existing_network(i);
                    }
                }
                if(new_network_flag && valid_name(ci.ap.name)){
                    NETWORK_ID new_id;
                    new_id.id = amount_of_found_networks;
                    new_id.bssid = (ci.ap.bssid);
                    new_id.name = (unsigned char *)ci.ap.name;
                    new_id.channel = ci.ap.channel;
                    new_id.aps.push_back(ci.ap);
                    add_new_network(new_id);
                }
            }
            //если какая-либо сеть выбрана
            if(current_network.id != NETWORK_NOT_SELECTED){
                //если текущий пакет пришел от выбранной сети:
                // 1) обновляем текущее значение сигнала
                if( valid_name(ci.ap.name) && strcmp((const char*)ci.ap.name, (const char*)current_network.name) == 0){
                    unsigned int j;
                    int current_best_signal = -MAX_ABS_SIG;
                    int tmp_ap_sig = 0;
                    for(j = 0; j < available_networks[current_network.id].aps.size(); ++j){
                        airodump_get_ap_info(available_networks[current_network.id].aps[j].id, &available_networks[current_network.id].aps[j]);
                        tmp_ap_sig = available_networks[current_network.id].aps[j].avg_sig;
                        if (tmp_ap_sig > current_best_signal){
                            current_best_signal=tmp_ap_sig;
                        }
                    }
                    current_signal_level = current_best_signal;
                    emit update_current_signal(current_signal_level);
                    start_getting_signal = time(NULL);
                }
                // 2) если в данный момент требуется определить сигнал для отображения на карте
                if(mouse_pressed > 0){
                    unsigned int j;
                    int max = -MAX_ABS_SIG;
                    int ap_sig = -MAX_ABS_SIG;
                    time_t start_time = 0;
                    time_t cur_time = 0;
                    int rr = 0;
                    //от каждой точки доступа этой сети ждем пакет и ищем наилучшее значение сигнала из этих пакетов
                    for(j = 0; j < available_networks[current_network.id].aps.size(); ++j){
                        //переключаемся на канал выбранной точки доступа
                        while(airodump_get_channel(current_device_id) != available_networks[current_network.id].aps[j].channel){
                            //пытаемся переключить канал
                            try_to_switch_channels();
                        }
                        start_time = cur_time = time(NULL);
                        while((difftime(cur_time , start_time)) < 1.5){
                            rr = airodump_receive(&ci, buf, sizeof(buf), &tv);
                            if(!rr)  airodump_parse_packet(&ci, 0);
                            if(!valid_packet(&ci)) {
                                continue;
                            }
                            if(equal_macs(ci.ap.bssid.mac, available_networks[current_network.id].aps[j].bssid.mac)){
                                ap_sig = ci.ap.avg_sig;
                                break;
                            }
                            cur_time = time(NULL);
                        }
                        if((difftime(cur_time , start_time)) >= 1.5) ap_sig = -MAX_ABS_SIG;
                        if (ap_sig > max){
                            max = ap_sig;
                        }
                    }
                    mouse_pressed = 0;
                    current_avg_signal = max;
                    ((*points)[points->size()-1]).sig = current_avg_signal;
                    sprintf(message, "Point %u was added\nAverage singal: %d\n", (unsigned int)points->size()-1, current_avg_signal);
                    emit point_added_message(QString(message));
                    emit update_current_signal(current_avg_signal);
                    start_getting_signal = time(NULL);
                    current_avg_signal = -MAX_ABS_SIG;
                }
            }
        }
    }
}

void airodump_client::close_client(){
    airodump_del_device(current_device_id);
    airodump_finish();
}

airodump_client::~airodump_client(){
    close_client();
}
