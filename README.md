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
```
sudo ./setup/setupGUI.sh server 1 [serverport] [clientport] . [server_public_ip] 
```
(serverport: the port number of the server, clientport: the port number of the clients)

* setup client <br>
```
sudo ./setup/setupGUI.sh client [ip] [serverport] [clientport] . [server_public_ip]
```
(ip: the ip address of client, serverport: the port number of the server, clientport: the port number of the clients)

* view dashboard <br>
1. open a web browser and go to "[server_ip]:3000" where [server_ip] is the server's public ip address. <br>
2. the default username and password are both "admin" <br>
3. click the configuration button on the left and click "add data source" <br>
4. In the setting window, choose Type as MySQL, Name: Infiniswap; Host: localhost:3306 (should be default); Database: infiniswap; User: root; Password: mysql. Then click "Save & Test", if everything goes well, it should appears "Database Connection OK". <br>
4. click the plus sign on the left and paste the three json files in GUI/grafana/ directory respectively to create three dashboards. <br>
5. General information is shown in general_info dashboard, and detailed data of block device and daemon can be seen on block_device and daemon dashboards respectively. Note that you can select dev_ip on the top left corner, and change time range on the top right corner.

* start infiniswap on all the devices <br>
(Note that the following are all executed under setup directory.)
1. install expect and tmux on the host machine: <br>
```
sudo apt-get install expect
sudo apt-get install tmux 
```

2. modify server.list to be "{server_public_ip}:{server_private_ip}" <br>
3. modify device.list to be "{device_public_ip}:{device_private_ip}" for each device on a separate line (note that the private_ip address will be used to setup infiniswap). <br>
4. modify bd.list to be "{device_public_ip}:{device_private_ip}" for each device which is going to setup a block device <br>
5. modify connect.exp: <br>
a. set user and password as your username and password in order to ssh to remote devices (the ssh command should be "ssh {$user}@ip") <br>
b. set dir to be the directory to setup infiniswap <br>
6. to install infiniswap on each device: <br>

```
./installall.sh
```

7. modify portal.list (in infiniswap/setup/ directory) on each device which is going to have block device setup <br>
8. to setup infiniswap on each device <br>
```
./setupall.sh
```
(using 'ctrl+b' followed by 'd' to exit tmux window, use tmux attach -t [session_name] to go to a tmux session) <br>
9. to stop infiniswap on each device <br>
```
./stopall.sh
```