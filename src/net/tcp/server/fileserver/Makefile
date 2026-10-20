fileserver: log_queue log filesystem net os_layer utils conn_pool server
	gcc log_queue.o log.o os_layer.o utils.o filesystem.o conn_pool.o net.o server.o -o fileserver

server:
	gcc -c server.c -o server.o -pthread

net:
	gcc -c net.c -o net.o

filesystem:
	gcc -c filesystem.c -o filesystem.o

utils:
	gcc -c utils.c -o utils.o

os_layer:
	gcc -c os_layer.c -o os_layer.o

conn_pool:
	gcc -c conn_pool.c -o conn_pool.o

log: log_queue
	gcc -c log.c -o log.o

log_queue:
	gcc -c log_msg_queue.c -o log_queue.o

clean_all:
	rm *.o