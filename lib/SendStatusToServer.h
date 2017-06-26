//
// Created by A547832 on 2017-06-22.
//

#ifndef TEMPSERVER_RMT_CPP_SENDSTATUSTOSERVER_H
#define TEMPSERVER_RMT_CPP_SENDSTATUSTOSERVER_H


#include "TalkToServer.h"
#include "json.hpp"

class SendStatusToServer : public TalkToServer {
public:
    board_parameters new_measurement;
    board_vector old_measurements, measurements_to_send, server_saved_messages;
    nlohmann::json json_blob;

    SendStatusToServer(remote_info rem_info, board_vector measurements);
    nlohmann::json create_status_blob(board_parameters *board);
    void generate_server_status_message();
    bool parse_saved_messages();
    void combine_measurements();
    board_vector parse_status_blob(nlohmann::json status_blob);

};


#endif //TEMPSERVER_RMT_CPP_SENDSTATUSTOSERVER_H
