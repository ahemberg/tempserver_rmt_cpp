#include <iostream>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

//MYSQL CONNECTOR -> Move to other file ffs
#include <cppconn/driver.h>
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

//LIBCURL
#include <curl/curl.h>

//JSON
#include "json.hpp"
#include <fstream>
#include <sstream>

#define SENSOR_PATH "/sys/bus/w1/devices"

//g++ -o <OUTPUT_FILE_NAME> <c++ file> -lcurl -lmysqlcppconn -std=c++11
//g++ -o read_temp main.cpp -lcurl -lmysqlcppconn -std=c++11

/*
 * TODO:
 * All functionality of python analogy
 * Get database parameter from external secrets file, no compiled passwords!
 * Move to separate classes
 *
 */

using namespace std;
using json = nlohmann::json;

struct db_auth {
    string host;
    string database;
    string user;
    string pwd;
};

struct saved_temp {
    int id;
    double temp;
    string timestamp;
};

struct remote_info {
    string server_address;
    string sensor_directory;
    string sensor_serial;
    int remote_id;
};

//JSON FUNCTIONS

bool load_db_param(db_auth *params) {
    /* database login_info from config
     *
     */
    ifstream i("secrets");

    if (i) {
        try {
            json j;
            i >> j;
            params->host = j["database_auth"]["host"];
            params->database = j["database_auth"]["database"];
            params->user = j["database_auth"]["user"];
            params->pwd = j["database_auth"]["password"];
        } catch (domain_error &e) {
            cout << "Bad format in secrets file!" << endl;
            return false;
        }
        return true;
    } else {
        cout << "Failed to load secrets file!" << endl;
        return false;
    }
   // return params;
}

//CURL FUNCTIONS

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string post_to_server(string server_address, string post) {
    CURL *curl;
    CURLcode res;
    string readBuffer;

    static const char *srv = server_address.c_str();
    static const char *postthis= post.c_str();

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, srv);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res)
            );

        /* always cleanup */
        curl_easy_cleanup(curl);
        return readBuffer;
    }
    return "";
}

//SQL FUNCTIONS

remote_info get_remote_info(db_auth auth) {

    string query = "SELECT * from REMOTE_INFO LIMIT 1";

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect(auth.host, auth.user, auth.pwd);
        con->setSchema(auth.database);

        stmt = con->createStatement();
        res = stmt->executeQuery(query);

        remote_info rem_info;

        while (res->next()) {
            rem_info.remote_id = res->getInt("remote_id");
            rem_info.sensor_directory = res->getString("sensor_directory");
            rem_info.server_address = res->getString("server_address");
            rem_info.sensor_serial = res->getString("sensor_serial");
        }

        delete res;
        delete stmt;
        delete con;

        return rem_info;

    } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

}

vector<saved_temp> get_saved_temperatures(db_auth auth) {

    string query = "SELECT * FROM saved_temp ORDER BY measurement_time DESC LIMIT 50";

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect(auth.host, auth.user, auth.pwd);
        con->setSchema(auth.database);

        stmt = con->createStatement();
        res = stmt->executeQuery(query);

        int res_len = res->rowsCount();
        int i = 0;

        vector<saved_temp> rows(res_len);

        while (res->next()) {
            rows[i].id = res->getInt("id");
            rows[i].timestamp = res->getString("measurement_time");
            rows[i].temp = res->getDouble("temp");
            i++;
        }
        delete res;
        delete stmt;
        delete con;
        return rows;
    } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}

bool save_temp(double temperature, db_auth auth) {

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement *prep_stmt;

        driver = get_driver_instance();
        con = driver->connect(auth.host, auth.user, auth.pwd);
        con->setSchema(auth.database);

        prep_stmt = con->prepareStatement("INSERT into saved_temp (temp) VALUES (?)");

        prep_stmt->setDouble(1,temperature);
        prep_stmt->executeQuery();
        delete(prep_stmt);
        delete(con);

    } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        return false;
    }
    return true;
}

//SENSOR FUNCTIONS

string get_sensor_serial() {
    /*
     * Reads out the sensor serial from the device list Should we maybe read from database instead???
     */
    char path[] = SENSOR_PATH;
    //char sensor_sn[16];

    string sensor = "";

    //sensor_sn[0] = 0;

    DIR *dir;
    struct dirent *dirent;

    dir = opendir(path);

    if (dir != NULL) {
        while ((dirent = readdir(dir))) {
            if (strstr(dirent->d_name,"28-") != NULL) {
                sensor = dirent->d_name;
            }
        }
        closedir(dir);
        if (sensor == "") {
            perror("No sensor found!");
            return "";
        }
    } else {
        perror("Could not open w1 devices directory!");
        return "";
    }
    return sensor;
}

