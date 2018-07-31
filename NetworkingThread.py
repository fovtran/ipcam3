# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import
__author__      = "Diego Cadogan"
__copyright__   = "Copyright 2017, sss.com"
__version__ = "0.3"
__license__ = "GPL"

import sys, os, time, requests
import numpy as np
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from Models import SharedData, WidgetData, ExporterSharedData

debug = False

class ThreadDispatcher(QtCore.QThread):
	killthread = False
	finished = QtCore.pyqtSignal()

	def __init__(self, SharedData, parent=None):
		QtCore.QThread.__init__(self)
		super(self.__class__, self).__init__()
		self.SharedData = SharedData
		self.SharedData.blank_image = False
		self.finished.connect(self.cleanup)
		self.bytes = b''


	def run(self):
		s = requests.Session()
		try:
			print("Streaming client start")
			self.stream = s.get(self.SharedData.stream_URI, stream=True, timeout=10.001)
			print("Stream Connected")
		except:
			print("Stopping stream")
			self.errors()

		while not ThreadDispatcher.killthread:
			self.SharedData.loopcounter[0] +=1
			#self.SharedData.timestart = time.time()
			try:
				if debug:
					print("Reading %i self.bytes" % self.SharedData.blksize)
				self.bytes += self.stream.raw.read(self.SharedData.blksize)
				a,b = (self.bytes.find(b'\xff\xd8'), self.bytes.find(b'\xff\xd9'))
				if a!=-1 and b!=-1:
					self.SharedData.loopcounter[0] = 0
					#self.SharedData.framequeue.put(self.bytes[a:b+2])
					self.SharedData.framequeue.extend([self.bytes[a:b+2]])
					self.bytes = self.bytes[b+2:]
			except:
				self.errors()
				self.finished.emit("DONE")

			#time.sleep(self.SharedData.loopdelay[0])

	#@QtCore.pyqtSlot(str)
	def errors(self):
		print("Errors decoding image")
		self.SharedData.blank_image = True

	#@QtCore.pyqtSlot()
	def cleanup(self):
		if self.SharedData.audio_enabled:
			self.audiotask.cleanup()
		ThreadDispatcher.killthread = True
		print("Exitting thread")