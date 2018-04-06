
void i2c_master_init(const unsigned long c);
void i2c_master_repeated_start();
char i2c_master_read();
static void i2c_master_wait();
void i2c_master_start();
void i2c_master_stop();
void i2c_master_write(char address);
void i2c_slave_init(short address);
char i2c_master_read_nack();
void i2c_master_read_block(char a[], unsigned bytes);

