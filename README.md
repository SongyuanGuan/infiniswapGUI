# infiniswapGUI
* MAC OS mysql login
/usr/local/mysql/bin/mysql -u root -p

* general information table
CREATE TABLE general_info(
    id INT NOT NULL AUTO_INCREMENT,
    pagein_speed INT NOT NULL,
    pageout_speed INT NOT NULL,
    pagein_latency INT NOT NULL,
    pageout_latency INT NOT NULL,
    time timestamp,
    device_num INT,
    RAM_used INT,
    RAM_unused INT,
    RAM_allocated INT,
    PRIMARY KEY ( id )
    );

* block device table
CREATE TABLE block_device(  id INT NOT NULL AUTO_INCREMENT, dev_id INT NOT NULL, 
pagein_speed INT NOT NULL, pageout_speed INT NOT NULL, pagein_latency INT NOT NULL,
pageout_latency INT NOT NULL, time timestamp, PRIMARY KEY ( id ) );
