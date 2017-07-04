//
// Created by A547832 on 2017-06-22.
//

#ifndef TEMPSERVER_RMT_CPP_SENDSTATUSTOSERVER_H
#define TEMPSERVER_RMT_CPP_SENDSTATUSTOSERVER_H


#include "TalkToServer.h"
#include "json.hpp"
#include "RemoteStatusDB.h"

class SendStatusToServer : public TalkToServer, public RemoteStatusDB {
public:


    nlohmann::json json_blob;

    SendStatusToServer() = default;
    //SendStatusToServer(remote_info rem_info, board_vector measurements);
    nlohmann::json create_status_blob(board_parameters *board);
    void generate_server_status_message();
    bool parse_saved_messages();
    void combine_measurements();
    std::vector<SendStatusToServer::board_parameters> parse_status_blob(nlohmann::json status_blob);

};


#endif //TEMPSERVER_RMT_CPP_SENDSTATUSTOSERVER_H
