# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import

__author__      = "Diego Cadogan"
__copyright__   = "Copyright 2017, sss.com"
__version__ = "0.3"
__license__ = "GPL"

import sys, os, platform, time
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from Models import SharedData, WidgetData, ExporterSharedData

debug = False

class Statistics(QtCore.QTimer):
	def __init__(self, SharedData):
		QtCore.QTimer.__init__(self)
		super(self.__class__, self).__init__()
		self.timeout.connect(self.PrintStats)
		self.SharedData = SharedData

	def PrintStats(self):
		pass
		#print("Network Read loops: ", self.SharedData.loopcounter[0])
		#print( "SharedData size: ", sys.getsizeof(self.SharedData) )
		# fps
		# buffers
		# timeStart timeEnd
		# loopdelay