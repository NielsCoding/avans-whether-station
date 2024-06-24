#!/bin/python3

"""
this is a garbage python script that opens a tcp socket on localhost:33 for
connecting with the qt client
"""

import socketserver
from random import randint

def hexpad(n, pad):
  return hex(n)[2:].zfill(pad)

def bs(str):
  return bytes(str, 'utf-8')

def r(max):
  return randint(0, max)

class DummyServer(socketserver.BaseRequestHandler):
  def error(self):
    self.request.sendall(bs("error\n"))

  def ok(self, rows):
    response = "id,temperature,humidity,atmospheric_pressure\n"
    line_len = len("xxxx,xx,xx,xx\n")
    retstr = f"ok,{hex(len(response) + rows * line_len)[2:]}\n"
    retstr += response
    for row in range(rows):
      retstr += f"{hexpad(row, 4)},{hexpad(r(0xff), 2)},{hexpad(r(0xff), 2)},{hexpad(r(0xff), 2)}\n"
    self.request.sendall(bs(retstr))

  def handle(self):
    self.data = self.request.recv(1024).strip()
    if len(self.data) > 40: return self.error()
    if not self.data.startswith(bs('last-records')): return self.error()
    self.data = self.data.replace(bs('last-records'), bs(''))
    try:
      self.ok(int(self.data))
    except ValueError:
      self.error()

if __name__ == "__main__":
  socketserver.TCPServer.allow_reuse_address = True
  with socketserver.TCPServer(("localhost", 33), DummyServer) as server:
    server.serve_forever()


