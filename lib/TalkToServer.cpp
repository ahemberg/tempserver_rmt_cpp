//
// Created by A547832 on 2017-06-05.
//

#include "TalkToServer.h"


size_t TalkToServer::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool TalkToServer::post_to_server(std::string post, std::string server_address) {
    CURL *curl;
    CURLcode res;
    //std::string readBuffer;

    if (server_address == "") {
        server_address = remote_data.server_address;
    }

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

void TalkToServer::url_encode(const std::string &value) {
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

    encoded_post = "data=" + escaped.str();
}


bool TalkToServer::parse_server_response() {

    size_t key_status, key_message, key_saved_data;

    //Check if string is on the expected format
    key_status = raw_server_response.find("\"status\"");
    key_message = raw_server_response.find("\"msg\"");
    key_saved_data = raw_server_response.find("\"saved_data\"");

    if (key_message == std::string::npos
        || key_saved_data == std::string::npos
        || key_status == std::string::npos) return false;


    auto server_response = nlohmann::json::parse(raw_server_response);

    server_response_code = server_response["status"];
    server_response_msg = server_response["msg"];
    server_saved_data = server_response["saved_data"];

    return true;
}
