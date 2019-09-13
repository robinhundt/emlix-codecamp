#!/usr/bin/env python
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import SocketServer
import json

class S(BaseHTTPRequestHandler):
    def _read_data(self):
        f = open("test.txt", "r")
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
        self.wfile.write(json.dumps(self._read_data()))

    def do_HEAD(self):
        self._set_headers()

def run(server_class=HTTPServer, handler_class=S, port=80):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print 'Starting server...'
    httpd.serve_forever()

if __name__ == "__main__":
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
