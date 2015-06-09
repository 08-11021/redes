/*
*
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
#include <pthread.h> //for threading , link with lpthread
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
 
 typedef struct
{
   bool (*puestos)[10][4];
   int *socket;
   int filas;
   int columnas;
   char * ip;
} params;
//the thread function
pthread_mutex_t lock;

void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    bool puestos[10][4];
    params args;
    int i,j;
    int filas,columnas, port;
    time_t t = time(NULL);
    struct tm tm; 
    system("clear");    
    // reserva_bol_ser -f <filas> -c <col> [-p <puerto>]
    //se establecen todos los puestos como ocupados
    for(i=0; i<10; i++)
    	for(j=0;j<4;j++)
 			puestos[i][j]=true; 	
    args.puestos = &puestos;

 	//validacion de argumentos
 	if (argc < 5) {
		fprintf(stderr,"\nuso: %s -f <filas> -c <col> [-p <puerto>]\n", argv[0]);
		exit(1);
	}else 
		if(strcmp(argv[1], "-f") != 0){
			fprintf(stderr,"\nerror: no se reconoce el parametro %s \n", argv[1]);
			fprintf(stderr,"\nuso: %s -f <filas> -c <col> [-p <puerto>]\n", argv[0]);
			exit(1);
		}else 
			if(strcmp(argv[3], "-c") != 0){
				fprintf(stderr,"\nerror: no se reconoce el parametro %s \n", argv[3]);
				fprintf(stderr,"\nuso: %s -f <filas> -c <col> [-p <puerto>]\n", argv[0]);
				exit(1);
			}
			else{
				filas = atoi(argv[2]);
				columnas = atoi(argv[4]);
				if (filas > 10 || columnas > 4 || filas < 1 || columnas < 1){
					fprintf(stderr,"\nha excedido el rango maximo: -f 10, -c: 4\n",filas,columnas);
					exit(1);
				}else{
					if(argc > 5){
						if (argc ==7)	
							if (strcmp(argv[5], "-p") != 0){
								fprintf(stderr,"\nerror: no se reconoce el parametro %s \n", argv[5]);
								fprintf(stderr,"\nuso: %s -f <filas> -c <col> [-p <puerto>]\n", argv[0]);
								exit(1);
							}else{
								port = atoi(argv[6]);
								if(port == 0){
									fprintf(stderr,"\nerror:%s no es un puerto valido\n", argv[6]);
									fprintf(stderr,"\nuso: %s -f <filas> -c <col> [-p <puerto>]\n", argv[0]);
									exit(1);
								}
							}
						else{
							fprintf(stderr,"\nuso: %s -f <filas> -c <col> [-p <puerto>]\n", argv[0]);
							exit(1);
						}
					}else{
						port = 8888;						
					}
				}
			}
    args.filas = filas;
    args.columnas = columnas;
	//se desocupan la cantidad de puestos establecidos
	for(i=0; i<filas; i++)
    	for(j=0;j<columnas;j++)
 			puestos[i][j]=false; 

    
	//se muestra la matriz que representa el vagon
	/*
    for(i=0; i<args.filas; i++){ 	 	
    	for(j=0;j<args.columnas;j++){
 			if((*args.puestos)[i][j]==true){
 				printf("1-");
 			}else{
 				printf("0-");
 			}
    	}
    	printf("\n");
 	}
    */
    //Crear socket
    /*tcp*/
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    /*udp*/
    //socket_desc = socket(AF_INET , SOCK_DGRAM , 0);

    if (socket_desc == -1)
    {
        printf("\nNo se pudo crear el socket");
    }
    puts("\nSocket creado");
     
    //prepara la estructura sockaddr_in
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = port;

	printf("La dirección ip del servidor es: %s\n",inet_ntoa(server.sin_addr));
	printf("el puerto del servidor es: %d\n",port);
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("fallo el bind. Error");
        return 1;
    }
    puts("bind realizado");
     
    //escucha
    listen(socket_desc , 3);  
     
    //acepta las conexiones entrantes
    puts("Esperando conexiones entrantes...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        


        //se introduce el apuntador al socket en la estructura para enviarla como parametro al hilo
        args.socket = (int*) &client_sock;    
        args.ip=inet_ntoa(client.sin_addr);
        t = time(NULL);
        tm = *localtime(&t);
        fprintf(stderr,"[%d-%d-%d %d:%d:%d]-Solicitud de Conexion-[%s]-Conexion Realizada\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, args.ip);

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &args) < 0)
        {
            t = time(NULL);
            tm = tm = *localtime(&t);
            fprintf(stderr,"[%d-%d-%d %d:%d:%d]-Error-[%s]-No se ha podido crear un hilo para este cliente\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, args.ip);
            return 1;
        }
        //pthread_join( thread_id , NULL);
    }
     
    if (client_sock < 0)
    {
        t = time(NULL);
        tm = *localtime(&t);
        fprintf(stderr,"[%d-%d-%d %d:%d:%d]-Solicitud de Conexion-[%s]-Conexion Rechazada\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, args.ip);
        return 1;
    }
    pthread_mutex_destroy(&lock);

    return 0;
}
 
