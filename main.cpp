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

#define SENSOR_PATH "/sys/bus/w1/devices"

//SQL DEFINES -- MOVE TO EXTERNAL FILE FFS
#define HOST "localhost"
#define DB_USR "root"
#define DB_PWD "fXDbX7N5ZWUmV2aOwFq0"
#define DB_SCHEMA "tempserver_rmt"

using namespace std;

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
  //TODO: Populate with info from remote
};

vector<remote_info> get_remote_info() {
    //TODO: Should get client info from database
}

vector<saved_temp> get_saved_temperatures() {

    string query = "SELECT * FROM saved_temp ORDER BY measurement_time DESC LIMIT 50";

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect(HOST, DB_USR, DB_PWD);
        con->setSchema(DB_SCHEMA);

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

bool save_temp(double temperature) {

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement *prep_stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect(HOST, DB_USR, DB_PWD);
        con->setSchema(DB_SCHEMA);

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

    //Get Sensor serial
    cout << "Getting sensor serial.." << endl;
    sensor_serial = get_sensor_serial();
    if (sensor_serial == "") {
        perror("Unable to get sensor serial, Exiting.");
        return 1;
    }
    cout << "Serial found! " << sensor_serial << endl;

    cout << "Reading sensor..." << endl;
    temp = read_temp(sensor_serial);
    if (temp == 999) {
        perror("Unable to read temperature, Exiting.");
        return 1;
    }
    cout << "Sensor read! Temperature is " << temp << "C" << endl;


    cout << "Saving temp to local storage..." << endl;
    if (!save_temp(temp)) {
        perror("Unable to store to local database. Aborting");
        return 1;
    }
    cout << "Temp saved to local storage. Getting locally stored measurements.." << endl;

    saved_temps = get_saved_temperatures();
    cout << "The following measurements are stored locally.." << endl;
    for (unsigned int i = 0; i<saved_temps.size(); i++) {
        cout << saved_temps[i].id << " " << saved_temps[i].timestamp << " " << saved_temps[i].temp << endl;
    }
    cout << "Posting temperatures to " << endl;

    //Send to server


    return 0;
}
