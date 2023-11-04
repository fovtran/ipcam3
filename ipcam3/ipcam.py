# -*- coding: utf-8 -*-
# from __future__ import print_function, division, absolute_import
__author__      = "Diego Cadogan"
__copyright__   = "Copyright 2017, sss.com"
__version__ = "0.3"
__license__ = "GPL"

import sys, os, platform, time, requests
from random import seed
seed(12345)

import cv2
#print(os.environ.pop("QT_QPA_PLATFORM_PLUGIN_PATH"))

from PyQt5 import QtCore, QtGui, QtWidgets, uic
from multiprocessing import freeze_support, Queue, Process, Pool, cpu_count
from configuration import config, ConfigRead
from ipcam_tool import MyWindowClass

plat = platform.system()
if plat=='Linux':
    myaudiodev = (0,0)
if plat=='Windows':
    import psutil
    myaudiodev = (8,12)
    p = psutil.Process(os.getpid())
    p.nice(psutil.REALTIME_PRIORITY_CLASS)

if __name__ == "__main__":
	# freeze_support()
	app = QtWidgets.QApplication(sys.argv)
	ex = MyWindowClass(myaudiodev)
	ex.setWindowTitle("IPCam Mastering Tool")
	sys.exit(app.exec_())
