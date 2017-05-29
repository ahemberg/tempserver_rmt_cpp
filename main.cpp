#include <iostream>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define SENSOR_PATH "/sys/bus/w1/devices"

using namespace std;

string get_sensor_serial() {
    /*
     * Reads out the sensor serial from the device list
     */
    char path[] = SENSOR_PATH;
    char sensor_sn[16];

    string sensor = "";

    sensor_sn[0] = 0;

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

float read_temp(string sensor_sn_str) {
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
        float tempC = strtof(tmpData, NULL)/1000;
        return tempC;
    }
    close(fd);
}

int main() {
    string sensor_serial;
    float temp;

    sensor_serial = get_sensor_serial();
    if (sensor_serial == "") {
        perror("Unable to get sensor serial");
        return 1;
    }

    temp = read_temp(sensor_serial);

    if (temp == 999) {
        perror("Unable to read temperature");
        return 1;
    }

    cout << sensor_serial << endl;
    cout << temp << " C" << endl;

    return 0;
}