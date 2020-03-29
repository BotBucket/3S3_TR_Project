default: serv_tcp client_tcp

serv_tcp: serv_tcp.c
	cc -Wall -D_REENTRANT -o serv_tcp serv_tcp.c -pthread

client_tcp: client_tcp.c
	cc -Wall -o client_tcp client_tcp.c
clean:
	rm -f serv_tcp client_tcp
