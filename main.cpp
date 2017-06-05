#include <iostream>
#include <string.h>
#include <stdio.h>
#include <vector>

//MYSQL CONNECTOR -> Move to other file ffs
#include <cppconn/driver.h>
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


//JSON
#include "json.hpp"
#include <fstream>


//MY Structs
#include "structs.h"

//Sensor read
#include "OneWireSensor.h"

#include "TalkToServer.h"

using namespace std;
using json = nlohmann::json;


//JSON FUNCTIONS

bool load_db_param(db_auth *params) {
    /* database login_info from config
     *
     */
    ifstream i("/home/alhe-remote/tempserver_remote/cpp/secrets"); // TODO Make this generic

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

bool remove_temps(db_auth auth, vector<saved_temp> temps_to_remove) {

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement *prep_stmt;

        driver = get_driver_instance();
        con = driver->connect(auth.host, auth.user, auth.pwd);
        con->setSchema(auth.database);

        prep_stmt = con->prepareStatement("DELETE FROM saved_temp WHERE id = (?) AND measurement_time = (?)");

        for (int i = 0; i < temps_to_remove.size(); i++) {
            prep_stmt->setInt(1,temps_to_remove[i].id);
            prep_stmt->setString(2,temps_to_remove[i].timestamp);
            prep_stmt->execute();
        }

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

//SENSOR AND BOARD FUNCTIONS

string get_rpi_serial() {

    ifstream i("/proc/cpuinfo");
    string line;
    string serial;

    if (!i) {
        return "0000000000000000";
    }

    while (getline(i, line)) {
        if (line.find("Serial") != string::npos) {
            serial = line.substr(line.length()- 16);
            return serial;
        }
    }
    return "00000000000000";
}

int main() {
    string sensor_serial;
    temperature_vector saved_temps;
    temperature_vector temps_saved_on_server;
    remote_info remote;
    db_auth sql_auth;
    string server_response_raw;
    json server_message_json;
    string urlencode_post;

    //Read database authentication info
    if (!load_db_param(&sql_auth)) {
        return EXIT_FAILURE;
    };

    //Get locally stored remote info
    cout << "Reading remote info from database..." << endl;
    remote = get_remote_info(sql_auth);
    remote.board_serial = get_rpi_serial();

    cout << "Remote id: " << remote.remote_id << endl;
    cout << "Remote serial: " << remote.board_serial << endl;
    cout << "Sensor directory: " << remote.sensor_directory << endl;
    cout << "Sensor serial: " << remote.sensor_serial << endl;
    cout << "Server address: " << remote.server_address << endl;

    OneWireSensor temp_sensor;

    cout << "Reading sensor..." << endl;
    temp_sensor.read_temp(remote.sensor_serial);
    if (temp_sensor.sensor_temp == 999) {
        perror("Unable to read temperature, Exiting.");
        return EXIT_FAILURE;
    }
    cout << "Sensor read! Temperature is " << temp_sensor.sensor_temp << "C" << endl;

    cout << "Saving temp to local storage..." << endl;
    if (!save_temp(temp_sensor.sensor_temp, sql_auth)) {
        perror("Unable to store to local database. Aborting");
        return EXIT_FAILURE;
    }
    cout << "Temp saved to local storage. Getting locally stored measurements.." << endl;

    saved_temps = get_saved_temperatures(sql_auth);

    for (unsigned int i = 0; i<saved_temps.size(); i++) {
        cout << saved_temps[i].id << " " << saved_temps[i].timestamp << " " << saved_temps[i].temp << endl;
    }

    // Create json object
    cout << "Parsing local temps to json" << endl;

    TalkToServer server_session;

    server_session.generate_server_message(remote, saved_temps);

    urlencode_post = "data=" + server_session.url_encode(server_session.server_message.dump());

    cout << server_session.server_message.dump(1) << endl;

    //Send to server
    cout << "Sending to server..." << endl;

    server_session.post_to_server("https://alehem.eu/api/save_temp", urlencode_post);

    cout << "raw response" << endl;
    cout << server_session.raw_server_response << endl;

    cout << "Server response:" << endl;

    server_session.parse_server_response();

    //TODO ERROR HANDLING FOR THIS STRING
    auto server_response = json::parse(server_session.raw_server_response);

    cout << server_response.dump(1) << endl;

    if (server_response["status"] == 1) {
        cout << "Server successfully saved temperatures." << endl;
        cout << "Server message: " << server_response["message"] << endl;


        saved_temp t;
        for (auto& element : server_response["saved_data"]) {
            t.timestamp = element["measurement_time"];
            t.id = element["id"];
            t.temp = element["temp"];
            temps_saved_on_server.push_back(t);
        }

        cout << "Removing data saved on server from local storage.." << endl;

        if (remove_temps(sql_auth, temps_saved_on_server)) {
            cout << "Local data removed" << endl;
            return EXIT_SUCCESS;
        }
    } else {
        cout << "Server failed to save temperatures" << endl;
        cout << "Server message: " << server_response["message"] << endl;
        cout << "Local storage has been kept." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}

