//
// Created by A547832 on 2017-06-22.
//

#include "SendStatusToServer.h"
/*
SendStatusToServer::SendStatusToServer(remote_info rem_info, board_vector measurements) {
    remote_data = rem_info;
    //new_measurement = status;
    //old_measurements = old;
    //combine_measurements();
    measurements_to_send = measurements;
    generate_server_status_message();
    url_encode(server_message.dump());
}
*/
void SendStatusToServer::combine_measurements() {
    measurements_to_send = old_measurements;
    measurements_to_send.push_back(new_measurement);
}

void SendStatusToServer::generate_server_status_message() {

    nlohmann::json status_record;

    for (unsigned int i = 0; i < measurements_to_send.size(); i++) {
        status_record.push_back(create_status_blob(&measurements_to_send[i]));
    }

    server_message["status"] = status_record;
    server_message["remote_id"] = remote.remote_id;
    server_message["remote_serial"] = remote.board_serial;
}

bool SendStatusToServer::parse_saved_messages() {

    if (!parse_server_response()) return false; //GETS parameters
    server_saved_messages = parse_status_blob(server_saved_data);
    return true;
}

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

    j["timestamp"] = board->timestamp;
    j["id"] = board->id;

    return j;
}

std::vector<SendStatusToServer::board_parameters> SendStatusToServer::parse_status_blob(nlohmann::json status_blob) {

    std::vector<SendStatusToServer::board_parameters> received_blobs;

    for (auto& j : status_blob) {
        board_parameters row;
        row.memory.mem_total = j["memory"]["mem_total"].get<double>();
        row.memory.mem_free = j["memory"]["mem_free"].get<double>();
        row.memory.total_used = j["memory"]["total_used"].get<double>();
        row.memory.non_cabuf_mem = j["memory"]["non_cabuf_mem"].get<double>();
        row.memory.buffers = j["memory"]["buffers"].get<double>();
        row.memory.cache = j["memory"]["cache"].get<double>();
        row.memory.swap = j["memory"]["swap"].get<double>();

        row.disk.size = j["disk"]["size"].get<double>();
        row.disk.used = j["disk"]["used"].get<double>();
        row.disk.available = j["disk"]["available"].get<double>();
        row.disk.used_p = j["disk"]["used_percent"].get<double>();

        row.network.sent = j["network"]["sent"].get<int>();
        row.network.received = j["network"]["received"].get<int>();
        row.network.min_ms = j["network"]["min_lat"].get<double>();
        row.network.avg_ms = j["network"]["avg_lat"].get<double>();
        row.network.max_ms = j["network"]["max_lat"].get<double>();
        row.network.mdev_ms = j["network"]["mdev_lat"].get<double>();

        row.cpu.temp = j["cpu"]["temp"].get<double>();
        row.cpu.user = j["cpu"]["user"].get<double>();
        row.cpu.nice = j["cpu"]["nice"].get<double>();
        row.cpu.system = j["cpu"]["system"].get<double>();
        row.cpu.idle = j["cpu"]["idle"].get<double>();
        row.cpu.iowait = j["cpu"]["iowait"].get<double>();
        row.cpu.irq = j["cpu"]["irq"].get<double>();
        row.cpu.softirq = j["cpu"]["softirq"].get<double>();

        row.timestamp = j["timestamp"].get<std::string>();
        row.id = j["id"].get<int>();

        received_blobs.push_back(row);
    }

    return received_blobs;

};