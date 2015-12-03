#ifndef AIRODUMP_CLIENT_H
#define AIRODUMP_CLIENT_H

#include <airodump-lib.h>
#include <signal.h>
#include <vector>
#include <ctime>
#include <QProgressBar>
#include <QObject>
#include "point.h"
#include <QLabel>
#include "settings.h"
#include <QGraphicsView>
#include <settings.h>

#define NETWORK_NOT_SELECTED -1
#define THREAD_LOCK_REQUEST 1
#define THREAD_LOCKED_ACKNOWLEDGE 2
#define THREAD_FREE 0

typedef struct NETWORK_ID{
    int id;
    struct AIRODUMP_MAC_INFO bssid;
    unsigned char *name;
    int channel;
    std::vector<AIRODUMP_AP_INFO> aps;
} NETWORK_ID;

class airodump_client : public QObject{
    Q_OBJECT
public:
    airodump_client(Settings* s);
    airodump_client(void* s);

    int pause;
    QString device; //имя текущего устройства (wlan0, wlan1, ...)
    int current_avg_signal;
    time_t start_getting_signal;
    Settings* settings;
    int current_device_id;
    airodump_time_t tv0, tv;
    char buf[AIRODUMP_MAX_BUF_SIZE];
    struct AIRODUMP_CAPTURE_INFO ci;
    int r, n; //service values
    volatile int stop;
    volatile int mouse_pressed;
    std::vector<NETWORK_ID> available_networks;
    int amount_of_found_networks;
    int current_signal_level;
    NETWORK_ID current_network;
    std::vector<Point>* points;

    /*
     * проверка имени (не пустая ли строка и не NULL)
     */
    int valid_name(const unsigned char* name);

    /*
     * проверка пакета (active и 1 <= channel <= 13)
     */
    int valid_packet(AIRODUMP_CAPTURE_INFO* ci);

    /*
     * сравнивание два MAC-адреса (формат - 12 символов unsigned char), если они равны - вернет 1, иначе - 0
     */
    int equal_macs(unsigned char* a, unsigned char* b);

    /*
     * попытка переключить каналы (если не получилось - пытаемся перезагрузить устройство)
     * были ситуации, когда в какой-то момент по неизвестной причине устройство переставало переключать каналы -
     * пришлось сделать так
     */
    void try_to_switch_channels();

    /*
     * добавление новой AP к списку AP сети с номером id
     */
    void add_new_ap_to_existing_network(int id);

    /*
     * добавление новой сети network к списку сетей
     */
    void add_new_network(NETWORK_ID network);

    /*
     * включение каналов, на которых вещают AP(Access Points - точки доступа) текущей сети (тек. сеть определяется через переменную current_network класса)
     */
    void enable_channels_for_current_network();

    /*
     * завершение работы
     */
    void close_client();

    /*
     *  включение непересекающихся каналов 1, 6, 11
     */
    void enable_disjoint_channels();

    /*
     * включение всех каналов
     */
    void enable_all_channels();

    ~airodump_client();
public slots:
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
    void scan();
signals:
    void update_current_network_aps_label(int);
    void update_networks_list(int id, char* name); //при появлении новой сети - id и имя
    void update_current_signal(int); // обновление сигнала на главном окне - уровень сигнала
    void point_added_message(QString); // сообщение о добавлении новой точке на г.о. - сообщение
};

#endif // AIRODUMP_CLIENT_H
