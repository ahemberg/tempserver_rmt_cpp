//
// Created by A547832 on 2017-06-05.
//


#ifndef TEMPSERVER_RMT_CPP_TALKTOSERVER_H
#define TEMPSERVER_RMT_CPP_TALKTOSERVER_H

#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "json.hpp"
#include "structs.h"

class TalkToServer {
private:

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

public:

    int server_response_code;
    std::string server_response_msg, encoded_post, raw_server_response = "";
    nlohmann::json server_message, server_saved_data;
    remote_info remote_data;

    //TalkToServer(remote_info rem_info, temperature_vector temps_to_send);

    bool post_to_server(std::string post, std::string server_address = "");
    void url_encode(const std::string &value);
    bool parse_server_response();
};

#endif //TEMPSERVER_RMT_CPP_TALKTOSERVER_H
