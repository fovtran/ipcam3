# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import

__author__      = "Diego Cadogan"
__copyright__   = "Copyright 2017, sss.com"
__version__ = "0.3"
__license__ = "GPL"

import time
import cv2
from PyQt5 import QtCore

class OpenCVTools(QtCore.QObject):
	current_frame = QtCore.pyqtSignal(object)

	def __init__(self, parent=None):
		QtCore.QObject.__init__(self)
		super(self.__class__, self).__init__()

	def imdecode(self,b):
		return cv2.flip(cv2.imdecode( b ,cv2.IMREAD_COLOR), 180)

	def cvread(self, f):
		return cv2.imread(f)

	def cvresize(self, v, scale):
		if scale == 0:	scale = 1
		if scale is not 1:
			img = cv2.resize(v, None, fx=scale, fy=scale, interpolation = cv2.INTER_CUBIC)
			return cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
		else:
			return cv2.cvtColor(v, cv2.COLOR_BGR2RGB)
