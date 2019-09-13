# How to start

In Raspi, start I2C modules 

```
insmod /lib/modules/4.19.67+/kernel/drivers/i2c/i2c-dev.ko

echo "bme280 0x76" > /sys/bus/i2c/devices/i2c-1/new_device

insmod /home/i2c-bme280.ko
```

Start webserver

```
python /home/server.py
```

Read from server

```

```

Start ambient light 

```

```
