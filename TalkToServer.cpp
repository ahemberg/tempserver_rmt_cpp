//
// Created by A547832 on 2017-06-05.
//

#include "TalkToServer.h"

size_t TalkToServer::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void TalkToServer::generate_server_message(remote_info rem_info, temperature_vector temps_to_send) {
    nlohmann::json temperatures;

    for (unsigned int i = 0; i<temps_to_send.size(); i++) {
        nlohmann::json j3;
        j3.push_back(nlohmann::json::object_t::value_type("temp",temps_to_send[i].temp));
        j3 += nlohmann::json::object_t::value_type("measurement_time",temps_to_send[i].timestamp);
        j3 += nlohmann::json::object_t::value_type("id",temps_to_send[i].id);

        temperatures.push_back(j3);
    }

    server_message["temperatures"] = temperatures;
    server_message["remote_id"] = rem_info.remote_id;
    server_message["remote_serial"] = rem_info.board_serial;
}

bool TalkToServer::post_to_server(std::string server_address, std::string post) {
    CURL *curl;
    CURLcode res;
    //std::string readBuffer;

    static const char *srv = server_address.c_str();
    static const char *postthis= post.c_str();

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, srv);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &raw_server_response);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res)
            );
            curl_easy_cleanup(curl);
            return false;
        }
        curl_easy_cleanup(curl);
        return true;
    }
    return false;
}

std::string TalkToServer::url_encode(const std::string &value) {
    //Analogue to urllib.urlencode in python.
    //taken from https://stackoverflow.com/questions/154536/encode-decode-urls-in-c

    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

void TalkToServer::parse_server_response() {
    //TODO: You were here. This does not work gives error:
    //Cannot parse JSON.
    std::cout << "server des" << std::endl;
    std::cout << raw_server_response << std::endl;
    auto server_response = nlohmann::json::parse(raw_server_response);
    server_response_code = server_response["status"];
    server_response_msg = server_response["message"];

    saved_temp t;
    for (auto& element : server_response["saved_data"]) {
        t.timestamp = element["measurement_time"];
        t.id = element["id"];
        t.temp = element["temp"];
        temps_saved_on_server.push_back(t);
    }

}
