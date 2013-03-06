#!/usr/bin/env python
# (C) 2010 Project Blox <JesseTannahill@gmail.com>
# A transfer program from a host computer to the STM32F103VE.
#
# Copyright (C) 2010 by Project Blox
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import serial
import os
import sys
import struct

class transfer:
	"""A class that interacts with a Blox running the base program to exchange programs and other information."""
	ACK = 0x79
	NAK = 0x1F

	opcodes = {
		'RCV_APP' : 0x1,
		'DEL_APP' : 0x2,
		'LST_APPS': 0x3,
		'RUN_APP' : 0x4
	}
            
	def processCmd(self, args):
		opcode = args[0]
		if opcode not in self.opcodes:
			raise Exception ("Invalid opcode: " + str(opcode));
		#Send opcode + checksum
		print("Sending opcode ("+opcode+","+str(self.opcodes[opcode])+")...", end='')
		sys.stdout.flush()
		self.ser.write(bytes([self.opcodes[opcode], 0xFF-self.opcodes[opcode]]))
		#Wait for ACK or NACK
		ret = self.ser.read(1)
		if len(ret) == 0:
			raise Exception ("processCmd failed, opcode timed out")
		elif ret[0] == self.NAK:
			raise Exception ("processCmd failed, opcode "+opcode+" returned NAK")
		elif ret[0] != self.ACK:
			raise Exception ("processCmd failed, opcode "+opcode+" return malform ACK: "+ret.decode('utf-8'))
		print("ACKed")		

		if opcode == 'RCV_APP':
			self.sendRcvApp(args[1:])
		elif opcode == 'DEL_APP':
			self.sendDelApp(args[1:])
		elif opcode == 'LST_APPS':
			self.sendLstApps(args[1:])
		else:
			self.sendRunApp(args[1:])

	def sendRcvApp(self, args):
		# Convert to a BIN if its a HEX
		if args[0][-3:] == "hex":
			os.system("hex2bin "+args[0])
			args[0] = args[0][0:-3] + "bin"
		f = open(args[0], 'rb')
		size = os.path.getsize(args[0])
		# Strip the file type if there is one
		if args[0].rfind(".") != -1:
			args[0] = args[0][0:args[0].rfind(".")]
		# Send length of file name
		name = os.path.basename(args[0])
		if len(name) > 31:
			name = name[0:31]
		print("\tRcvApp sending name len ("+name+","+str(len(name))+")...", end='')
		self.ser.write(bytes([len(name)]))
		self.ser.write(bytes([0xFF-len(name)]))
		ret = self.ser.read(1)
		if len(ret) == 0:
			raise Exception ("processCmd failed, opcode timed out")
		elif ret[0] == self.NAK:
			raise Exception ("processCmd failed, name len returned NAK")
		elif ret[0] != self.ACK:
			raise Exception ("processCmd failed, name len return malform ACK: "+ret.decode('utf-8'))
		print("ACKed")
		# Send the file name
		print("\tRcvApp sending name("+name+")...", end='')
		checksum = bytearray([0])
		for char in name:
			checksum[0] = (checksum[0] + ord(char)) % 0x100
		self.ser.write(bytes(name, 'utf-8'))
		checksum[0] = 0xFF-checksum[0]
		self.ser.write(checksum)
		ret = self.ser.read(1)
		if len(ret) == 0:
			raise Exception ("processCmd failed, opcode timed out")
		elif ret[0] == self.NAK:
			raise Exception ("processCmd failed, name returned NAK")
		elif ret[0] != self.ACK:
			raise Exception ("processCmd failed, name return malform ACK: "+ret.decode('utf-8'))
		print("ACKed")

		# Send file size in pages
		checksum[0] = 0
		if not size > 0:
			self.ser.write(bytes([0]))
			return
		numPages = int(size/2048)
		if (size % 2048) > 0:
			numPages += 1
		sizeArray = struct.pack("L", size)
		checksum[0] = (checksum[0] + sizeArray[0]) % 0x100
		checksum[0] = (checksum[0] + sizeArray[1]) % 0x100
		checksum[0] = (checksum[0] + sizeArray[2]) % 0x100
		checksum[0] = (checksum[0] + sizeArray[3]) % 0x100
		print("\tRcvApp sending the file's size in pages("+str(size)+","+str(numPages)+")...", end='')
		sys.stdout.flush()
		self.ser.write(sizeArray)
		checksum[0] = 0xFF-checksum[0]
		self.ser.write(checksum)
		ret = self.ser.read(1)
		if len(ret) == 0:
			raise Exception ("processCmd failed, opcode timed out")
		elif ret[0] == self.NAK:
			raise Exception ("sendRcvApp failed, opcode RCV_APP returned NAK")
		elif ret[0] != self.ACK:
			raise Exception ("sendRcvApp failed, opcode RCV_APP return malform ACK: "+ret.decode('utf-8'))
		print("ACKed")

		# Send each page 1 at a time
		curPageNum = 0
		while True:
			checksum[0] = 0
			page = f.read(2048)
			if not page:
				break
			for b in page:
				checksum[0] = (checksum[0] + b) % 0x100

			print("\tRcvApp sending page " + str(curPageNum) + "...", end='')
			sys.stdout.flush();
			self.ser.write(page)
			checksum[0] = 0xFF - checksum[0]
			self.ser.write(checksum)
			ret = self.ser.read(1)
			if len(ret) == 0:
				raise Exception ("processCmd failed, opcode timed out")
			elif ret[0] == self.NAK:
				raise Exception ("sendRcvApp failed, opcode RCV_APP returned NAK")
			elif ret[0] != self.ACK:
				raise Exception ("sendRcvApp failed, opcode RCV_APP return malform ACK: "+ret.decode('utf-8'))
			print("ACKed")
			curPageNum += 1
		f.close()
		print("\tEnd RcvApp")

	def sendDelApp(self, args):
		# Send file id
		print("\tDelApp Sending file id("+args[0]+")...", end='')
		data = bytearray(2)
		data[0] = int(args[0])
		data[1] = 0xFF - data[0]
		self.ser.write(data)
		ret = self.ser.read(1)
		if len(ret) == 0:
			raise Exception ("sendDelApp failed, id ACK timed out")
		elif ret[0] == self.NAK:
			raise Exception ("sendDelApp failed, returned NAK")
		elif ret[0] != self.ACK:
			raise Exception ("sendDelApp failed, returned malform ACK: "+ret.decode('utf-8'))
		print("ACKed")
		return
	def sendLstApps(self, args):
		# Receive number of files (1 byte)
		print("\tLstApps Receiving number of files...", end='')
		ret = self.ser.read(1)
		numFiles = ret[0]
		ret = self.ser.read(1)
		if len(ret) == 0:
			self.ser.write([self.NAK])
			raise Exception ("sendLstApps failed, didn't receive number of file")		
		self.ser.write([self.ACK])
		print("got "+str(numFiles)+" files")
		# Receive files 1 at a time
		for i in range(numFiles):
			print("\tLstApps receving file "+str(i)+"...", end='')
			# Receive file id
			checksum = bytearray([0])
			ret = self.ser.read(1)
			if len(ret) == 0:
				self.ser.write([self.NAK])
				raise Exception ("sendLstApps failed, didn't receive file id")
			checksum[0] = (checksum[0] + ret[0]) % 0x100
			id = ret[0]
			# Receive file name
			ret = self.ser.read(32)
			for byte in ret:				
				checksum[0] = (checksum[0] + byte) % 0x100
			if len(ret) != 32:
				self.ser.write([self.NAK])
				raise Exception ("sendLstApps failed, didn't receive file name")
			name = ret.decode('utf-8').split('\0')[0]
			# Receive file size
			ret = self.ser.read(1)
			checksum[0] = (checksum[0] + ret[0]) % 0x100
			if len(ret) == 0:
				self.ser.write([self.NAK])
				raise Exception ("sendLstApps failed, didn't receive file numPages")
			numPages = ret[0]
			ret = self.ser.read(1)
			checksum[0] = (checksum[0] + ret[0]) % 0x100
			if checksum[0] != 0xFF:
				self.ser.write([self.NAK])
			self.ser.write([self.ACK])
			print("got id,name,numPages("+str(id)+","+name+","+str(numPages)+")")
		return
	def sendRunApp(self, args):
		# Send file id
		print("\tRunApp Sending file id("+args[0]+")...", end='')
		data = bytearray(2)
		data[0] = int(args[0])
		data[1] = 0xFF - data[0]
		self.ser.write(data)
		ret = self.ser.read(1)
		if len(ret) == 0:
			raise Exception ("sendRunApp failed, id ACK timed out")
		elif ret[0] == self.NAK:
			raise Exception ("sendRunApp failed, returned NAK")
		elif ret[0] != self.ACK:
			raise Exception ("sendRunApp failed, returned malform ACK: "+ret.decode('utf-8'))
		print("ACKed")
		return

	def __init__(self, ser):
		self.ser = ser;
			
if len(sys.argv) < 3:
	help()

ser = serial.Serial(sys.argv[1], 115200, parity=serial.PARITY_EVEN)
transfer = transfer(ser)
transfer.processCmd(sys.argv[2:])

def help():
	"""\
	Usage: transfer.py [port] [command] [data|filename]

	A transfer program for interacting with a base program loaded on a Blox.

	Examples:
	transfer.py COM4 RCV_APP myfile.hex"""

