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

const string ip = "192.168.0.57";
int last_version = -1;

void send_to_server(request_msg & msg){
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

        strcpy(msg.ip, ip.c_str());
        msg.pagein_speed = rand() % 100 + 50;
        msg.pageout_speed = rand() % 100 + 100;
        msg.pagein_latency = rand() % 20 + 10;
        msg.pageout_latency = rand() % 20 + 10;
        msg.time = time(0);
        send(sock, &msg, sizeof(request_msg), 0);
        close(sock);
}

void read_bd(request_msg & msg){

}

void read_daemon(request_msg & msg){
    ifstream ifile;
    ifile.open("/tmp/daemon");
    ifile >> msg.daemon_on;
cout << msg.daemon_on << endl;
    if (msg.daemon_on){
        int version;
        ifile >> version;
        if (version != last_version){
		cout << version << endl;
            last_version = version;
            ifile >> msg.ram.free >> msg.ram.filter_free >> msg.ram.allocated_not_mapped >> msg.ram.mapped;
        }
	else {
		msg.daemon_on = false;
	}
    }
    ifile.close();
}

int main(int argc, char** argv)
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
