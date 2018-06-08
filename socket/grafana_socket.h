#ifndef __GRAFANA_SOCKET_H__
#define __GRAFANA_SOCKET_H__

#include <ctime>
#include <string>

const int hostport = 8000;
const int send_interval = 2; // the interval (seconds) a client sending message to server
const int process_interval = 10; // the interval (seconds) server process data
const int total_ram = 64; // 64GB

struct ram_t{
    int mapped;
    int free;
    int filter_free;
    int allocated_not_mapped; 
};

struct request_msg{
    std::string ip;
    bool bd_on;
    bool daemon_on;
    //char time[14]; // yyyymmddhhmmss
    time_t time;
    ram_t ram;
    int pageout_speed;
    int pagein_speed;
    int pageout_latency;
    int pagein_latency;
};

#endif
