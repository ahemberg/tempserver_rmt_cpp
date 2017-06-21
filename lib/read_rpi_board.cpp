//
// Created by A547832 on 2017-06-20.
// Functions for reading RPI board info
//

#include "read_rpi_board.h"

std::string exec(const char* cmd) {
    /*
     * Reads stoud from cmd. Kindly taken form StackOverflow
     */

    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    return result;
}

bool get_rpi_serial(std::string *s_no, bool set_null_if_fail) {

    std::ifstream i("/proc/cpuinfo");
    std::string line;
    std::string serial;

    if (i) {
        while (getline(i, line)) {
            if (line.find("Serial") != std::string::npos) {
                *s_no = line.substr(line.length() - 16);
                return true;
            }
        }
        std::cerr << "Could not find serial in \"/proc/cpuinfo\"" << std::endl;
    }
    else {
        std::cerr << "Failed to read \"/proc/cpuinfo\"" << std::endl;
    }
    if (set_null_if_fail) {
        *s_no = "0000000000000000";
    }
    return false;
}

bool get_cpu_temp(double *temp, bool set_neg_if_fail) {

    std::ifstream i("/sys/class/thermal/thermal_zone0/temp");
    std::string line;

    if (i) {
        while (getline(i, line)) {
            if (line.length() == 5) {
                try {
                    *temp = std::stod(line) / 1000;
                    return true;
                } catch (std::invalid_argument &e) {
                    std::cerr << "Error Converting to double" << std::endl;
                    std::cerr << "Read:" << line << std::endl;
                    std::cerr << "e.what(): " << e.what() << std::endl;
                }
            }
        }
    } else {
        std::cerr << "Failed to read \"/sys/class/thermal/thermal_zone0/temp\"" << std::endl;
    }
    if (set_neg_if_fail) *temp = -1;
    return false;
}

bool get_ram_info(board_memory *mem_info) {
    /*
     * Reads RAM memory status from /proc/meminfo. Reads out the same parameters as top/htop.
     * any parameters at -1 indicated that they were not read
     */
    std::ifstream i("/proc/meminfo");
    std::string line;
    std::string desc, unit;
    bool read_all_ok = true;
    double  size = -1, mem_total= -1, mem_free = -1, buf = -1, cached = -1, s_reclaimable = -1, sh_mem = -1,
            swap_total = -1, swap_free = -1;

    if (i) {
        while (std::getline(i, line)) {
            std::istringstream iss(line);
            iss >> desc >> size >> unit;

            if (unit.compare("kB") == 0) {
                size *= 1000;
            } else if (unit.compare("MB") == 0) {
                size *= 1000000;
            } else if (unit.compare("GB") == 0) {
                size *= 1000000000;
            }

            desc.pop_back();

            if (desc.compare("MemTotal") == 0) {
                mem_total = size;
            } else if (desc.compare("MemFree") == 0) {
                mem_free = size;
            } else if (desc.compare("Buffers") == 0) {
                buf = size;
            } else if (desc.compare("Cached") == 0) {
                cached = size;
            } else if (desc.compare("SReclaimable") == 0) {
                s_reclaimable = size;
            } else if (desc.compare("Shmem") == 0) {
                sh_mem = size;
            } else if (desc.compare("SwapTotal") == 0) {
                swap_total = size;
            } else if (desc.compare("SwapFree") == 0) {
                swap_free = size;
            }

        }
        mem_info->mem_total = mem_total;
        mem_info->mem_free = mem_free;
        mem_info->total_used = mem_total-mem_free;
        mem_info->non_cabuf_mem = (mem_total-mem_free)-(buf+cached);
        mem_info->buffers = buf;
        mem_info->cache = cached + s_reclaimable - sh_mem;
        mem_info->swap = swap_total - swap_free;

        if (mem_total == -1) {
            read_all_ok = false;
            std::cerr << "Failed to read \"MemTotal\", some parameters might be invalid" << std::endl;
        }
        if (mem_free == -1) {
            read_all_ok = false;
            std::cerr << "Failed to read \"MemFree\", some parameters might be invalid" << std::endl;
        }
        if (buf == -1) {
            read_all_ok = false;
            std::cerr << "Failed to read \"Buffers\", some parameters might be invalid" << std::endl;
        }
        if (cached == -1) {
            read_all_ok = false;
            std::cerr << "Failed to read \"Cached\", some parameters might be invalid" << std::endl;
        }
        if (s_reclaimable == -1) {
            read_all_ok = false;
            std::cerr << "Failed to read \"SReclaimable\", some parameters might be invalid" << std::endl;
        }
        if (sh_mem == -1) {
            read_all_ok = false;
            std::cerr << "Failed to read \"Shmem\", some parameters might be invalid" << std::endl;
        } if (swap_total == -1) {
            read_all_ok = false;
            std::cerr << "Failed to read \"SwapTotal\", some parameters might be invalid" << std::endl;
        }
        if (swap_free == -1) {
            read_all_ok = false;
            std::cerr << "Failed to read \"SwapFree\", some parameters might be invalid" << std::endl;
        }
        return read_all_ok;
    } else {
        std::cerr << "Failed to read \"/sys/class/thermal/thermal_zone0/temp\"" << std::endl;
    }
    return false;
}

double convert_to_bytes(char unit, double value) {
    /*
     * Converts value in unit to bytes. Unit can be K,M,G,T,
     */

    if (unit=='K') return value*1e3;
    if (unit=='M') return value*1e6;
    if (unit=='G') return value*1e9;
    if (unit=='T') return value*1e12;
    std::cerr << "WARNING: no unit matched in call convert_to_bytes(\'" << unit << "\', " << value << ")" << std::endl;
    return value;
}

bool get_hdd_usage(board_disk_status *dsk) {
    std::string cmd_out;
    std::string file_system, size_s, used_s, available_s, used_p_s, mo;
    char unit_s, unit_u, unit_a;

    cmd_out = exec("df -h /");

    std::istringstream iss(cmd_out);
    iss.ignore(128, '\n');
    iss >> file_system >> size_s >> used_s >> available_s >> used_p_s >> mo;

    unit_s = size_s.back();
    unit_u = used_s.back();
    unit_a = available_s.back();

    size_s.pop_back();
    used_s.pop_back();
    available_s.pop_back();
    used_p_s.pop_back();

    try {
        dsk->size = convert_to_bytes(unit_s, std::stod(size_s));
        dsk->used = convert_to_bytes(unit_u, std::stod(used_s));
        dsk->available = convert_to_bytes(unit_a, std::stod(available_s));
        dsk->used_p = std::stod(used_p_s)/100;
        return true;
    } catch (std::invalid_argument &e) {
        std::cerr << "Error Converting to double" << std::endl;
        std::cerr << "e.what(): " << e.what() << std::endl;
        return false;
    }

}