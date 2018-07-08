# -*- coding: utf-8 -*-
__author__      = "Diego Cadogan"
__copyright__   = "Copyright 2017, sss.com"
__version__ = "0.3"
__license__ = "GPL"

import sys, os, platform, time, requests
from queue import Queue
from multiprocessing import freeze_support, Queue, Process, Pool, cpu_count
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
from Models import SharedData, WidgetData, ExporterSharedData

debug = True
audio = False

LEFTCLICK = 1
RIGHTCLICK = 2
CONTEXTMENU = 3
RELEASECLICK = 10
EVT_DOUBLECLICK = 0
EVT_PRESS = 1
EVT_RELEASE = 2
EVT_CTX = 3
EVT_WHEEL = 4
EVT_MOVE = 5
EVT_DRAG = 6
EVT_DROP = 7

class OwnImageWidget(QtWidgets.QWidget):
	clicked = QtCore.pyqtSignal(int)
	last_event = None

	def __init__(self, parent=None):
		super(OwnImageWidget, self).__init__(parent)
		#self.installEventFilter(self)
		self.setAcceptDrops(False)
		self.name = parent.objectName()
		self.image = None
		self.SharedData = None
		self.WidgetData = WidgetData(self)
		#self.clicked.connect(self.buttonWasPressed)
		self.t = QtCore.QElapsedTimer()
		self.qp = QtGui.QPainter()

	def setup(self, SharedData):
		self.SharedData = SharedData
		self.VideoRecorder = VideoRecorder(self.SharedData)
		if audio:
			self.AudioTask = AudioTaskThread(self.SharedData)
			self.WidgetData.audio_finished.connect(self.AudioTask.stop)
		self.WidgetData.recorder_finished.connect(self.VideoRecorder.Shutdown)

	def mouseDoubleClickEvent(self, evt):
		self.WidgetData.last_event = EVT_DOUBLECLICK

	def dropEvent(self, evt):
		if debug:	print("Drop event")
		self.WidgetData.last_event = EVT_DROP

	def wheelEvent(self, evt):
		if debug:	print("Wheel event")
		self.WidgetData.last_event = EVT_WHEEL

	def contextMenuEvent(self, evt):
		self.WidgetData.last_event = EVT_CTX
		self.WidgetData.retainimage = False
		if QtCore.QRect(self.WidgetData.startpos, self.WidgetData.releasepos).contains(evt.pos()):
			self.WidgetData.retainclip = True
			self.WidgetData.clipper = QtCore.QRect(self.WidgetData.startpos, self.WidgetData.releasepos)
		else:
			self.WidgetData.retainclip = False
			self.WidgetData.clipper = None

	def mousePressEvent(self, evt):
		if debug:	print("Mouse press")
		self.WidgetData.recording = True
		if self.WidgetData.retainclip and self.WidgetData.last_event: #if clip.rect().contains(event.pos())::
			self.WidgetData.retainclip = False
		self.WidgetData.last_event = EVT_PRESS
		self.WidgetData.startpos = evt.pos()
		self.t.start()

	def mouseReleaseEvent(self, evt):
		if debug:	print("Mouse Release")
		self.WidgetData.recording = False
		self.WidgetData.last_event = EVT_RELEASE
		self.WidgetData.releasepos = evt.pos()
		if self.WidgetData.retainimage:
			self.WidgetData.retainimage = False
		self.t.restart()
		if self.WidgetData.editing and self.WidgetData.startpos is not self.WidgetData.releasepos:
			self.WidgetData.clipzoom = True
			self.WidgetData.editing = False

	def mouseMoveEvent(self, evt):
		self.WidgetData.last_event = EVT_MOVE
		if not self.t.hasExpired(50):
			self.t.restart()
			self.WidgetData.editing = True
			self.WidgetData.pos = evt.pos()
		else:
			self.WidgetData.editing = False
		if self.WidgetData.editing:
			self.WidgetData.retainimage = True
		else:
			self.WidgetData.retainimage = False
			self.WidgetData.retainclip = False

	def setImage(self, image, recording, IsStream, counter):
		self.WidgetData.counter = counter
		self.WidgetData.recording = recording
		self.WidgetData.IsStream = IsStream
		if not self.WidgetData.last_event==EVT_PRESS:
			if self.WidgetData.clipzoom:
				if self.WidgetData.retainclip:
					self.WidgetData.image = image.copy(self.WidgetData.clipper)
					self.update()
					return
				else:
					self.WidgetData.image = image
					self.update()
					return
			else:
				pass
		self.image = image
		sz = image.size()
		self.setMinimumSize(sz)
		self.update()

	def setStream(self, img):
		if self.WidgetData.recording:
			if self.SharedData.IsStream:
				if self.RecordingReady:
					self.VideoRecorder.write_current_frame(img)
			else:
				self.Snapshot()
				self.WidgetData.recording = False # remover para video recording

	def rescale(self, image, x,y):
		self.setImage(QtGui.QImage( QtGui.QPixmap(image).scaled(x,y,QtCore.Qt.KeepAspectRatio)))

	def paintEvent(self, event):
		self.qp.begin(self)
		if self.image:
			self.qp.drawImage(QtCore.QPoint(0, 0), self.image)
			if self.WidgetData.clipzoom:
				self.qp.fillRect(QtCore.QRect(self.WidgetData.startpos, self.WidgetData.releasepos), self.WidgetData.pen)
		self.qp.end()

	def EnableVideoRecording(self):
		self.WidgetData.recording = True
		if audio:
			self.WidgetData.MyThreadPool.start(self.AudioTask)
		self.VideoRecorder.setup()
		self.RecordingReady = True
		if debug:	print("Recording")

	def DisableVideoRecording(self):
		self.RecordingReady = False
		self.WidgetData.recording = False
		if audio:
			self.WidgetData.audio_finished.emit()
		self.WidgetData.recorder_finished.emit()

	def Snapshot(self):
		quality = 100
		self.WidgetData.filename = 'img-' + '' + str(self.WidgetData.counter) + '.jpg'
		self.WidgetData.full_image_path = os.path.join(self.WidgetData.save_dir, self.WidgetData.filename)
		print(self.WidgetData.full_image_path)
		save_response = self.image.save(self.WidgetData.full_image_path, "jpg", quality)
