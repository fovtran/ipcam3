# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import
__author__      = "Diego Cadogan"
__copyright__   = "2018"
__version__ = "0.1"
__license__ = "LGPL"

import struct, sys, os, math, platform, time, requests, traceback
import numpy as np
from PyQt5 import QtCore, QtGui, QtWidgets, uic
import wave
from Models import SharedData, WidgetData, ExporterSharedData

sound= False
debug = True

if sound:
	import soundfile as sf
	import sounddevice as sd
	devices = sd.query_devices()
	if debug:
		print(devices)

def get_signal_data(frequency=440, duration=1, volume=32768, samplerate=44100):
	"""Outputs a numpy array of intensities"""
	samples = duration * samplerate
	period = samplerate / float(frequency)
	omega = N.pi * 2 / period
	t = N.arange(samples, dtype=N.float)
	y = volume * N.sin(t * omega)
	return y

class Consumer(QtCore.QThread):
	def __init__(self, SharedData, parent=None):
		QtCore.QThread.__init__(self)
		super(self.__class, self).__init__()

class AudioRecorderThread(QtCore.QThread):
	finished = QtCore.pyqtSignal()

	def __init__(self, SharedData, parent=None):
		QtCore.QThread.__init__(self)
		super(self.__class__, self).__init__()
		self.sem = QtCore.QSemaphore()
		self.lock = QtCore.QMutex()
		self.cliplen = 2
		self.SharedData = SharedData
		self.CHUNK_SIZE = 512
		#sd.default.blocksize = self.CHUNK_SIZE
		self.fs = 44100
		sd.default.samplerate = self.fs
		self.CHANNELS= 1
		sd.default.channels = self.CHANNELS
		sd.default.device = mydev
		sd.default.dtype = 'int16'
		sd.default.never_drop_input = False
		sd.default.latency = ('low','low')
		self.frames = []
		self.stream = sd.Stream(callback=self.callback)
		try:
			print("Remove file")
			os.remove(self.SharedData.audio_filename)
		except:
			print("Remove file failed")

		self.t = 0

	def callback(self, indata, outdata, frames, input_time, status):
		self.sem.tryAcquire(1)
		self.t = time.time()
		print(self.t)
		self.frames.append(indata.copy())
		self.sem.release()

	def run(self):
		print("Start audio streaming")
		with self.stream:
			sd.sleep(self.cliplen * 1000)
			sd.wait()
		self.stream.close()
		self.finish()

	def finish(self):
		print("Closing audio streaming", len(self.frames))
		self.waveFile = wave.open(self.SharedData.audio_filename, 'wb')
		self.waveFile.setparams((1,2, self.fs, 0, 'NONE', 'not compressed'))
		wvData = []
		for block in range(0, len(self.frames)):
			wvData = self.frames[block].astype('int16').tobytes()
			self.waveFile.writeframesraw(wvData)

		self.waveFile.writeframes(b'')
		self.waveFile.close()
		self.stop()

	@staticmethod
	def stop():
		print("file closed")
		sd = None
		framesAll = None
		result = None

class AudioEmitterThread(QtCore.QThread):
	finished2 = QtCore.pyqtSignal()

	def __init__(self, SharedData, parent=None):
		QtCore.QThread.__init__(self)
		super(self.__class__, self).__init__()

		self.SharedData = SharedData
		#self.CHUNK_SIZE = 512
		#sd.default.blocksize = self.CHUNK_SIZE
		self.fs=44100
		sd.default.samplerate = self.fs
		self.CHANNELS= 1
		sd.default.channels = self.CHANNELS
		sd.default.device = mydev
		sd.default.dtype = 'float32'
		sd.default.latency = ('low','low')
		self.frames = []
		print("wav data")

	def run(self):
		#data= wave.open(self.SharedData.audio_filename, 'r')
		volume = 1.0     # range [0.0, 1.0]
		fs = 44100       # sampling rate, Hz, must be integer
		duration = 1.0   # in seconds, may be float
		f = 1000.0        # sine frequency, Hz, may be float
		samples = (np.sin(2*np.pi*np.arange(fs*duration)*f/fs)).astype(np.float32)
		sd.play(samples, self.fs)
		fs = None
		self.stop()

	@staticmethod
	def stop():
		print("Quit wavreader")
		sf = None

	#@staticmethod
	#def __del__():
	#	sys.exit()

class MyWindowClass(QtCore.QObject):
	def __init__(self, parent=None):
		#QtCore.QObject.__init__(self)
		super(MyWindowClass, self).__init__(parent)
		self.ExporterSharedData = ExporterSharedData()

		self.worker = AudioRecorderThread(ExporterSharedData)
		self.worker.start()
		self.worker.wait()
		self.worker = None

		#self.worker2 = AudioEmitterThread(ExporterSharedData)
		#self.worker2.start()
		#self.worker2.wait()
		#self.worker2 = None