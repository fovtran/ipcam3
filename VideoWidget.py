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

debug_videowidget = True

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

	def __init__(self, parent=None):
		super(OwnImageWidget, self).__init__(parent)
		#self.installEventFilter(self)
		self.audio_enabled = False
		self.setAcceptDrops(False)
		self.name = parent.objectName()

		self.image = None
		self.startpos = QtCore.QPoint(0,0)
		self.releasepos = QtCore.QPoint(0,0)
		self.pos = QtCore.QPoint(0,0)
		self.clipper = QtCore.QRect(self.startpos, self.releasepos)
		self.pen = QtGui.QColor(255,34,255,50)
		self.recording = False
		self.last_event = None
		self.editing = None
		self.retainimage = False
		self.clipzoom = False
		self.retainclip = False
		self.current_event = None
		self.retainimage = False

		self.SharedData = None
		self.WidgetData = WidgetData(self)
		#self.clicked.connect(self.buttonWasPressed)
		self.t = QtCore.QElapsedTimer()
		self.qp = QtGui.QPainter()

	def mouseDoubleClickEvent(self, evt):
		self.last_event = EVT_DOUBLECLICK

	def dropEvent(self, evt):
		if debug_videowidget:	print("Drop event")
		self.last_event = EVT_DROP

	def wheelEvent(self, evt):
		if debug_videowidget:	print("Wheel event")
		self.last_event = EVT_WHEEL

	def contextMenuEvent(self, evt):
		self.last_event = EVT_CTX
		self.retainimage = False
		if QtCore.QRect(self.startpos, self.releasepos).contains(evt.pos()):
			self.retainclip = True
			self.clipper = QtCore.QRect(self.startpos, self.releasepos)
		else:
			self.retainclip = False
			self.clipper = None

	def mousePressEvent(self, evt):
		if debug_videowidget:	print("Mouse press")
		self.recording = True
		if self.retainclip and self.last_event: #if clip.rect().contains(event.pos())::
			self.retainclip = False
		self.last_event = EVT_PRESS
		self.startpos = evt.pos()
		self.t.start()

	def mouseReleaseEvent(self, evt):
		if debug_videowidget:	print("Mouse Release")
		self.recording = False
		self.last_event = EVT_RELEASE
		self.releasepos = evt.pos()
		if self.retainimage:
			self.retainimage = False
		self.t.restart()
		if self.editing and self.startpos is not self.releasepos:
			self.clipzoom = True
			self.editing = False

	def mouseMoveEvent(self, evt):
		self.last_event = EVT_MOVE
		if not self.t.hasExpired(50):
			self.t.restart()
			self.editing = True
			self.pos = evt.pos()
		else:
			self.editing = False
		if self.editing:
			self.retainimage = True
		else:
			self.retainimage = False
			self.retainclip = False

	def setImage(self, image, recording, IsStream, counter):
		self.WidgetData.counter = counter
		self.WidgetData.IsStream = IsStream

		if self.recording:
			if self.SharedData.IsStream:
				if self.RecordingReady:
					self.VideoRecorder.write_current_frame(self.img)
			else:
				self.Snapshot()
				self.recording = False # remover para video recording

		if not self.last_event==EVT_PRESS:
			if self.clipzoom:
				if self.retainclip:
					self.WidgetData.image = image.copy(self.clipper)
					self.update()
					return
				else:
					self.WidgetData.image = image
					self.update()
					return
			else:
				pass
		self.image = image
		sz = self.image.size()
		self.setMinimumSize(sz)
		self.update()

	def rescale(self, x,y):
		self.setImage(QtGui.QImage( QtGui.QPixmap(self.image).scaled(x,y,QtCore.Qt.KeepAspectRatio)))

	def paintEvent(self, event):
		self.qp.begin(self)
		if self.image:
			self.qp.drawImage(QtCore.QPoint(0, 0), self.image)
			if self.clipzoom:
				self.qp.fillRect(QtCore.QRect(self.startpos, self.releasepos), self.pen)
		self.qp.end()

	def Snapshot(self):
		quality = 100
		self.WidgetData.filename = 'img-' + '' + str(self.WidgetData.counter) + '.jpg'
		self.WidgetData.full_image_path = os.path.join(self.WidgetData.save_dir, self.WidgetData.filename)
		save_response = self.image.save(self.WidgetData.full_image_path, "jpg", quality)
		print(save_response, self.WidgetData.full_image_path)

	def EnableVideoRecording(self):
		self.recording = True
		if self.audio_enabled:
			self.WidgetData.MyThreadPool.start(self.AudioTask)
		self.VideoRecorder.setup()
		self.RecordingReady = True
		if debug_videowidget:	print("Recording")

	def DisableVideoRecording(self):
		self.RecordingReady = False
		self.recording = False
		if self.audio_enabled:
			self.WidgetData.audio_finished.emit()
		self.WidgetData.recorder_finished.emit()

	def setup(self, SharedData):
		self.SharedData = SharedData
		self.VideoRecorder = VideoRecorder(self.SharedData)
		if self.audio_enabled:
			self.AudioTask = AudioRecorderThread(self.SharedData)
			self.WidgetData.audio_finished.connect(self.AudioTask.stop)
		self.WidgetData.recorder_finished.connect(self.VideoRecorder.Shutdown)