/*
 * Esto va a manejar las conexiones para cada cliente
 * */
void *connection_handler(void *args)
{
    params data = *(params*)args;
    //data contiene un apuntador a la estructura en data.puestos
    //obtiene el descriptor del socket
    int sock = *(data.socket);
    int read_size;
    char client_message[64];
    char respuesta[64];
    int fila, columna;
    char c[4],f[4];
    bool isFull;
    int i, j, index;
    char numero[] = "0123456789"; 
    time_t t = time(NULL);
    struct tm tm = *localtime(&t); 
        
    //recibe mensajes del cliente
    while( (read_size = recv(sock , client_message , 64 , 0)) > 0 )
    {
    	c[0] =client_message[1];
    	f[0] =client_message[0];
        c[1]='\0';
        f[1]='\0';
    	fila=atoi(f);
    	columna=atoi(c);
               


        /*
        char* arrDayNames[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // Jeez I hope this works, I haven't done this in ages and it's hard without a compiler..
        SYSTEMTIME st;
        GetLocalTime(&st); // Alternatively use GetSystemTime for the UTC version of the time
        printf("The current date and time are: %d/%d/%d %d:%d:%d:%d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        printf("The day is: %s", arrDayNames[st.wDayOfWeek]);
        */

        pthread_mutex_lock(&lock);
        if ((*(data.puestos))[fila][columna]==true){
            isFull = true;
            respuesta[0]='o';
            respuesta[1]=numero[data.filas - 1];
            respuesta[2]=numero[data.columnas - 1];
            index=3;
            //recorre la estructura para ver si quedan puestos libres
            for(i=0; i<data.filas; i++)       
                for(j=0;j<data.columnas;j++)
                    if((*(data.puestos))[i][j]==false){
                        isFull=false;
                        respuesta[index]='0';
                        index++;
                    }else{
                        respuesta[index]='1';
                        index++;
                    }
            respuesta[index]='\0';

            if (isFull == true){
                t = time(NULL);
                tm = *localtime(&t);
                fprintf(stderr,"[%d-%d-%d %d:%d:%d]-Reserva Pos:[%d,%d]-[%s]-Reserva Rechazada, Vagón Full\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,fila+1,columna+1, data.ip);
                send(sock, "f\0", 64,0); //envia "f" que el cliente debe interpretar como que esta ful el vagon
            }else{
                t = time(NULL);
                tm = tm = *localtime(&t);
                fprintf(stderr,"[%d-%d-%d %d:%d:%d]-Reserva Pos:[%d,%d]-[%s]-Reserva Rechazada, Puesto Ocupado\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,fila+1,columna+1, data.ip);
                send(sock, respuesta, 64,0);  //envia al cliente
            }
        }else{
            (*data.puestos)[fila][columna]=true; 
            t = time(NULL);
            tm = *localtime(&t);
            fprintf(stderr,"[%d-%d-%d %d:%d:%d]-Reserva Pos:[%d,%d]-[%s]-Asiento Reservado\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,fila+1,columna+1, data.ip);
            send(sock, "r\0", 64,0);
        }
        pthread_mutex_unlock(&lock);

        


    	//validar el rango de fila y columna del lado del servidor
    	//hacer la reservacion 

		//envia el mensaje de vuelta al cliente
		//limpia el buffer de mensajes
		memset(client_message, 0, 64);
    }
     
    if(read_size == 0)
    {
        t = time(NULL);
        tm = *localtime(&t);
        fprintf(stderr,"[%d-%d-%d %d:%d:%d]-Desconectado-[%s]-Se ha desconectado\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, data.ip);
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv fallo");
    }
         
    return 0;
} 