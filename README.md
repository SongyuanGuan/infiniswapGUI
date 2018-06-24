# infiniswapGUI
* MAC OS mysql login <br>
/usr/local/mysql/bin/mysql -u root -p

* general information table <br>
CREATE TABLE general_info( id INT NOT NULL AUTO_INCREMENT, device_num INT NOT NULL, bd_num INT NOT NULL, daemon_num INT NOT NULL, RAM_free INT NOT NULL, RAM_filter_free INT NOT NULL, RAM_allocated INT NOT NULL, RAM_mapped INT NOT NULL, time TIMESTAMP NOT NULL, pagein_throughput INT NOT NULL, pageout_throughput INT NOT NULL, pagein_latency INT NOT NULL, pageout_latency INT NOT NULL, total_IO INT NOT NULL, remote_IO INT NOT NULL, PRIMARY KEY ( id ));


* block device table <br>
CREATE TABLE block_device(  id INT NOT NULL AUTO_INCREMENT, dev_ip VARCHAR(16) NOT NULL, pagein_throughput INT NOT NULL, pageout_throughput INT NOT NULL, pagein_latency INT NOT NULL, pageout_latency INT NOT NULL, total_IO INT NOT NULL, remote_IO INT NOT NULL, time TIMESTAMP NOT NULL, PRIMARY KEY ( id ) );

* daemon table <br>
CREATE TABLE daemon(  id INT NOT NULL AUTO_INCREMENT, dev_ip VARCHAR(16) NOT NULL, RAM_free INT NOT NULL, RAM_filter_free INT NOT NULL, RAM_mapped INT NOT NULL, RAM_allocated INT NOT NULL, time timestamp, PRIMARY KEY ( id ) );

* daemon_mem table <br>
CREATE TABLE daemon_mem( id INT NOT NULL AUTO_INCREMENT, dev_ip VARCHAR(16) NOT NULL, mem_status VARCHAR(33) NOT NULL, time TIMESTAMP NOT NULL, PRIMARY KEY ( id ) );

* daemon_mapping table <br>
CREATE TABLE daemon_mapping( id INT NOT NULL AUTO_INCREMENT, dev_ip VARCHAR(16) NOT NULL, remote_ip VARCHAR(16) NOT NULL, local_chunk INT NOT NULL, remote_chunk INT NOT NULL, time TIMESTAMP NOT NULL, PRIMARY KEY ( id ) );

* install piechart on grafana <br>
sudo grafana-cli plugins install grafana-piechart-panel
