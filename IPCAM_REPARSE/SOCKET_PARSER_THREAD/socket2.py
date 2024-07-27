import socket

#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.bind(("0.0.0.0", 0))

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(("0.0.0.0", 0))
s.connect(("192.168.2.198", 80))

print("recv from sock")
buffer = b"GET /IMAGE.JPG? HTTP/1.1\r\nhost: 192.168.2.45\r\nUser-agent: Mozilla/4.0\r\nAccept: */*\r\nConnection: keep-alive\r\nConnection: close\r\r\n\r\n"
a = s.send(buffer, 512)
b = s.recv(1024)
c = b.split(b'\r\n\r\n')
d = c[0].split(b'\r\n')
size = int(d[-1].replace(b'Content-Length: ',b''))
print(size)

data = b''
data+=c[1]

for x in range(size):
	D = s.recv(1024)
	data += D
	if len(D)>0:
		print("Len of data", len(c[1]), len(data))
	else:
		break

print("Len matches", size, size==len(data))
s.close()
# socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC, IPPROTO_IP) = 3
# bind(3, {sa_family=AF_INET, sin_port=htons(0), sin_addr=inet_addr("0.0.0.0")}, 16) = 0

"""
However, some use cases exist where you might want to bind to an address thats not configured on any network interface at all. One of the canonical examples is when you want the return traffic for a given connection to pass through a load balancer, and leave it up to the load balancer to determine where to ultimately deliver the traffic.
There are a few ways you can do this in Linux, but well look at two. The first is through a socket option called IP_FREEBIND (described in Linuxs IPv4 protocol documentation). Socket options are per-socket configuration settings that you can specify after initially creating the socket. This is done through the setsockopt() syscall. Linux uses an integer value of 15 for the IP_FREEBIND option, so thats what we need to pass in to setsockopt(), while also specifying a value of 1, indicating this option should be enabled. Python also makes this easy:
# creating this, for convenience (will be used in later examples)
IP_FREEBIND = 15

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_IP, IP_FREEBIND, 1)
s.bind(("192.168.123.123", 0))
"""
"""
# Note the AF_INET6 address family
s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
# Matches the previously-configured Any-IP route
s.bind(("2001:db8:123::1", 0))
"""
"""
s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_IP, IP_FREEBIND, 1)
s.bind(("2001:db8:123::1", 0))
"""

"""
kprobe:inet_addr_type_table
{
    printf("inet_addr_type_table finding type for %s\n", ntop(arg1));
}

kretprobe:inet_addr_type_table
{
    printf("inet_addr_type_table returned: %d\n", retval);
}

We will pass this file to bpftrace, and once we see the message Attaching 2 probes..., we can open a few sockets in a separate process. Heres the output from bpftrace when we bind to a few different addresses:

~$ bpftrace addr_type_trace.bt
"""