from PyQt5 import QtCore, QtGui, QtWidgets, uic
from threading import Thread, Event, ThreadError
from concurrent.futures import Future, ThreadPoolExecutor
from multiprocessing import freeze_support, Queue, Process, Pool, cpu_count
import os

def getURI():
	from xml.dom import minidom
	xmldoc = minidom.parse('conf/config.xml')
	itemlist = xmldoc.getElementsByTagName('ip')
	ip = itemlist[0].childNodes[0].data
	return 'http://' + ip +'/video.cgi?.mjpg'

class WidgetData(QtCore.QObject):
	timer1 = QtCore.QElapsedTimer()
	audio_finished = QtCore.pyqtSignal()
	recorder_finished = QtCore.pyqtSignal()
	image = None
	scale = 0
	image = None
	clipart = None
	event = None
	save_dir = os.getcwd()
	full_image_path = None
	filename = None
	counter = 0
	def __init__(self, parent=None):
		QtCore.QObject.__init__(self)
		super(self.__class__, self).__init__()

class SharedData(QtCore.QObject):
	bufsize = 1024
	buffers = 4
	blksize = bufsize*buffers
	timer1 = QtCore.QElapsedTimer()
	current_frame = QtCore.pyqtSignal(object)
	frame_processor_finished = QtCore.pyqtSignal()
	dispatcher_finished = QtCore.pyqtSignal()
	MyThreadPool = QtCore.QThreadPool()
	threadcount = MyThreadPool.maxThreadCount()
	loopcounter = [0] * threadcount
	stream_URI = getURI()
	timestart = 0
	timeend = 0
	errors = False
	framequeue = []   # or Queue()
	loopdelay = [0.0010, 0.0020]
	width = 640
	height = 480
	recording = False
	fps = 10
	audio_enabled = False
	blank_image = False
	videoframe = None
	IsStream = False

	def __init__(self, parent=None):
		QtCore.QObject.__init__(self)
		super(self.__class__, self).__init__()

class ExporterSharedData(QtCore.QObject):
	filters = ""
	curdir = os.getcwd()
	filename = os.path.join(curdir, 'output.avi')
	input_avi = "output.avi"
	output_gif = "output.gif"
	t = "12"
	ss = "0"
	fps = 10
	audio_filename = os.path.join(curdir, 'output.wav')
	audio_filename2 = os.path.join(curdir, 'discipline.wav')
	bufsize = 1024
	buffers = 6
	blksize = bufsize*buffers
	def __init__(self, parent=None):
		QtCore.QObject.__init__(self)
		super(self.__class__, self).__init__()