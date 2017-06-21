//
// Created by A547832 on 2017-06-21.
//

#ifndef TEMPSERVER_RMT_CPP_SENDTEMPTOSERVER_H
#define TEMPSERVER_RMT_CPP_SENDTEMPTOSERVER_H


//TODO: LOOK at these imports. Many are unnecessary!
#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "json.hpp"
#include "structs.h"

#include "TalkToServer.h"

class SendTempToServer : public TalkToServer {
private:
    void generate_server_temperature_message();
public:
    temperature_vector local_temps, temps_saved_on_server;

    SendTempToServer(remote_info rem_info, temperature_vector temps_to_send);
    void parse_saved_temperatures();
};

#endif //TEMPSERVER_RMT_CPP_SENDTEMPTOSERVER_H
