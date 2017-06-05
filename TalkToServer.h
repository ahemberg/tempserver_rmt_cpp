//
// Created by A547832 on 2017-06-05.
//

#ifndef TEMPSERVER_RMT_CPP_TALKTOSERVER_H
#define TEMPSERVER_RMT_CPP_TALKTOSERVER_H

#include <string>
#include <vector>
#include <curl/curl.h>
#include "json.hpp"
#include "structs.h"



class TalkToServer {
private:
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
public:
    nlohmann::json server_message;
    std::string raw_server_response = "";
    int server_response_code;
    std::string server_response_msg;
    temperature_vector temps_saved_on_server;

    void generate_server_message(remote_info rem_info, temperature_vector temps_to_send);
    bool post_to_server(std::string server_address, std::string post);
    std::string url_encode(const std::string &value);
    void parse_server_response();
};


#endif //TEMPSERVER_RMT_CPP_TALKTOSERVER_H
