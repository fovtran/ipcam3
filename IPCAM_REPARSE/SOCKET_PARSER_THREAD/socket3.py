import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(("0.0.0.0", 0))
s.connect(("192.168.2.198", 80))

print("recv from sock")
buffer = "GET /IMAGE.JPG? HTTP/1.1\r\nhost: 192.168.2.45\r\nUser-agent: Mozilla/4.0\r\nAccept: */*\r\nConnection: keep-alive\r\nConnection: close\r\r\n\r\n"

remote_status = s.send(buffer, 512)
reply_header = s.recv(1024)
reply_content_list = reply_header.split('\r\n\r\n')
segmented_http_header = reply_content_list[0].split('\r\n')
content_length = int(segmented_http_header[-1].replace('Content-Length: ',''))
print(content_length)

data = b''
data+=segmented_http_header[1]

for x in range(content_length):
	buffer = s.recv(1024)
	data += buffer
	if len(buffer)>0:
		continue
	else:
		break

print("Len matches", content_length, content_length==len(data))
s.close()
f = open('/tmp/f3.jpg', 'wb')
f.write(data)
f.close()