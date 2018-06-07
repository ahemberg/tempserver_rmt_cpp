//
// Created by A547832 on 2017-06-30.
//

#include "GetBoardStatus.h"

std::string GetBoardStatus::exec(const char* cmd) {
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

double GetBoardStatus::convert_to_bytes(char unit, double value) {
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

bool GetBoardStatus::get_rpi_serial(std::string *s_no, bool set_null_if_fail) {

  std::ifstream i("/proc/cpuinfo");
  std::string line;
  std::string serial;

  if (i) {
    while (std::getline(i, line)) {
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

bool GetBoardStatus::get_cpu_temp(GetBoardStatus::board_cpu *cpu, bool set_neg_if_fail) {

  std::ifstream i("/sys/class/thermal/thermal_zone0/temp");
  std::string line;

  if (i) {
    while (std::getline(i, line)) {
      if (line.length() == 5) {
        try {
          cpu->temp = std::stod(line) / 1000;
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
  if (set_neg_if_fail) cpu->temp = -1;
  return false;
}

bool GetBoardStatus::get_cpu_load(GetBoardStatus::board_cpu *cpu, bool set_neg_if_fail) {
  /*
  * Reads cpu load from cat /proc/stat
  */

  std::ifstream i("/proc/stat");
  std::string line, cpu_s;
  double user = -1, nice = -1, system = -1, idle = -1, iowait = -1, irq = -1, softirq = -1, total = -1;
  bool success = false;

  if (i) {
    while (std::getline(i, line)) {
      if (line.find("cpu") != std::string::npos) { //Only extracts aggregate line
        std::istringstream iss(line);
        iss >> cpu_s >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
        if(cpu_s.compare("cpu") != 0) continue;

        total = user + nice + system + idle + iowait + irq + softirq;

        user = round((user/total)*100);
        nice = (nice/total)*100;
        system =  (system/total)*100;
        idle = (idle/total)*100;
        iowait = (iowait/total)*100;
        irq = (irq/total)*100;
        softirq = (softirq/total)*100;
        success = true;
        break;
      }
    }
  }

  if (success) {
    cpu->user = user;
    cpu->nice = nice;
    cpu->system = system;
    cpu->idle = idle;
    cpu->iowait = iowait;
    cpu->irq = irq;
    cpu->softirq = softirq;
  } else if (set_neg_if_fail) {
    cpu->user = -1;
    cpu->nice = -1;
    cpu->system = -1;
    cpu->idle = -1;
    cpu->iowait = -1;
    cpu->irq = -1;
    cpu->softirq = -1;
  }
  return success;
}

bool GetBoardStatus::get_ram_info(GetBoardStatus::board_memory *mem_info) {
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
    std::string failed_to_read = "";

    if (mem_total == -1) {
      read_all_ok = false;
      mem_info->total_used = -1;
      mem_info->non_cabuf_mem = -1;
      failed_to_read += " \"MemTotal\" ";
    }
    if (mem_free == -1) {
      read_all_ok = false;
      mem_info->total_used = -1;
      mem_info->non_cabuf_mem = -1;
      failed_to_read += " \"MemFree\" ";
    }
    if (buf == -1) {
      read_all_ok = false;
      mem_info->non_cabuf_mem = -1;
      failed_to_read += " \"Buffers\" ";
    }
    if (cached == -1) {
      read_all_ok = false;
      mem_info->non_cabuf_mem = -1;
      mem_info->cache = -1;
      failed_to_read += " \"Cached\" ";
    }
    if (s_reclaimable == -1) {
      read_all_ok = false;
      mem_info->cache = -1;
      failed_to_read += " \"SReclaimable\" ";
    }
    if (sh_mem == -1) {
      read_all_ok = false;
      mem_info->cache = -1;
      failed_to_read += " \"Shmem\" ";
    } if (swap_total == -1) {
      read_all_ok = false;
      mem_info->swap = -1;
      failed_to_read += " \"SwapTotal\" ";
    }
    if (swap_free == -1) {
      read_all_ok = false;
      mem_info->swap = -1;
      failed_to_read += " \"SwapFree\" ";
    }
    if (!read_all_ok) std::cerr << "Failed to read: "
    << failed_to_read
    << ", some parameters might be invalid" << std::endl;
    return read_all_ok;
  } else {
    std::cerr << "Failed to read \"/proc/meminfo\"" << std::endl;
    mem_info->mem_total = -1;
    mem_info->mem_free = -1;
    mem_info->total_used = -1;
    mem_info->non_cabuf_mem = -1;
    mem_info->buffers = -1;
    mem_info->cache = -1;
    mem_info->swap = -1;
  }
  return false;
}

bool GetBoardStatus::get_hdd_usage(GetBoardStatus::board_disk_status *dsk) {
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

void GetBoardStatus::ping_server(GetBoardStatus::board_network_status *net, const char *ping) {
  std::string cmd_out;
  std::string file_system;
  std::string line;
  size_t s, t;
  cmd_out = exec(ping);
  double pingstat[4] = {-1,-1,-1,-1};
  int n_send = -1, n_rec = -1;

  std::istringstream iss(cmd_out);

  while (std::getline(iss, line)) {
    s = line.find("packets transmitted,"); //Finds second last line, statistics line
    t = line.find("rtt min/avg/max/mdev");

    if (s!= std::string::npos) {
      std::istringstream iss2(line);
      std::string foo1, foo2, pploss, foo3, foo4, foo5, rtt;
      iss2 >> n_send >> foo1 >> foo2 >> n_rec >> pploss >> foo3 >> foo4 >> foo5 >> rtt;
    }
    if (t!= std::string::npos) {
      std::istringstream iss3(line);
      std::string foo6, foo7, foo8, rtt_stat, foo9, stat;
      iss3 >> foo6 >> foo7 >> foo7 >> rtt_stat >> foo9;
      std::istringstream iss4(rtt_stat);

      int i = 0;
      while (std::getline(iss4, stat, '/')) {
        pingstat[i] = std::stod(stat);
        i++;
      }
    }

    net->sent = n_send;
    net->received = n_rec;
    net->min_ms = pingstat[0];
    net->avg_ms = pingstat[1];
    net->max_ms = pingstat[2];
    net->mdev_ms = pingstat[3];
  }
}
