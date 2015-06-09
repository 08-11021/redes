all: server client

server: servidor.c 
	gcc servidor.c -lpthread -o reserva_bol_ser

client: cliente.c
	gcc cliente.c -o reserva_bol_cli

clean: reserva_bol_ser reserva_bol_cli
	rm reserva_bol_ser reserva_bol_cli