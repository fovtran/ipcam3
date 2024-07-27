from PIL import Image
from PIL import ImageChops
import urllib3
from io import StringIO
import base64
import matplotlib.pyplot as plt
from time import sleep
import requests

url = 'http://192.168.2.198/IMAGE.JPG?'
http = urllib3.PoolManager()
filename1 = '/tmp/f1.jpg'
filename2 = '/tmp/f2.jpg'
	
	
def getrequests():
	return requests.get(url).content


def geturllib3():
	return http.request('get',url).data


def decode1(img):
	return base64.b64decode(image)


def convert(image):
	d = StringIO(str(image))
	d.seek(0)
	return d	


def write(image, filename):
	f = open(filename, 'wb')
	f.write(image)
	f.close()


def readopen(filename):
	print('Loading')
	image = getrequests()
	# image = geturllib3()
	write(image, filename)
	im = Image.open(filename)
	# im.thumbnail((256,256))
	return im


print('prepare')
sleep(1)
im1 = readopen(filename1)
print('move')
sleep(2)
im2 = readopen(filename2)
diff = ImageChops.difference(im2, im1)
# diff.save('/tmp/f2.jpg')
plt.imshow(diff)
# Image.show()

back = Image.open('cover.jpg')
image = ImageChops.subtract(im2, im1)
mask1 = Image.eval(image, lambda a: 0 if a <= 1 else 255)
mask2 = mask1.convert('1')
blank = Image.eval(image, lambda a: 0)
new = Image.composite(im1, back, mask2) 
plt.imshow(new)
