#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>


struct comp_params {
	unsigned short dig_T1;
	short dig_T2;
	short dig_T3;
};

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
	ret = read_data(client, buf, 0x88, 6);
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
// t_fine carries fine temperature as global value
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

static int i2c_bme_probe(struct i2c_client *client, const struct i2c_device_id *id) {
	char recv_buf[3];
	int ret;
	int uncomp_temp;
	struct comp_params params = get_comp_params(client);
	pr_info("%s probe\n", client->name);
	ret = read_data(client, recv_buf, 0xFA, 3);
	if (ret < 0) {
		pr_err("Unable ro read temp: %d\n", ret);
	}
	uncomp_temp = recv_buf[0] << 12 | recv_buf[1] << 4 | ((recv_buf[2] >> 4) & 0x0F0);
	ret = compensate_temp(uncomp_temp, &params);
	pr_info("%02X %02X %02X\n", recv_buf[0], recv_buf[1], recv_buf[2]);
	pr_info("Uncomp temp: %d\n", uncomp_temp);
	pr_info("Temp: %d\n", ret);
	return 0;
}


static int i2c_bme_remove(struct i2c_client *client) {
	pr_info("%s remove", client->name);
	return 0;
}



static struct i2c_device_id i2c_bme_id[] = {
	{"bme280", 0x76},
	{ }
};

MODULE_DEVICE_TABLE(i2c, i2c_bme_id);

static struct i2c_driver i2c_bme_driver = { 
	.driver = {
		.name = "i2c-bme280",
	},
	.probe = i2c_bme_probe,
	.remove = i2c_bme_remove,
	.id_table = i2c_bme_id,
};

module_i2c_driver(i2c_bme_driver);

MODULE_AUTHOR("Some humans");
MODULE_LICENSE("GPL");

