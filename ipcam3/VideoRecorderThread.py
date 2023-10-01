# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import
__author__      = "Diego Cadogan"
__copyright__   = "Copyright 2017, sss.com"
__version__ = "0.3"
__license__ = "GPL"

import sys, os, time
import numpy as np
import cv2
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from subprocess import Popen, call, PIPE, STDOUT
from Models import SharedData, WidgetData, ExporterSharedData

debug = False

class VideoRecorder(QtCore.QObject):
	def __init__(self, SharedData, parent=None):
		QtCore.QObject.__init__(self)
		super(self.__class__, self).__init__()
		self.SharedData = SharedData
		#self.m = self.SharedData.mutex
		#self.cond = self.SharedData.condition
		self.fourcc = cv2.VideoWriter_fourcc(*'MP43')  #MJPG o MP43
		self.ExporterSharedData = ExporterSharedData()
		self.export = False

	def setup(self):
		self.export = True
		self.video_writer = cv2.VideoWriter(self.ExporterSharedData.filename, self.fourcc, self.ExporterSharedData.fps, (self.SharedData.width, self.SharedData.height), True)

	def write_current_frame(self, frame):
		self.video_writer.write(frame)

	def Shutdown(self):
		print("Writer shutdown")
		if self.export:
			ret = ExportGif(self.ExporterSharedData)
		self.video_writer.release()
