#!/usr/bin/env python3
from http.server import BaseHTTPRequestHandler, HTTPServer
import json
from os import environ

SYSFS_TEMPERATURE_FILE = "/sys/devices/platform/soc/20804000.i2c/i2c-1/1-0076/temperature"

class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    def _read_data(self):
        f = open(SYSFS_TEMPERATURE_FILE, "r")
        temp = int(f.readline().splitlines()[0])/100

        result = {
            "temperature": f.readline().splitlines()[0]
        }
        return result

    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def do_GET(self):
        self._set_headers()
        self.wfile.write(json.dumps(self._read_data()).encode())

    def do_HEAD(self):
        self._set_headers()

def run():
    print('starting server...')

    server_address = ('127.0.0.1', 80)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    print('running server...')
    httpd.serve_forever()

if __name__ == "__main__":
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
