//
// Created by A547832 on 2017-06-21.
//

#ifndef TEMPSERVER_RMT_CPP_SENDTEMPTOSERVER_H
#define TEMPSERVER_RMT_CPP_SENDTEMPTOSERVER_H

#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"
//#include "structs.h"

#include "TalkToServer.h"
#include "RemoteTempDB.h"

class SendTempToServer : public TalkToServer, public RemoteTempDB {
private:
//    void generate_server_temperature_message();
public:
    void generate_server_temperature_message();
    std::vector<SendTempToServer::saved_temp> local_temps, temps_saved_on_server;
    //temperature_vector local_temps, temps_saved_on_server;
    SendTempToServer() = default;
    //SendTempToServer(std::string secrets_path  = "/tempserver_remote/cpp/secrets");
    bool parse_saved_temperatures();
};

#endif //TEMPSERVER_RMT_CPP_SENDTEMPTOSERVER_H
