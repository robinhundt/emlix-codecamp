#!/usr/bin/env bash

cd $(dirname "$0")

insmod driver/i2c-bme280.ko
echo "bme280 0x76" > /sys/bus/i2c/devices/i2c-1/new_device

ambient-light/ambient-light.py &
server/server.py
