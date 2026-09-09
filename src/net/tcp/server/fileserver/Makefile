fileserver: filesystem net os_layer utils server
	gcc os_layer.o utils.o filesystem.o net.o server.o -o fileserver

server:
	gcc -c server.c -o server.o

net:
	gcc -c net.c -o net.o

filesystem:
	gcc -c filesystem.c -o filesystem.o

utils:
	gcc -c utils.c -o utils.o

os_layer:
	gcc -c os_layer.c -o os_layer.o

clean_all:
	rm *.o