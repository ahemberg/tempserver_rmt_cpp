//
// Created by A547832 on 2017-06-03.
//

#include "OneWireSensor.h"

void OneWireSensor::get_sensor_serial() {
    /*
     * Reads out the sensor serial from the device list Might cause issues if there are multiple sensors.
     */
    char path[] = SENSOR_PATH;

    sensor_serial = "";

    DIR *dir;
    struct dirent *dirent;

    dir = opendir(path);

    if (dir != NULL) {
        while ((dirent = readdir(dir))) {
            if (strstr(dirent->d_name,"28-") != NULL) {
                sensor_serial = dirent->d_name;
            }
        }
        closedir(dir);
        if (sensor_serial == "") {
            perror("No sensor found!");
        }
    } else {
        perror("Could not open w1 devices directory!");
    }
}

void OneWireSensor::read_temp(std::string sensor_sn_str) {

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
        sensor_temp = 999;
    } else {
        while (read(fd, buf, 256) > 0) {
            strncpy(tmpData, strstr(buf, "t=")+2, 5);
            sensor_temp = strtod(tmpData, NULL)/1000;
        }
    }
    close(fd);
}