//
// Created by A547832 on 2017-06-20.
//

#include <iostream>
#include <string>

#include "lib/read_rpi_board.h"

using namespace std;

int main() {

    string serial;
    double cpu_temp;
    //Get RPI Serial
    if (!get_rpi_serial(&serial)) {
        cerr << "Failed to get board serial" << endl;
    }
    //Get CPU temp
    if (!get_cpu_temp(&cpu_temp)) {
        cerr << "Failed to get cpu temp" << endl;
    }
    //Get CPU Usage

    //Get memory Usage

    //Get GPU onboard temps
    cout << serial << " " << cpu_temp << endl;
    //Get available disk-space
    return EXIT_SUCCESS;

}