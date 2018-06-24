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
#include "grafana_socket.h"

using namespace std;

const char *ip = "192.168.0.57";
const char *swap_area = "/dev/infiniswap0";
int last_version = -1;

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
    server.sin_addr.s_addr = inet_addr("128.110.96.95");
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
        ifstream ifile;
        ifile.open("/tmp/bd_info");
        ifile >> msg.IO.pagein_speed >> msg.IO.pageout_speed >> msg.IO.total_IO >> msg.IO.remote_IO >> msg.IO.pagein_latency >> msg.IO.pageout_latency;  
        ifile.close(); 
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
            ifile >> msg.ram.free >> msg.ram.filter_free >> msg.ram.allocated_not_mapped >> msg.ram.mapped >> msg.mapping.mem_status;
            for (int i = 0; i < msg.ram.mapped; i++){
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
