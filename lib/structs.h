//
// Created by A547832 on 2017-06-05.
//

#ifndef TEMPSERVER_RMT_CPP_STRUCTS_H
#define TEMPSERVER_RMT_CPP_STRUCTS_H

#include <vector>
#include <string.h>

struct db_auth {
    std::string host;
    std::string database;
    std::string user;
    std::string pwd;
};

struct saved_temp {
    int id;
    double temp;
    std::string timestamp;
};

struct remote_info {
    std::string server_address;
    std::string sensor_directory;
    std::string sensor_serial;
    std::string board_serial;
    int remote_id;
};

struct board_memory {
    double  mem_total, mem_free, total_used,
            non_cabuf_mem,
            buffers,
            cache,
            swap;
};

struct board_disk_status {
    double size, used, available, used_p;
};

struct board_network_status {
    double min_ms, avg_ms, max_ms, mdev_ms;
    int sent, received;
};

struct board_cpu {
    double temp, user, nice, system, idle, iowait, irq, softirq;
};

struct board_parameters {
    board_memory memory;
    board_disk_status disk;
    board_network_status network;
    board_cpu cpu;
    std::string timestamp;
    int id;
};

typedef std::vector<saved_temp> temperature_vector;
typedef std::vector<board_parameters> board_vector;

#endif //TEMPSERVER_RMT_CPP_STRUCTS_H
