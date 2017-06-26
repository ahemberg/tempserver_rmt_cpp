//
// Created by A547832 on 2017-06-22.
//

#include "SendStatusToServer.h"

nlohmann::json SendStatusToServer::create_status_blob(board_parameters *board) {
    nlohmann::json j;

    j["memory"]["mem_total"] = board->memory.mem_total;
    j["memory"]["mem_free"]  = board->memory.mem_free;
    j["memory"]["total_used"] = board->memory.total_used;
    j["memory"]["non_cabuf_mem"] = board->memory.non_cabuf_mem;
    j["memory"]["buffers"] = board->memory.buffers;
    j["memory"]["cache"] = board->memory.cache;
    j["memory"]["swap"] = board->memory.swap;

    j["disk"]["size"] = board->disk.size;
    j["disk"]["used"] = board->disk.used;
    j["disk"]["available"] = board->disk.available;
    j["disk"]["used_percent"] = board->disk.used_p;

    j["network"]["sent"] = board->network.sent;
    j["network"]["received"] = board->network.received;
    j["network"]["min_lat"] = board->network.min_ms;
    j["network"]["avg_lat"] = board->network.avg_ms;
    j["network"]["max_lat"] = board->network.max_ms;
    j["network"]["mdev_lat"] = board->network.mdev_ms;

    j["cpu"]["temp"] = board->cpu.temp;
    j["cpu"]["user"] = board->cpu.user;
    j["cpu"]["nice"] = board->cpu.nice;
    j["cpu"]["system"] = board->cpu.system;
    j["cpu"]["idle"] = board->cpu.idle;
    j["cpu"]["iowait"] = board->cpu.iowait;
    j["cpu"]["irq"] = board->cpu.irq;
    j["cpu"]["softirq"] = board->cpu.softirq;

    return j;
}