//
// Created by A547832 on 2017-06-22.
//

#ifndef TEMPSERVER_RMT_CPP_SENDSTATUSTOSERVER_H
#define TEMPSERVER_RMT_CPP_SENDSTATUSTOSERVER_H


#include "TalkToServer.h"
#include "json.hpp"

class SendStatusToServer : public TalkToServer {
public:
    nlohmann::json create_status_blob(board_parameters *board);
};


#endif //TEMPSERVER_RMT_CPP_SENDSTATUSTOSERVER_H
