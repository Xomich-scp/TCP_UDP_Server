# TCP_UDP_Server

TCP and UDP multiclient Server. Uses select(). 

make -f make to compile.
./server to launch.
By default uses 127.0.0.2 ip and 3000 port for tcp clients and 4000 port for udp. If you want to change it, type no after start and write your ip and port.

Server read data from client, then count sum of number in message.
Example:
Input: 
20 apples, 30 bananas, 15 peaches and 1 watermelon

Reply:
1 15 20 30
66

If there are no numbers in input, then it just send input back
