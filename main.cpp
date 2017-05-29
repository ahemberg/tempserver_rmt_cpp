#include <iostream>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define SENSOR_PATH "/sys/bus/w1/devices"

using namespace std;

float read_temp() {
    char path[] = SENSOR_PATH;
    char sensor_sn[16];
    char sensor_path[128];
    char buf[256];
    char tmpData[6];
    ssize_t numRead;

    sensor_sn[0] = 0;

    DIR *dir;
    struct dirent *dirent;

    dir = opendir(path);

    if (dir != NULL) {
        while ((dirent = readdir(dir))) {
            if (strstr(dirent->d_name,"28-") != NULL) { //Add sensor name from database call!! Or no?
                strcpy(sensor_sn, dirent->d_name);
            }
        }

        closedir(dir);
        if (sensor_sn[0] == 0) {
            perror("No sensor found!");
            return 1;
        }
    } else {
        perror("Could not open w1 devices directory!");
        return 1;
    }

    // Assemble directory string
    sprintf(sensor_path, "%s/%s/w1_slave", path, sensor_sn);

    //cout << sensor_path << endl;

    // Read temperature
    int fd = open(sensor_path, O_RDONLY);

    if (fd == -1) {
        perror("Unable to read the sensor");
        return 1;
    }

    while (read(fd, buf, 256) > 0) {
        strncpy(tmpData, strstr(buf, "t=")+2, 5);
        float tempC = strtof(tmpData, NULL)/1000;
        //printf("Device: %s  - ", sensor_sn);
        //printf("Temp: %.3f C  ", tempC);
        //cout << endl;
        return tempC;
    }
    close(fd);


}


int main() {
    float temp;

    temp = read_temp();

    cout << temp << " C" << endl;

    return 0;
}