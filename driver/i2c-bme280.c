#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>


#define CTRL_MEAS_ADDR 0xF4
#define COMP_PARPAM_ADDR 0x88
#define MODE 0x1
#define OSRS_P 0x0 << 2
#define OSRS_T 0x1 << 5

struct comp_params {
	unsigned short dig_T1;
	short dig_T2;
	short dig_T3;
};

static int init_sensor(struct i2c_client *client) {
	const char opts[] = {CTRL_MEAS_ADDR, MODE | OSRS_P | OSRS_T};

	return i2c_master_send(client, opts, 2);
}

static int read_data(struct i2c_client *client, char *buf, char start_addr, int count) {
	int ret = 0;
	ret = i2c_master_send(client, &start_addr, 1);
	if (ret < 0) {
		pr_err("send: %d\n", ret);
	}
	ret = i2c_master_recv(client, buf, count);
	if (ret < 0) {
		pr_err("recv: %d\n", ret);
	}
	return ret;
}

static struct comp_params get_comp_params(struct i2c_client *client) {
	char buf[6];
	int ret;
	ret = read_data(client, buf, COMP_PARPAM_ADDR, 6);
	if (ret < 0) {
		pr_err("Unable ro read comp params: %d\n", ret);
	}

	return (struct comp_params) {
		.dig_T1 = buf[1] << 8 | buf[0],
		.dig_T2 = buf[3] << 8 | buf[2],
		.dig_T3 = buf[5] << 8 | buf[4]
	};
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
static int compensate_temp(int adc_T, struct comp_params *params)
{
	unsigned short dig_T1 = params->dig_T1;
	short dig_T2 = params->dig_T2;
	short dig_T3 = params->dig_T3;

	int var1, var2, T;
	var1 = ((((adc_T>>3) - ((int)dig_T1<<1))) * ((int)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int)dig_T1)) * ((adc_T>>4) - ((int)dig_T1))) >> 12) * ((int)dig_T3)) >> 14;
	T = ((var1 + var2) * 5 + 128) >> 8;
	return T;
}

static int get_temp(struct i2c_client *client)
{
	char recv_buf[3];
	int ret;
	int uncomp_temp;

	struct comp_params params = get_comp_params(client);
	init_sensor(client);
	ret = read_data(client, recv_buf, 0xFA, 3);
	if (ret < 0) {
		pr_err("Unable ro read temp: %d\n", ret);
	}
	uncomp_temp = recv_buf[0] << 12 | recv_buf[1] << 4 | ((recv_buf[2] >> 4) & 0x0F0);
	ret = compensate_temp(uncomp_temp, &params);
	return ret;
}

static ssize_t show_temp(struct device *dev,struct device_attribute *attr, char *buf)
{
	int temp;
	struct i2c_client *client = to_i2c_client(dev);
	// get temperature from sensor
	temp = get_temp(client);
	return sprintf(buf, "%d\n", temp);
}


static struct device_attribute dev_attr_temp = {
	.attr = {
		.name = "temperature",
		.mode = S_IRUGO,
	},
	.show = show_temp,
};


static int i2c_bme_probe(struct i2c_client *client, const struct i2c_device_id *id) {
	int ret;
	// create temperature file
	ret = device_create_file(&client->dev, &dev_attr_temp);
	pr_info("%s probe\n", client->name);
	return 0;
}
static int i2c_bme_remove(struct i2c_client *client) {
	device_remove_file(&client->dev, &dev_attr_temp);
	pr_info("%s remove", client->name);
	return 0;
}

// Für welche devices zuständg
static struct i2c_device_id i2c_bme_id[] = {
	{"bme280", 0x76},
	{ }
};

// register id in kernel
MODULE_DEVICE_TABLE(i2c, i2c_bme_id);

static struct i2c_driver i2c_bme_driver = { 
	.driver = {
		.name = "i2c-bme280",
	},
	.probe = i2c_bme_probe,
	.remove = i2c_bme_remove,
	.id_table = i2c_bme_id,
};

// init makro
module_i2c_driver(i2c_bme_driver);

MODULE_AUTHOR("Some humans");
MODULE_LICENSE("GPL");

