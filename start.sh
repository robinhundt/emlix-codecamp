#!/usr/bin/env bash

cd $(dirname "$0")

insmod driver/i2c-bme280.ko
echo "bme280 0x76" > /sys/bus/i2c/devices/i2c-1/new_device

server/server.py &
ambient-light/ambient-light.py -c

trap "kill 0" EXIT
