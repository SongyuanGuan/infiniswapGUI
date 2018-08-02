# infiniswapGUI
* MAC OS mysql login <br>
/usr/local/mysql/bin/mysql -u root -p

* create database
CREATE DATABASE infiniswap

* general information table <br>
CREATE TABLE general_info( id INT NOT NULL AUTO_INCREMENT, device_num INT NOT NULL, bd_num INT NOT NULL, daemon_num INT NOT NULL, RAM_free INT NOT NULL, RAM_filter_free INT NOT NULL, RAM_allocated INT NOT NULL, RAM_mapped INT NOT NULL, time TIMESTAMP NOT NULL, pagein_throughput INT NOT NULL, pageout_throughput INT NOT NULL, pagein_latency INT NOT NULL, pageout_latency INT NOT NULL, total_IO INT NOT NULL, remote_IO INT NOT NULL, PRIMARY KEY ( id ));


* block device table <br>
CREATE TABLE block_device(  id INT NOT NULL AUTO_INCREMENT, dev_ip VARCHAR(16) NOT NULL, pagein_throughput INT NOT NULL, pageout_throughput INT NOT NULL, pagein_latency INT NOT NULL, pageout_latency INT NOT NULL, high_pagein_latency INT NOT NULL, low_pagein_latency INT NOT NULL, high_pageout_latency INT NOT NULL, low_pageout_latency INT NOT NULL, total_IO INT NOT NULL, remote_IO INT NOT NULL, time TIMESTAMP NOT NULL, PRIMARY KEY ( id ) );

* daemon table <br>
CREATE TABLE daemon(  id INT NOT NULL AUTO_INCREMENT, dev_ip VARCHAR(16) NOT NULL, RAM_free INT NOT NULL, RAM_filter_free INT NOT NULL, RAM_mapped INT NOT NULL, RAM_allocated INT NOT NULL, time timestamp, PRIMARY KEY ( id ) );

* daemon_mem table <br>
CREATE TABLE daemon_mem( id INT NOT NULL AUTO_INCREMENT, dev_ip VARCHAR(16) NOT NULL, mem_status VARCHAR(33) NOT NULL, time TIMESTAMP NOT NULL, PRIMARY KEY ( id ) );

* daemon_mapping table <br>
CREATE TABLE daemon_mapping( id INT NOT NULL AUTO_INCREMENT, dev_ip VARCHAR(16) NOT NULL, remote_ip VARCHAR(16) NOT NULL, local_chunk INT NOT NULL, remote_chunk INT NOT NULL, time TIMESTAMP NOT NULL, PRIMARY KEY ( id ) );

* bd_mapping table <br>
CREATE TABLE bd_mapping( id INT NOT NULL AUTO_INCREMENT, dev_ip VARCHAR(16) NOT NULL, remote_ip VARCHAR(16) NOT NULL, pagein_throughput INT NOT NULL, pageout_throughput INT NOT NULL, pagein_latency INT NOT NULL, pageout_latency INT NOT NULL, time TIMESTAMP NOT NULL, PRIMARY KEY ( id ) );

* install piechart on grafana <br>
sudo grafana-cli plugins install grafana-piechart-panel

* On the server device: <br>
1. install mysql and grafana <br>
2. go to mysql and create the database and tables 

* setup server <br>
1. modify server_ip in GUI/socket/grafana_socket.h to the public ip address of the server <br>
2. sudo ./setup/setupGUI.sh server 1 [serverport] [clientport] <br>
(serverport: the port number of the server, clientport: the port number of the clients)

* setup client <br>
1. modify server_ip in GUI/socket/grafana_socket.h to the public ip address of the server <br>
2. sudo ./setup/setupGUI.sh client [ip] [serverport] [clientport] <br>
(ip: the ip address of client, serverport: the port number of the server, clientport: the port number of the clients)

* view dashboard <br>
1. open a web browser and go to "[serverip]:3000" where [serverip] is the server's public ip address. <br>
2. the default username and password are both "admin" <br>
3. click the configuration button on the left and click "add data source"
4. In the setting window, choose Type as MySQL, Name: Infiniswap; Host: localhost:3306 (should be default); Database: infiniswap; User: root; Password: mysql. Then click "Save & Test", if everything goes well, it should appears "Database Connection OK".
4. click the plus sign on the left and paste the three json files in GUI/grafana/ directory respectively to create three dashboards. <br>


