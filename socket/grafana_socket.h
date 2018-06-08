#ifndef __GRAFANA_SOCKET_H__
#define __GRAFANA_SOCKET_H__

#include <ctime>
#include <string>

const int hostport = 8000;
const int send_interval = 2; // the interval (seconds) a client sending message to server
const int process_interval = 10; // the interval (seconds) server process data
const int TOTALRAM= 64; // 64GB

struct ram_t{
    int mapped;
    int free;
    int filter_free;
    int allocated_not_mapped; 
};

ram_t operator+(const ram_t & a, const ram_t & b){
    ram_t result;
    result.mapped = a.mapped + b.mapped;
    result.free = a.free + b.free;
    result.filter_free = a.filter_free + b.filter_free;
    result.allocated_not_mapped = a.allocated_not_mapped + b.allocated_not_mapped;
    return result;
}

ram_t operator/(const ram_t & a, int b){
    ram_t result;
    result.mapped = a.mapped / b;
    result.free = a.free / b;
    result.filter_free = a.filter_free / b;
    result.allocated_not_mapped = a.allocated_not_mapped / b;
    return result;
}

struct IO_para{
    int pageout_speed;
    int pagein_speed;
    int pageout_latency;
    int pagein_latency;
};

IO_para operator+(const IO_para & a, const IO_para & b){
    IO_para result;
    result.pageout_speed = a.pageout_speed + b.pageout_speed;
    result.pagein_speed = a.pagein_speed + b.pagein_speed;
    result.pageout_latency = a.pageout_latency + b.pageout_latency;
    result.pagein_latency = a.pagein_latency + b.pagein_latency;
    return result;
}

IO_para operator/(const IO_para & a,int  b){
    IO_para result;
    result.pageout_speed = a.pageout_speed / b;
    result.pagein_speed = a.pagein_speed / b;
    result.pageout_latency = a.pageout_latency / b;
    result.pagein_latency = a.pagein_latency / b;
    return result;
}

struct request_msg{
    char ip[16];
    bool bd_on;
    bool daemon_on;
    //char time[14]; // yyyymmddhhmmss
    time_t time;
    ram_t ram;
    IO_para IO;
};

#endif
