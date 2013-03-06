#!/usr/bin/env python
# (C) 2010 Project Blox <JesseTannahill@gmail.com>
# A serial programmer for STM32F103xx. Clears flash and writes a single file
# Configuration: BOOT0 and BOOT1 should be configured so the system bootloader runs
# BOOT1 | BOOT0 |   Boot Mode
#   X   |   0   |  User Flash
#   0   |   1   | System Memory <-- Set jumpers to this
#   1   |   1   | Embedded SRAM
# BOOT0/1 must be set when the system is off (or before a hard reset).
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
import sys

class StmSerialProgrammer:
	"""A wrapper around the programming functions in the STM32F103 system bootloader."""
	ACK = 0x79
	NACK = 0x1F

	cmdToByte = { 'get': 0x00,
		      'getVersion': 0x01,
		      'getID': 0x02,
		      'readMem': 0x11,
		      'go': 0x21,
		      'writeMem': 0x31,
		      'erase': 0x43,
		      'extErase': 0x44,
		      'writeProtect': 0x63,
		      'writeUnProtect': 0x73,
		      'readProtect': 0x82,
		      'readUnProtect': 0x92 }

	device_params = { 0x412: { 'device': 'low-density',
				   'ram_low':  0x20000200,
				   'ram_high': 0x20002800,
				   'flash_low':  0x08000000,
				   'flash_high': 0x08008000,
				   'flash_sector_kb': 4,
				   'flash_sector_num_pages': 4,
				   'option_byte_low':  0x1FFFF800,
				   'option_byte_high': 0x1FFFF80F,
				   'system_low':  0x1FFFF000,
				   'system_high': 0x1FFFF800 },
			  0x410: { 'device': 'medium-density',
				   'ram_low':  0x20000200,
				   'ram_high': 0x20005000,
				   'flash_low':  0x08000000,
				   'flash_high': 0x08020000,
				   'flash_sector_kb': 4,
				   'flash_sector_num_pages': 4,
				   'option_byte_low':  0x1FFFF800,
				   'option_byte_high': 0x1FFFF80F,
				   'system_low':  0x1FFFF000,
				   'system_high': 0x1FFFF800 },
			  0x414: { 'device': 'high-density',
				   'ram_low':  0x20000200,
				   'ram_high': 0x20010000,
				   'flash_low':  0x08000000,
				   'flash_high': 0x08080000,
				   'flash_sector_kb': 4,
				   'flash_sector_num_pages': 2,
				   'option_byte_low': 0x1FFFF800,
				   'option_byte_high': 0x1FFFF80F,
				   'system_low': 0x1FFFF000,
				   'system_high': 0x1FFFF800 },
			  0x418: { 'device': 'connectivity line',
				   'ram_low':  0x20001000,
				   'ram_high': 0x20010000,
				   'flash_low':  0x08000000,
				   'flash_high': 0x08020000,
				   'flash_sector_kb': 4,
				   'flash_sector_num_pages': 2,
				   'option_byte_low':  0x1FFFF800,
				   'option_byte_high': 0x1FFFF80F,
				   'system_low':  0x1FFFB000,
				   'system_high': 0x1FFFF800 },
			  0x420: { 'device': 'medium-density value line',
				   'ram_low':  0x20000200,
				   'ram_high': 0x20002000,
				   'flash_low':  0x08000000,
				   'flash_high': 0x08020000,
				   'flash_sector_kb': 4,
				   'flash_sector_num_pages': 4,
				   'option_byte_low':  0x1FFFF800,
				   'option_byte_high': 0x1FFFF80F,
				   'system_low':  0x1FFFF000,
				   'system_high': 0x1FFFF800 },
			  0x430: { 'device': 'XL-density',
				   'ram_low':  0x20000800,
				   'ram_high': 0x20018000,
				   'flash_low':  0x08000000,
				   'flash_high': 0x08100000,
				   'flash_sector_kb': 4,
				   'flash_sector_num_pages': 2,
				   'option_byte_low': 0x1FFFF800,
				   'option_byte_high': 0x1FFFF80F,
				   'system_low': 0x1FFFE000,
				   'system_high': 0x1FFFF800 }}

	def __init__(self, serial, file):
		self.serial = serial;
		self.file = file;
		self.enabledCmds = { 0x00: True, 
				     0x01: False,
				     0x02: False,
				     0x11: False,
				     0x21: False,
				     0x31: False,
				     0x43: False,
				     0x44: False,
				     0x63: False,
				     0x73: False,
				     0x82: False,
				     0x92: False }
	def init(self):
		self.serial.read(self.serial.inWaiting()) #clear read buffer
		self.serial.write(str(0x7F))
		if ord(self.serial.read(1)) != ACK:
			raise Exception("On initialization, no response byte.");		
	def get(self):
	"""	Byte 1: ACK
		Byte 2: N = 11 = the number of bytes to follow â€“ 1 except current and ACKs.
		Byte 3: Bootloader version (0 < Version < 255), example: 0x10 = Version 1.0
		Byte 4: 0x00 “Get command
		Byte 5: 0x01 “Get Version and Read Protection Status
		Byte 6: 0x02 “Get ID
		Byte 7: 0x11 “Read Memory command
		Byte 8: 0x21 “Go command
		Byte 9: 0x31 “Write Memory command
		Byte 10: 0x43 or 0x44 “Erase command or Extended Erase command 
			(these commands are exclusive)
		Byte 11: 0x63 “Write Protect command
		Byte 12: 0x73 “Write Unprotect command
		Byte 13: 0x82 “Readout Protect command
		Byte 14: 0x92 “Readout Unprotect command
		Last byte (15): ACK"""
		if !enabledCmds[cmdToByte['get']]:
			raise Exception("Attempted to access unsupported command: get")

		self.serial.read(self.serial.inWaiting())
		if self.sendCmd(bytearray(b'\x00\xFF')):
			numBytes = self.serial.read(1)+1 #device sends numBytes remaining-1 (ignoring ACK)
			data = bytearray(self.serial.read(numBytes))
			self.bootloader_version = data[0]
			for byte in data[1:]:
				self.enabledCmds[byte] = True
			waitForACK('get')
			return self.enabledCmds
		else:
			raise Exception("Failed to execute get command!")



	def getVersionAndReadProt(self):
	"""	Byte 1: ACK
		Byte 2: Bootloader version (0 < Version <= 255), example: 0x10 = Version 1.0
		Byte 3: Option byte 1: 0x00 to keep the compatibility with generic bootloader protocol
		Byte 4: Option byte 2: 0x00 to keep the compatibility with generic bootloader protocol
		Byte 5: ACK"""
		if !enabledCmds[cmdToByte['getVersion']]:
			raise Exception("Attempted to access unsupported command: getVersion")
		self.serial.read(self.serial.inWaiting())

		if self.sendCmd(bytearray(b'\x01\xFE')):
			data = self.serial.read(3)			
			self.bootloader_version = data[0]			
			waitForACK('getVersionAndReadProt')
			return data
		else:
			raise Exception("Failed to execute getVersionAndReadProt command!")

	def getID(self):
	"""	Byte 1: ACK
		Byte 2: N = the number of bytes “1 (N = 1 for STM32), except for current byte and ACKs.
		Bytes 3-4: PID(1) byte 3 = 0x04, byte 4 = 0x1X		
		Byte 5: ACK"""
		if !enabledCmds[cmdToByte['getID']]:
			raise Exception("Attempted to access unsupported command: getID")
		self.serial.read(self.serial.inWaiting())
		if self.sendCmd(bytearray(b'\x02\xFD')):
			numBytes = self.serial.read(1)+1 #device sends numBytes remaining-1 (ignoring ACK)
			data = bytearray(self.serial.read(numBytes))
			self.product_id = data[0] << 8 + data[1]
			if !key in device_params:
				raise Exception("Unsupported product ID!")
			self.waitForACK('getID')
			return self.product_id
		else
			raise Exception("Failed to execute getID command!")

	def readMem(self, loc, numBytes):
		if !enabledCmds[cmdToByte['readMem']]:
			raise Exception("Attempted to access unsupported command: readMem")
		if self.product_id == None:				
			raise Exception("Attempted to call readMem before getID")
		if loc < device_params[self.product_id]['flash_low'] or
		   loc > device_params[self.product_id]['flash_high']: 
			   raise Exception("Attempted to call readMem on an invalid location: " + str(loc))
		if numBytes < 0 or numBytes > 255:
			   raise Exception("Attempted to call readMem with an illegal amount of bytes: " + str(numBytes))
		self.serial.read(self.serial.inWaiting())
		if self.sendCmd(bytearray(b'\x11\xEE')):
		
		else:		
			raise Exception("Failed to execute readMem command!")


	def go(self):
		if !enabledCmds[cmdToByte['go']]:
			raise Exception("Attempted to access unsupported command: go")
		self.serial.read(self.serial.inWaiting())
	def writeMem(self):
		if !enabledCmds[cmdToByte['writeMem']]:
			raise Exception("Attempted to access unsupported command: writeMem")
		self.serial.read(self.serial.inWaiting())
	def erase(self):
		if !enabledCmds[cmdToByte['erase']]:
			raise Exception("Attempted to access unsupported command: erase")
		self.serial.read(self.serial.inWaiting())
	def extErase(self):
		if !enabledCmds[cmdToByte['extErase']]:
			raise Exception("Attempted to access unsupported command: extErase")
		self.serial.read(self.serial.inWaiting())
	def writeProtect(self):
		if !enabledCmds[cmdToByte['writeProtect']]:
			raise Exception("Attempted to access unsupported command: writeProtect")
		self.serial.read(self.serial.inWaiting())
	def writeUnProtect(self):
		if !enabledCmds[cmdToByte['writeUnProtect']]:
			raise Exception("Attempted to access unsupported command: writeUnProtect")
		self.serial.read(self.serial.inWaiting())
	def readProtect(self):
		if !enabledCmds[cmdToByte['readProtect']]:
			raise Exception("Attempted to access unsupported command: readProtect")
		self.serial.read(self.serial.inWaiting())
	def readUnProtect(self):
		if !enabledCmds[cmdToByte['readUnProtect']]:
			raise Exception("Attempted to access unsupported command: readUnProtect")
		self.serial.read(self.serial.inWaiting())

	def sendCmd(self, cmd):		
		self.serial.write(cmd)
		response = self.serial.read(1)
		if response == ACK:
			return True
		else if response == NACK:
			return False
		else:
			raise Exception("Invalid response to cmd" + cmd)

	def waitForACK(self, cmdName):
		if self.serial.read(1) != ACK:
			raise Exception("Did not receive ACK for command: " + cmdName)

if len(sys.argv) < 3:
	help()

serial = new serial.Serial(sys.argv[1], 115200, parity=serial.PARITY_EVEN, timeout=1)
file = open(sys.argv[2], 'r');

stm_serial = StmSerialProgrammer(serial, file);



def help():
"""\
Usage: serial_programmer.py [port] [file]

A serial programmer for STM32F103xx.

Examples:
serial_programmer.py /dev/USBtty0 foo"""

