#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include "grafana_socket.h"
#include "mysql.h"

using namespace std;

const int device_num = 5; // (max) total number of divices
const int delay_time = 5; // process data before delay_time

class compare_by_time
{
  public:
    bool operator()(request_msg m1, request_msg m2)
    {
        return m1.time > m2.time;
    }
};

class all_info
{
  public:
    priority_queue<request_msg, vector<request_msg>, compare_by_time> infos;
    void add_info(request_msg msg)
    {
        infos.push(msg);
    }
    //select infos from time to time+interval
    void select_infos(time_t time, vector<request_msg> *msgs)
    {
        while (true)
        {
            if (infos.empty())
            {
                break;
            }
            if (infos.top().time < time)
            {
                infos.pop();
            }
            else if (infos.top().time < time + process_interval)
            {
                msgs->push_back(infos.top());
                infos.pop();
            }
            else
            {
                break;
            }
        }
    }
};

MYSQL *conn;
all_info infos;

static int server_init()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        cerr << "cannot open stream socket!\n";
        exit(1);
    }

    int on = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    //bind the port number to the socket
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("128.110.96.95");
    addr.sin_port = htons(hostport);
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    socklen_t length = sizeof(addr);
    if (getsockname(sock, (struct sockaddr *)&addr, &length) == -1)
    {
        exit(1);
    }

    //starting listen, with buffer set to 10
    listen(sock, 10);
    return sock;
}

static void put_data_into_mysql(char *str)
{
    int res = mysql_query(conn, str);

    if (!res)
    {
        printf("Inserted %lu rows", (unsigned long)mysql_affected_rows(conn));
    }
    else
    {
        fprintf(stderr, "Insert error %d: %s\n", mysql_errno(conn),
                mysql_error(conn));
    }
}

static void process_data()
{
    while (true)
    {
        // calculate the total RAM, average speed and latency
        vector<request_msg> msgs;
        time_t t = time(0);
        t -= (process_interval + delay_time);
        infos.select_infos(t, &msgs);
        ram_t total_ram;
        memset(&total_ram, 0, sizeof(ram_t));
        IO_para avg_IO;
        memset(&avg_IO, 0, sizeof(IO_para));
        int total_bd = 0, total_daemon = 0;
        unordered_set<string> ips;
        unordered_map<string, int> ip_times;
        unordered_map<string, request_msg> ip_average;
        if (!msgs.empty())
        {
            for (request_msg info : msgs)
            {
                string info_ip = info.ip;
                if (ips.find(info_ip) == ips.end())
                {
                    ips.insert(info_ip);
                    ip_times[info_ip] = 1;
                    ip_average[info_ip] = info;
                    if (info.bd_on)
                    {
                        total_bd++;
                    }
                    if (info.daemon_on)
                    {
                        total_daemon++
                    }
                }
                else
                {
                    ip_times[info_ip]++;
                    //sum up
                    ip_average[info_ip].ram = ip_average[info_ip].ram + info.ram;
                    ip_average[info_ip].IO = ip_average[info_ip].IO + info.IO;
                }
            }
            for (string ip : ips)
            {
                //get average for each ip
                ip_average[ip].ram = ip_average[ip].ram / ip_times[ip];
                ip_average[ip].IO = ip_average[ip].IO / ip_times[ip];
                total_ram = total_ram + ip_average[ip].ram;
                avg_IO = avg_IO + ip_average[ip].IO;
            }
            if (total_bd > 1)
            {
                avg_IO = avg_IO / total_bd;
            }
        }
        tm *my_tm = localtime(&t);
        char timestamp[30];
        sprintf(timestamp, "%d-%d-%d %d:%d:%d", 1900 + my_tm->tm_year,
                my_tm->tm_mon, my_tm->tm_mday, my_tm->tm_hour, my_tm->tm_min, my_tm->tm_sec);

        cout << msgs.size() << endl;
        // put the data into mysql
        char str[200];
        sprintf(str,
                "INSERT INTO general_info (pagein_speed, pageout_speed, pagein_latency, pageout_latency, time, device_num, bd_num, daemon_num, RAM_free, RAM_filter_free, RAM_allocated, RAM_mapped) VALUES (%d, %d, %d, %d, NOW(), %d, %d, %d, %d, %d, %d, %d)",
                avg_IO.pagein_speed, avg_IO.pageout_speed, avg_IO.pagein_latency, avg_IO.pageout_latency,
                ips.size(), total_bd, total_daemon, total_ram.free, total_ram.filter_free, total_ram.allocated_not_mapped, total_ram.mapped);
        cout << str << endl;

        put_data_into_mysql(str);

        sleep(process_interval);
    }
}

static void process_request(request_msg msg)
{
    cout << "ip is " << msg.ip << endl;
    //cout << "time is " << msg.time << " " << ctime(&msg.time) << endl;
    infos.add_info(msg);
    if (msg.bd_on)
    {
        char str[200];
        sprintf(str,
                "INSERT INTO block_device (dev_ip, pagein_speed, pageout_speed, pagein_latency, pageout_latency, time) VALUES ('%s', %d, %d, %d, %d, NOW())",
                msg.ip, msg.IO.pagein_speed, msg.IO.pageout_speed, msg.IO.pagein_latency, msg.IO.pageout_latency);
        cout << str << endl;
        put_data_into_mysql(str);
    }
}

static void deal_request(int msgsock)
{
    //receive message
    request_msg msg;
    recv(msgsock, &msg, sizeof(msg), MSG_WAITALL);
    process_request(msg);
    close(msgsock);
}

//listen to the clients' requests
static void server_listen(int sock)
{
    while (true)
    {
        time_t t = time(0);
        cout << "time: " << t << " " << ctime(&t) << endl;
        cout << "ready to accept message\n";

        int msgsock = accept(sock, (struct sockaddr *)0, (socklen_t *)0);
        if (msgsock == -1)
        {
            cerr << "Error: accept socket connection\n";
        }
        else
        {
            //thread tt(deal_request, msgsock);
            //tt.detach();
            deal_request(msgsock);
        }
    }
}

void connect_to_mysql()
{
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        printf("mysql_init failed!\n");
        exit(EXIT_FAILURE);
    }

    conn = mysql_real_connect(conn, "127.0.0.1", "root", "mysql", "Infiniswap",
                              0, NULL, 0);

    if (conn)
    {
        printf("Connection success!\n");
        FILE *fp = NULL;
    }
    else
    {
        printf("Connection failed!\n");
    }
}

int main()
{
    connect_to_mysql();
    int sock = server_init();
    thread dataprocessing_t(process_data);
    dataprocessing_t.detach();
    server_listen(sock);

    return 0;
}
