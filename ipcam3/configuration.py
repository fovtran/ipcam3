# -*- coding: utf-8 -*-
import os
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from configparser import SafeConfigParser

class config(QtCore.QObject):
	DEBUGLEVEL = 2

	def __init__(self):
		# Configuration
		pass

	def read(self, f):
		pass

	def write(self, f):
		pass

class ConfigRead(QtCore.QObject):
	def __init__(self, cf, parent=None):
		QtCore.QObject.__init__(self)
		super(self.__class__, self).__init__()

		self.cf = cf
		self.config = SafeConfigParser()
		self.source1 = None
		self.last_files = ''
		self.save_dir = os.getcwd()
		self.image_counter = 0
		self.video_counter = 0
		self.initSections()

	def initSections(self):
		if os.path.exists(self.cf):
			self.config.read(self.cf)
			if not self.config.has_section('main'):
				self.config.add_section('main')
				self.config.set('main', 'source1', 'nada')
				self.config.set('main', 'last_files', str(self.last_files))
				if self.config.has_option('main', 'video_counter'):
					self.video_counter = self.config.getint('main', 'video_counter')
				else:
					self.video_counter = 0
					self.config.set('main', 'video_counter', str(self.video_counter))

				if self.config.has_option('main', 'image_counter'):
					self.image_counter = self.config.getint('main', 'image_counter')
				else:
					self.image_counter = 0
					self.config.set('main', 'image_counter', str(self.image_counter))


				if self.config.has_option('main', 'save_dir'):
					self.save_dir = self.config.get('main', 'save_dir')
				else:
					self.save_dir= 	self.save_dir = os.getcwd()
					self.config.set('main', 'save_dir', str(self.save_dir))

				with open(self.cf, 'w') as f:
					self.config.write(f)

	def Writer(self):
		try:
			if os.path.exists(self.cf):
				if len(self.last_files)>50:
					self.last_files = ''
				self.config.set('main', 'source1', 'nada')
				self.config.set('main', 'last_files', str(self.last_files))
				self.config.set('main', 'video_counter', str(self.video_counter))
				self.config.set('main', 'image_counter', str(self.image_counter))
				self.config.set('main', 'save_dir', str(self.save_dir))
				with open(self.cf, 'w') as f:
					self.config.write(f)
				return True
			else:
				with open(self.cf, 'w') as f:
					self.config.write(f)
				return True
		except IOError:
			print("Error writing config file.")
			return False

	def Reader(self):
		try:
			if os.path.exists(self.cf):
				self.config.read(self.cf)
				self.video_counter = self.config.getint('main', 'video_counter')
				self.image_counter = self.config.getint('main', 'image_counter')
				self.last_files = self.config.get('main', 'last_files').split(',')
				self.save_dir = self.config.get('main', 'save_dir')
				return True
			else:
				return False
		except IOError:
			raise MyError()