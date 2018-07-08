# -*- coding: utf-8 -*-
# from __future__ import print_function, division, absolute_import
__author__      = "Diego Cadogan"
__copyright__   = "Copyright 2017, sss.com"
__version__ = "0.3"
__license__ = "GPL"

import sys, os, platform, time, requests
from queue import Queue
from random import seed
seed(12345)
from multiprocessing import freeze_support, Queue, Process, Pool, cpu_count

#from PIL import ImageGrab, Image, ImageTk
#import cv2
import numpy as np
from PyQt5 import QtCore, QtGui, QtWidgets, uic

from configuration import config, ConfigRead
from NetworkingThread import ThreadDispatcher
from FrameProcessorThread import FrameProcessor
from AudioRecordThread import AudioRecorderThread
from OpenCVTools import OpenCVTools
from VideoRecorderThread import VideoRecorder
from GifEmitterThread import GifExporter, GifEmitterThread
from Statistics import Statistics

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
	freeze_support()
	app = QtWidgets.QApplication(sys.argv)
	ex = MyWindowClass(myaudiodev)
	ex.setWindowTitle("IPCam Mastering Tool")
	sys.exit(app.exec_())
