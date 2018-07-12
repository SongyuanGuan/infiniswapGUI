#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cmath>
#include "grafana_socket.h"

using namespace std;

const char *ip = "192.168.0.57";
const char *swap_area = "/dev/infiniswap0";
int last_version = -1;
char *write_latency_files[] = {"/tmp/bd_write_latency_1", "/tmp/bd_write_latency_2", "/tmp/bd_write_latency_3"};
char *read_latency_files[] = {"/tmp/bd_read_latency_1", "/tmp/bd_read_latency_2", "/tmp/bd_read_latency_3"};
char *bd_info_files[] = {"/tmp/bd_info_1", "/tmp/bd_info_2", "/tmp/bd_info_3"};

class latency_t
{
  public:
    vector<unsigned> read;
    vector<unsigned> write;

    void sort()
    {
        std::sort(read.begin(), read.end());
        std::sort(write.begin(), write.end());
    }

    void insert(unsigned latency, bool is_write)
    {
        if (is_write)
        {
            write.push_back(latency);
        }
        else
        {
            read.push_back(latency);
        }
    }

    void clear(){
        read.resize(0);
        write.resize(0);
    }
};

latency_t all_latency;

void read_file(char *filename, unsigned size, bool is_write)
{
    ifstream ifile;
    ifile.open(filename);

    for (int i = 0; i < size; i++)
    {
        string s_latency;
        ifile >> s_latency;
        size_t pos = s_latency.find_first_not_of('\0');
        string ss_latency = s_latency.substr(pos, s_latency.size());
        all_latency.insert(atoi(ss_latency.c_str()), is_write);
    }

    ifile.close();
    remove(filename);
}

//0 < range <= 1
unsigned calculate_proportion(float range, bool is_write)
{
    if (is_write)
    {
        int pos = (int)ceil(all_latency.write.size() * range) - 1;
        return all_latency.write[pos];
    }
    else
    {
        int pos = (int)ceil(all_latency.read.size() * range) - 1;
        return all_latency.read[pos];
    }
}

void read_tp_and_latency(request_msg &msg)
{
    ifstream ifile;
    int version;

    ifile.open("/tmp/bd_version");
    ifile >> version;
    ifile.close();
    cout << "bd file version is: " << version << endl;

    ifile.open(bd_info_files[version]);
    ifile >> msg.IO.pagein_speed >> msg.IO.pageout_speed >> msg.IO.total_IO >> msg.IO.remote_IO;
    ifile.close();

    read_file(read_latency_files[version], msg.IO.pagein_speed, false);
    read_file(write_latency_files[version], msg.IO.pageout_speed, true);

    all_latency.sort();

    msg.IO.pagein_latency = calculate_proportion(0.5, false);
    msg.IO.high_pagein_latency = calculate_proportion(0.9, false);
    msg.IO.low_pagein_latency = calculate_proportion(0.1, false);

    msg.IO.pageout_latency = calculate_proportion(0.5, true);
    msg.IO.high_pageout_latency = calculate_proportion(0.9, true);
    msg.IO.low_pageout_latency = calculate_proportion(0.1, true);
}

void send_to_server(request_msg &msg)
{
    int sock;
    struct sockaddr_in server;
    char buf[1024];
    /* Create socket. */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("opening stream socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_port = htons(hostport);
    if (connect(sock, (struct sockaddr *)&server, sizeof server) == -1)
    {
        perror("connecting stream socket");
        exit(1);
    }

    strcpy(msg.ip, ip);
    msg.time = time(0);

    send(sock, &msg, sizeof(request_msg), 0);
    close(sock);
}

void read_bd(request_msg &msg)
{
    char cmd[100];
    sprintf(cmd, "swapon -s | grep %s | wc -l > /tmp/bd_on", swap_area);
    system(cmd);
    ifstream ifile;
    ifile.open("/tmp/bd_on");
    ifile >> msg.bd_on;
    ifile.close();
    if (msg.bd_on)
    {
        read_tp_and_latency(msg);
    }
}

void read_daemon(request_msg &msg)
{
    ifstream ifile;
    ifile.open("/tmp/daemon");
    ifile >> msg.daemon_on;
    if (msg.daemon_on)
    {
        int version;
        ifile >> version;
        //valid data if version is different
        if (version != last_version)
        {
            cout << version << endl;
            last_version = version;
            ifile >> msg.ram.free >> msg.ram.filter_free >> msg.ram.allocated_not_mapped >>
                msg.ram.mapped >> msg.mapping.mem_status;
            for (int i = 0; i < msg.ram.mapped; i++)
            {
                ifile >> msg.mapping.map_infos[i].remote_ip >> msg.mapping.map_infos[i].remote_chunk_num;
            }
        }
        else
        {
            msg.daemon_on = false;
        }
    }
    ifile.close();
}

int main(int argc, char **argv)
{
    while (true)
    {
        request_msg msg;
        memset(&msg, 0, sizeof(request_msg));
        read_bd(msg);
        read_daemon(msg);
        send_to_server(msg);
        sleep(send_interval);
    }
}