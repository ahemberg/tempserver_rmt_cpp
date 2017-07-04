//
// Created by A547832 on 2017-06-30.
//

#ifndef TEMPSERVER_RMT_CPP_GETBOARDSTATUS_H
#define TEMPSERVER_RMT_CPP_GETBOARDSTATUS_H

#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>

class GetBoardStatus {
public:

    struct board_memory {
        double  mem_total, mem_free, total_used, non_cabuf_mem, buffers, cache, swap;
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

    GetBoardStatus::board_parameters new_measurement;
    std::vector<GetBoardStatus::board_parameters> old_measurements, measurements_to_send, server_saved_messages;

    GetBoardStatus() = default;
    bool get_rpi_serial(std::string *s_no, bool set_null_if_fail = true);

    bool get_cpu_temp(board_cpu *cpu, bool set_neg_if_fail = true);
    bool get_cpu_load(board_cpu *cpu, bool set_neg_if_fail = true);

    bool get_ram_info(GetBoardStatus::board_memory *mem_info);

    bool get_hdd_usage(board_disk_status *dsk);

    void ping_server(GetBoardStatus::board_network_status *net, const char *ping = "ping -c 5 8.8.8.8");

protected:
    std::string exec(const char* cmd);
    double convert_to_bytes(char unit, double value);

private:

};

#endif //TEMPSERVER_RMT_CPP_GETBOARDSTATUS_H
