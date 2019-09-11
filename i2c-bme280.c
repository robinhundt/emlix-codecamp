#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>


static int i2c_bme_probe(struct i2c_client *client, const struct i2c_device_id *id) {
	pr_info("%s probe", client->name);
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

