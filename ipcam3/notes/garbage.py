#from PIL import ImageGrab, Image, ImageTk
#import scipy.io.wavfile
#from scipy.io import wavfile

#img = ImageGrab.grab(bbox=(0,0,640,480))
#(h, w) = frame.shape[:2]
#zeros = np.zeros((h, w), dtype="uint8")
#(B, G, R) = cv2.split(frame)
#R = cv2.merge([zeros, zeros, R])
#G = cv2.merge([zeros, G, zeros])
#B = cv2.merge([B, zeros, zeros])
#output = np.zeros((h, w, 3), dtype="uint8")
#output[0:h, 0:w] = frame
#output[0:h, w:w * 2] = R
#output[h:h * 2, w:w * 2] = G
#output[h:h * 2, 0:w] = B
#img_np = np.array(frame)
#output = cv2.flip(frame,180)

#time.sleep(self.SharedData.loopdelay[1])
#QtCore.QThread.sleep(.5)

#self.freq_vect = np.fft.rfftfreq(self.CHUNK_SIZE, 1./ self.fs)
#self.time_vect = np.arange(self.CHUNK_SIZE, dtype=np.int16) / self.fs * 1000
#print(self.freq_vect, self.time_vect)

if __name__ == "__main__":
	#app = QtWidgets.QApplication(sys.argv)
	ex = MyWindowClass()
	#sys.exit(app.exec_())

#result = np.fromstring(framesAll, dtype=np.int16) #final = np.reshape(result, (len(result), 1))
#wavfile.write(self.SharedData.audio_filename, self.fs, result)

#mutex = QtCore.QMutex()
#mutexlocker = QtCore.QMutexLocker(mutex)
#condition = QtCore.QWaitCondition()
#bytes = b''
#ticks = [0] * threadcount
#(STATE_WAITING, STATE_RUNNING, STATE_FINISHED, STATE_CANCELED) = list(range(4))
#evt = QtCore.QEvent(QtCore.QEvent.MouseButtonPress)


#	def eventFilter(self, obj, event):
#		self.WidgetData.event = event
#		if self.WidgetData.last_event == EVT_DOUBLECLICK:
#			self.WidgetData.last_event = None
#			if not self.WidgetData.recording:
#				self.WidgetData.recording = True
#			else:
#				self.WidgetData.recording = False
#			if debug:	print("Event Filtered")
#			self.clicked.emit(self.WidgetData.recording)
#			return True
#
#		if self.WidgetData.last_event == EVT_CTX:
#			self.WidgetData.last_event = None
#			self.clicked.emit(EVT_CTX)
#			return True
#		return False