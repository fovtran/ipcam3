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
from VideoWidget import OwnImageWidget
from Statistics import Statistics
from Models import SharedData, WidgetData, ExporterSharedData

form_class = uic.loadUiType("simple.ui")[0]

class MyWindowClass(QtWidgets.QMainWindow, form_class):
	def __init__(self, myaudiodev, parent=None):
		QtWidgets.QMainWindow.__init__(self, parent)
		self.setupUi(self)
		self.startButton.clicked.connect(self.recording_clicked)
		self.startButton.setEnabled(True)
		self.startButton.setText('Snapshoting')
		self.videoSource.clicked.connect(self.video_source)
		self.checkBox.stateChanged.connect(self.extra_filtering)
		self.sourceComboBox.activated[str].connect(self.video_select_source)

		self.mydev = myaudiodev
		self.SharedData = SharedData(parent)
		self.cvtools = OpenCVTools(parent)

		self.config = ConfigRead('config.ini', parent=parent)
		self.config.Reader()
		self.my_video_source = None
		self.updating = True
		self.filtering = False
		self.ImageObjects = [None]*11
		self.window_width = self.ImgWidget.frameSize().width()
		self.window_height = self.ImgWidget.frameSize().height()

		self.ImageObjects[0] = OwnImageWidget(self.ImgWidget)
		self.ImageObjects[0].setup(self.SharedData)
		self.show()

		self.ImageObjects[0].clicked.connect(self.imgWidgetClicked)

		self.thumb_ctx = self.ImageObjects[1]
		self.thumb_counter = 0
		store = self.update_other(self.thumb_ctx , self.cvtools.cvread(r'thumb.png'))

		self.Dispatcher()

	def Dispatcher(self):
		self.processor = FrameProcessor(self.SharedData)
		self.SharedData.MyThreadPool.start(self.processor)
		self.SharedData.current_frame.connect(self.update_frame)
		self.SharedData.frame_processor_finished.connect(self.processor.cleanup)
		print("Frame Dispatcher ready")

		self.dispatcher = ThreadDispatcher(self.SharedData)
		self.SharedData.dispatcher_finished.connect(self.dispatcher.cleanup)
		self.dispatcher.start()
		print("Network Reader ready")

		self.stats = Statistics(self.SharedData)
		self.stats.start(1000)

	def extra_filtering(self, s):
		if self.checkBox.isChecked():
			self.SharedData.filtering = True
		else:
			self.SharedData.filtering = False

	def thumbnail_counter(self):
		self.thumb_ctx = self.ImageObjects[self.thumb_counter]
		self.thumb_counter +=1
		if self.thumb_counter == len(self.ImageObjects):
			self.thumb_counter = 0

	def video_select_source(self, text):
		if text== "IPCam Video Source":
			self.my_video_source = "IPCam"
			self.videoSource.setText('IPCam')
		elif text == "Desktop Video Source":
			self.my_video_source = "Desktop"
			self.videoSource.setText('Desktop')
		elif text == "File Attachment Source":
			self.my_video_source = "Attachment"
			self.videoSource.setText('Attachment')
		elif text == "No Source" or self.my_video_source == None:
			self.my_video_source = "NoSource"
			self.videoSource.setText('NoSource')

	def video_source(self):
		if self.my_video_source == "Desktop":
			self.videoSource.setText('IPCam')
			self.my_video_source = "IPCam"
		elif self.my_video_source == "IPCam":
			self.videoSource.setText('Attachment')
			self.my_video_source = "Attachment"
		elif self.my_video_source == "Attachment":
			self.videoSource.setText('NoSource')
			self.my_video_source = "NoSource"
		elif self.my_video_source == "NoSource" or self.my_video_source == None:
			self.videoSource.setText('Desktop')
			self.my_video_source = "Desktop"

	def recording_clicked(self):
		time.sleep(1000)
		if self.SharedData.recording:
			self.SharedData.IsStream = False
			self.startButton.setText('Snapshoting')
			self.SharedData.recording = False
			self.SharedData.IsStream = False
			self.ImageObjects[0].DisableVideoRecording()
		else:
			self.ImageObjects[0].EnableVideoRecording()
			self.SharedData.IsStream = True
			self.SharedData.recording = True
			self.startButton.setText('Streaming...')

	@QtCore.pyqtSlot()
	def imgWidgetClicked(self):
		self.thumbnail_counter()
		#print("Event: ", evt)
		self.SharedData.timer1.start()
		self.source = 'ipcam'
		store = self.update_other(self.thumb_ctx, self.SharedData.videoframe)

	def update_other(self, ctx, im):
		if type(im) == np.ndarray:
			img_height, img_width, img_colors = im.shape
		elif type(im) == QtGui.QImage:
			img_width = im.width()
			img_height = im.height()
			img_colors = im.depth()
			ctx.rescale(im,64,64)

	@QtCore.pyqtSlot(object)
	def update_frame(self, current_frame):
		self.config.image_counter +=1
		#self.config.last_files = str([self.config.last_files] + [full_image_path])
		self.SharedData.videoframe = current_frame
		img_height, img_width, img_colors = self.SharedData.videoframe.shape
		scale_w = np.float(self.window_width) / np.float(img_width)
		scale_h = np.float(self.window_height) / np.float(img_height)
		scale = np.min([scale_w, scale_h])
		self.SharedData.img = self.cvtools.cvresize(self.SharedData.videoframe, scale)
		self.ImageObjects[0].setStream(self.SharedData.videoframe)
		self.SharedData.height, self.SharedData.width, bpc = self.SharedData.img.shape
		self.SharedData.qimage = QtGui.QImage(self.SharedData.img.data, self.SharedData.width, self.SharedData.height, bpc * self.SharedData.width, QtGui.QImage.Format_RGB888)
		self.ImageObjects[0].setImage(self.SharedData.qimage, self.SharedData.recording, self.SharedData.IsStream, self.config.image_counter)

	def closeEvent(self, event):
		self.config.Writer()
		print("Stopping all")
		self.processor.cleanup()
		self.dispatcher.cleanup()
		self.updating = None
		self.img = None
		print("Data Thread shutdown succesful")
