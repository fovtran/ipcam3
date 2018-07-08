# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import
__author__      = "Diego Cadogan"
__copyright__   = "2018"
__version__ = "0.1"
__license__ = "LGPL"

import sys, os, platform, time, requests, traceback
import msvcrt as m
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from subprocess import Popen, call, PIPE, STDOUT

class EmitGif(QtCore.QThread):
	finished = QtCore.pyqtSignal(str)

	def __init__(self, SharedData, curdir, fps, ss, t, filters, input_avi, output_gif, parent=None):
		QtCore.QThread.__init__(self)
		#super(self.__class__, self).__init__(parent)
		self.p = parent
		compiler = "ffmpeg.exe"
		compiler_dir = "ffmpeg-win64-static\\bin"
		self.SharedData = SharedData
		self.curdir = os.path.join(curdir, compiler_dir)
		self.our_compiler = os.path.join(self.curdir, compiler)
		print(self.our_compiler)
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
		print("Init", self.our_compiler)

	def call_command(self, cmd):
		print(cmd)
		p = Popen(cmd, universal_newlines=True, shell=True, stdout=PIPE, stderr=STDOUT)
		text = p.stdout.read()
		p.stdout.close()
		retcode = p.wait()
		print(text)
		return retcode

	def EmitGifDone(self, res):
		print(res)
		sys.exit()

	def run(self):
		print("Launch")
		if os.path.exists(self.input_avi):
			if os.path.exists(self.our_compiler):
				print("Palette operation")
				self.call_command([self.our_compiler] + self.cmd_palette)
				print("Video builder")
				self.call_command([self.our_compiler] + self.cmd_gif)
				self.finished.emit( "DONE")
			else:
				print ("No video compiler found")
				self.emit( SIGNAL("nocompiler"), "ERR")
		else:
			print ("No input file found")
			self.emit( SIGNAL("noinput"), "ERR")
		return

	def __del__(self):
		print("Stop")
		self.quit()
		self.wait()

class ExporterSharedData(QtCore.QObject):
		filters = ""
		curdir = os.getcwd()
		input_avi = "output.avi"
		output_gif = "output.gif"
		t = "12"
		ss = "0"
		fps = 10

class MyWindowClass(QtCore.QObject):
	def __init__(self, parent=None):
		QtCore.QObject.__init__(self)
		super(MyWindowClass, self).__init__(parent)
		self.ExporterSharedData = ExporterSharedData()

		self.worker = EmitGif(self.ExporterSharedData, self.ExporterSharedData.curdir, self.ExporterSharedData.fps, self.ExporterSharedData.ss, self.ExporterSharedData.t, self.ExporterSharedData.filters, self.ExporterSharedData.input_avi, self.ExporterSharedData.output_gif)
		self.worker.start()
		self.worker_thread = QtCore.QThread()
		self.worker.moveToThread(self.worker_thread)
		self.worker_thread.start()

if __name__ == "__main__":
	app = QtWidgets.QApplication(sys.argv)
	ex = MyWindowClass()
	sys.exit(app.exec_())
