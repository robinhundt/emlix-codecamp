#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>

static int temp;

static ssize_t show_temp(struct device *dev,struct device_attribute *attr, char *buf)
{
	struct i2c_client *i = to_i2c_client(dev);
	// get temperature from sensor
	//
	return sprintf(buf, "%d\n", temp++);
}

static struct device_attribute dev_attr_temp = {
	.attr = {
		.name = "temperature",
		.mode = S_IRUGO,
	},
	.show = show_temp,
};

// aufgerufen wenn zugehöriges device gefunden wird
static int i2c_bme_probe(struct i2c_client *client, const struct i2c_device_id *id) {
	int ret;
	ret = device_create_file(&client->dev, &dev_attr_temp);
	
	printk("Hello World! %d\n", ret);
	return ret;
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

