# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import
__author__      = "Diego Cadogan"
__copyright__   = "Copyright 2017, sss.com"
__version__ = "0.3"
__license__ = "GPL"

import sys, os, time
import numpy as np
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from subprocess import Popen, call, PIPE, STDOUT
from Models import SharedData, WidgetData, ExporterSharedData

debug = False

class GifEmitterThread(QtCore.QThread):
	finished = QtCore.pyqtSignal(str)

	def __init__(self, ExporterSharedData, curdir, fps, ss, t, filters, input_avi, output_gif, parent=None):
		QtCore.QThread.__init__(self)
		#super(self.__class__, self).__init__(parent)
		self.p = parent
		compiler = "ffmpeg.exe"
		compiler_dir = "ffmpeg-win64-static\\bin"
		self.ExporterSharedData = ExporterSharedData
		self.curdir = os.path.join(curdir, compiler_dir)
		self.our_compiler = os.path.join(self.curdir, compiler)
		self.start_time = str(ss)
		self.duration = t
		self.palette="./palette.png"
		self.fps = fps
		self.filters = 'fps='+str(self.fps)+',scale=320:-1:flags=lanczos'
		self.input_avi = input_avi
		self.output_gif = output_gif
		self.cmd_palette = ["-v","warning","-ss",self.start_time, "-t",self.duration, "-r", "10", "-i",self.input_avi, "-vf",self.filters+',palettegen',"-y",self.palette]
		self.cmd_gif = ["-v","warning", "-ss",self.start_time, "-t", self.duration,"-i", self.input_avi,"-i" , self.palette,"-lavfi",self.filters+"[x]; [x][1:v] paletteuse","-y",self.output_gif]

		self.finished.connect( self.EmitGifDone)
		if debug:
			print("Init", self.our_compiler)

	def call_command(self, cmd):
		if debug:
			print(cmd)
		p = Popen(cmd, universal_newlines=True, shell=True, stdout=PIPE, stderr=STDOUT)
		text = p.stdout.read()
		p.stdout.close()
		retcode = p.wait()
		if debug:
			print(text)
		return retcode

	@QtCore.pyqtSlot()
	def EmitGifDone(self, res):
		if debug:
			print(res)

	def run(self):
		''' GIF video emitter'''
		if os.path.exists(self.input_avi):
			if os.path.exists(self.our_compiler):
				if debug:
					print("Palette operation")
				self.call_command([self.our_compiler] + self.cmd_palette)
				if debug:
					print("Video builder")
				self.call_command([self.our_compiler] + self.cmd_gif)
				self.finished.emit( "DONE")
			else:
				if debug:
					print ("No video compiler found")
				self.emit( SIGNAL("nocompiler"), "ERR")
		else:
			if debug:
				print ("No input file found")
			self.emit( SIGNAL("noinput"), "ERR")

	def __del__(self):
		if debug:
			print("Stop processing gif")

class GifExporter(QtCore.QObject):
	def __init__(self, ExporterSharedData, parent=None):
		QtCore.QObject.__init__(self)
		super(self.__class__, self).__init__()

		self.ExporterSharedData = ExporterSharedData
		self.worker = EmitGif(self.ExporterSharedData, self.ExporterSharedData.curdir, self.ExporterSharedData.fps, self.ExporterSharedData.ss, self.ExporterSharedData.t, self.ExporterSharedData.filters, self.ExporterSharedData.input_avi, self.ExporterSharedData.output_gif)
		self.worker.start()
		self.worker_thread = QtCore.QThread()
		self.worker.moveToThread(self.worker_thread)
		self.worker_thread.start()