double read_temp(string sensor_sn_str) {
    char path[] = SENSOR_PATH;
    char sensor_path[128];
    char buf[256];
    char tmpData[6];

    char *sensor_sn = new char[sensor_sn_str.size()+1];
    copy(sensor_sn_str.begin(), sensor_sn_str.end(), sensor_sn);
    sensor_sn[sensor_sn_str.size()] = '\0';

    // Assemble directory string
    sprintf(sensor_path, "%s/%s/w1_slave", path, sensor_sn);
    delete(sensor_sn);

    // Read temperature
    int fd = open(sensor_path, O_RDONLY);

    if (fd == -1) {
        perror("Unable to read the sensor");
        return 999;
    }

    while (read(fd, buf, 256) > 0) {
        strncpy(tmpData, strstr(buf, "t=")+2, 5);
        double tempC = strtod(tmpData, NULL)/1000;
        return tempC;
    }
    close(fd);
    return 999;
}

int main() {
    string sensor_serial;
    double temp;
    vector<saved_temp> saved_temps;
    vector<saved_temp> temps_saved_on_server;
    remote_info remote;
    db_auth sql_auth;

    //Read database authentication info
    if (!load_db_param(&sql_auth)) {
        return EXIT_FAILURE;
    };

    cout << sql_auth.host << endl;
    cout << sql_auth.database << endl;
    cout << sql_auth.user << endl;
    cout << sql_auth.pwd << endl;

    //Get locally stored remote info
    cout << "Reading remote info from database..." << endl;
    remote = get_remote_info(sql_auth);

    cout << "Remote id: " << remote.remote_id << endl;
    cout << "Sensor directory: " << remote.sensor_directory << endl;
    cout << "Sensor serial: " << remote.sensor_serial << endl;
    cout << "Server address: " << remote.server_address << endl;

    //TODO--> THIS STEP is unnecessary??
    //Get Sensor serial
    cout << "Getting sensor serial.." << endl;
    sensor_serial = get_sensor_serial();
    if (sensor_serial == "") {
        perror("Unable to get sensor serial, Exiting.");
        return EXIT_FAILURE;
    }
    cout << "Serial found! " << sensor_serial << endl;

    cout << "Reading sensor..." << endl;
    temp = read_temp(remote.sensor_serial);
    if (temp == 999) {
        perror("Unable to read temperature, Exiting.");
        return EXIT_FAILURE;
    }
    cout << "Sensor read! Temperature is " << temp << "C" << endl;

    cout << "Saving temp to local storage..." << endl;
    if (!save_temp(temp, sql_auth)) {
        perror("Unable to store to local database. Aborting");
        return EXIT_FAILURE;
    }
    cout << "Temp saved to local storage. Getting locally stored measurements.." << endl;

    saved_temps = get_saved_temperatures(sql_auth);
    cout << "The following measurements are stored locally.." << endl;

    for (unsigned int i = 0; i<saved_temps.size(); i++) {
        cout << saved_temps[i].id << " " << saved_temps[i].timestamp << " " << saved_temps[i].temp << endl;
    }
    cout << "Posting temperatures to server..." << endl;

    //Send to server
    //TEST CODE

    cout << "testing json parse" << endl;

    string json_string;

    json_string = post_to_server("https://alehem.eu/api/get_averages", "");
    cout << json_string << endl;
    auto j = json::parse(json_string);

    cout << j.size() << endl;

    for (int i = 0; i < j.size(); i++) {
        cout << "Remote: " << j[i]["remote"] << " Avg temp: " << j[i]["avg_temp"] << endl;
    }

    cout << "parsing local temps to json" << endl;

    json j2;

    for (unsigned int i = 0; i<saved_temps.size(); i++) {
        j2.push_back(json::object_t::value_type("temp",saved_temps[i].temp));
        j2 += json::object_t::value_type("measurement_time",saved_temps[i].timestamp);
        j2 += json::object_t::value_type("id",saved_temps[i].id);
    }

    //json j_vec(saved_temps);

    cout << j2.dump(2) << endl;


    return EXIT_SUCCESS;
}
