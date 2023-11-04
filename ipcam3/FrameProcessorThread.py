# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import

__author__      = "Diego Cadogan"
__copyright__   = "Copyright 2017, sss.com"
__version__ = "0.3"
__license__ = "GPL"

import sys, os, platform, time
import numpy as np
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from Models import SharedData, WidgetData, ExporterSharedData
from OpenCVTools import OpenCVTools
import concurrent.futures
import scipy.misc

debug = True
ocv = OpenCVTools()

@np.vectorize
def decode_string(f):
	return np.fromstring(f, dtype=np.uint8)

def decode_image(frame):
	""" SLOW CV2 CONVERT TO SCIPY ARRAY"""
	myframe = decode_string(frame)
	RGB = ocv.imdecode(myframe)
	# RGB = scipy.misc.toimage(frame)
	return RGB

class FrameProcessor(QtCore.QRunnable):
	killthread = False
	def __init__(self, SharedData, parent=None):
		QtCore.QRunnable.__init__(self)
		super(self.__class__, self).__init__()
		self.SharedData = SharedData

		# VECT BLANK IMG CREATION
		self.blank_image = np.zeros((self.SharedData.height,self.SharedData.width,3), np.uint8)
		self.blank_image[:,0:1*self.SharedData.width] = (0,255,0)      # (B, G, R)
		self.blank_image[:,1*self.SharedData.width:self.SharedData.width] = (0,255,0)
		self.outputstream = None

	def future_decode_image(self):
		with concurrent.futures.ThreadPoolExecutor(max_workers=len(self.SharedData.framequeue)) as executor:
			future_to_image = {executor.submit(decode_image, f): f for f in self.SharedData.framequeue}
			for future in concurrent.futures.as_completed(future_to_image):
				self.outputstream = future_to_image[future]
				try:
					self.outputstream = future.result()
				except Exception as exc:
					print('image decoder generated an exception: %s' % (exc))
				else:
					self.SharedData.errors = False
					self.SharedData.current_frame.emit(self.outputstream)
			self.SharedData.framequeue = []

	@QtCore.pyqtSlot()
	def run(self):
		""" Core Loop: Try to vectorize comparator"""
		while not FrameProcessor.killthread:
			if self.SharedData.blank_image:
				self.SharedData.current_frame.emit(self.blank_image)
			elif len(self.SharedData.framequeue)>0:
				#self.outputstream = self.overlay.copy()
				self.future_decode_image()
				#self.current_frame.emit(self.blank_image)

	def cleanup(self):
		FrameProcessor.killthread = True
