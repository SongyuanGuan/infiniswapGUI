#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include "grafana_socket.h"

using namespace std;

int id;

int main(int argc, char** argv)
{
    id = atoi(argv[1]);
    while (true)
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

        request_msg msg;
        msg.id = id;
        msg.pagein_speed = rand() % 100 + 50;
        msg.pageout_speed = rand() % 100 + 100;
        msg.pagein_latency = rand() % 20 + 10;
        msg.pageout_latency = rand() % 20 + 10;
        msg.time = time(0);
        msg.ram.used = rand()%10 + 16;
        msg.ram.unused = rand() % 10 + 16;
        msg.ram.allocated = total_ram - msg.ram.used - msg.ram.unused;
        send(sock, &msg, sizeof(request_msg), 0);
        close(sock);
        sleep(send_interval);
    }
}
