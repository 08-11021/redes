/*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define SERVER_PORT 4321
#define BUFFER_LEN 1024

main(int argc , char *argv[]){
int sockfd;
 int len;
 struct sockaddr_in address;
 int result;
 char ch[64];
 char c[64];
 int buf;
 int inicio = 0;
 char cs[64];
 int bufs;
 int ciclo =1;
 char ipserver[10];
 int port, filas, columnas;
 int intentos=3;
 char numero[10]="0123456789";
 
system("clear");
//reserva_bol_cli <ip-servidor> -p <puerto servicio> -f <fila> -c <col>
 
//validacion de argumentos
 	if (argc < 8) {
		fprintf(stderr,"\nuso: %s <ip-servidor> -p <puerto servicio> -f <fila> -c <col>\n", argv[0]);
		exit(1);
	}else{
		if ( inet_addr(argv[1]) == INADDR_NONE ) {
		    fprintf(stderr,"formato de ip invalido\n");
		    exit(1);
		}else{			
			if(strcmp(argv[2], "-p") != 0){
					fprintf(stderr,"\nerror: no se reconoce el parametro %s", argv[2]);
					fprintf(stderr,"\nuso: %s <ip-servidor> -p <puerto servicio> -f <fila> -c <col>\n", argv[0]);
					exit(1);
			}else{
				port = atoi(argv[3]);
				if(port == 0){
					fprintf(stderr,"\nerror:%s no es un puerto valido", argv[3]);
					fprintf(stderr,"\nuso: %s <ip-servidor> -p <puerto servicio> -f <fila> -c <col>\n", argv[0]);
					exit(1);
				}else
					if(strcmp(argv[4], "-f") != 0){
						fprintf(stderr,"\nerror: no se reconoce el parametro %s", argv[4]);
						fprintf(stderr,"\nuso: %s <ip-servidor> -p <puerto servicio> -f <fila> -c <col>\n", argv[0]);
						exit(1);
					}else 
						if(strcmp(argv[6], "-c") != 0){
							fprintf(stderr,"\nerror: no se reconoce el parametro %s", argv[6]);
							fprintf(stderr,"\nuso: %s <ip-servidor> -p <puerto servicio> -f <fila> -c <col>\n", argv[0]);
							exit(1);
						}
						else{
							filas = atoi(argv[5]);
							columnas = atoi(argv[7]);
							if (filas > 10 || filas < 1 || columnas > 4 || columnas < 1){
								fprintf(stderr,"\nrango invalido: -f %d, -c: %d\n",filas,columnas);
								exit(1);
							}
						}
			}
		}
	}

/*tcp*/
 sockfd = socket(AF_INET, SOCK_STREAM,0);
 /*udp*/
 //sockfd = socket(AF_INET, SOCK_DGRAM,0);
 //llenado de la estructura de datos
 address.sin_family = AF_INET;
 address.sin_addr.s_addr = inet_addr(argv[1]);
 address.sin_port = atoi(argv[3]);
 len = sizeof(address);
 
//conectar con el server
 do{
 	result = connect(sockfd, (struct sockaddr *)&address, len);
 	fprintf(stderr,"\nConectando...");
 	intentos--;
 	if (result == -1){
 		sleep(2);
 		if (intentos == 0){
 			perror("\nNo se ha podido establecer la conexion con el servidor\n");
	 		close(sockfd);
	 		exit(1);
 		} 
 	}
 }while(result ==-1 && intentos > 0);

 
//validar el inicio de sesion
 if(inicio==0){
 printf("\n--------------- Conexion establecida --------------\n");
 inicio = 1;
 }
 int index,i,j;
while(ciclo){
 c[0]=numero[filas-1];
 c[1]=numero[columnas-1];
 c[2]='\0';
//write(sockfd , c , strlen(c));
  send(sockfd,c, 64,0);

 recv(sockfd, ch, 64,0);
 if(ch[0] == 'r'){
 	printf("\nReservado\n");
 	exit(1);
 }else 
 	if(ch[0] == 'f'){
 		printf("\nVagón completo\n");
 		exit(1);
	 }else if(ch[0] == 'o'){
	 	c[0] = ch[1];
	 	c[1] = '\0';
	 	filas = atoi(c);
	 	c[0] = ch[2];
	 	c[1] = '\0';
	 	columnas= atoi(c);
	 	filas++;
	 	columnas++;
	 	index = 3;
 		printf("\npuesto ocupado, intente con otra  posición",ch);
 		printf("\nlos asientos disponibles son los marcados con O, los ocupados estan marcados con X:\n");
 		printf("\n   ");
 		for (j=0;j<columnas;j++)
 			printf("%d ",j+1);
 		printf("\n");
 		for(i=0; i<filas; i++){ 
 			if(i<9){ 
 		    	printf(" %d|",i+1);
            }else{
                printf("%d|",i+1);
            }     
            for(j=0;j<columnas;j++){          	
                if(ch[index]=='0'){
                    printf("O ");
                    index++;
                }else{
                    printf("X ");
                    index++;
                }
            }
            printf("\n");
        }	
	 }
 	int filasAux = filas;
 	int columnasAux = columnas;
 	int incorrecto = 1;
 	do{
	printf("\nindique el asiento que desea reservar -f <fila> -c <columna>: ");
    scanf("%s", argv[4]); /* Las cadenas NO llevan & */
    scanf("%s", argv[5]);
    scanf("%s", argv[6]);
    scanf("%s", argv[7]);
    if(strcmp(argv[4], "-f") != 0){
		fprintf(stderr,"\nerror: no se reconoce el parametro %s", argv[4]);
	}else 
		if(strcmp(argv[6], "-c") != 0){
			fprintf(stderr,"\nerror: no se reconoce el parametro %s", argv[6]);
		}
		else{
			filas = atoi(argv[5]);
			columnas = atoi(argv[7]);
			if (filas > filasAux || filas < 1 || columnas > columnasAux || columnas < 1){
				fprintf(stderr,"\nrango maximo: -f %d, -c: %d\n",filasAux,columnasAux);
			}else{
				incorrecto=0;
			}
		}
	}while(incorrecto);
 }
 
close(sockfd);
 
}