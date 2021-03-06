/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "vagon.h"

int
main (int argc, char *argv[])
{
	char *host;

	CLIENT *clnt;
	char  **result_1;
	int columna, columnas;
	int fila, filas,i,j;
	long  posicion;
	int incorrecto;
	char * aux = malloc(45 * sizeof(char));
	char * aux2;
	char atoi_buffer[2];
	aux2 = aux;

	//-----------------------------------------------
	//validacion de argumentos
 	if (argc < 5) {
		fprintf(stderr,"\nuso: %s <ip-servidor> -f <fila> -c <col>\n", argv[0]);
		free(aux2);
		exit(1);
	}else{
		if ( inet_addr(argv[1]) == INADDR_NONE ) {
		    fprintf(stderr,"formato de ip invalido\n");
		    free(aux2);
		    exit(1);
		}else{	
			if(strcmp(argv[2], "-f") != 0){
				fprintf(stderr,"\nerror: no se reconoce el parametro %s", argv[2]);
				fprintf(stderr,"\nuso: %s <ip-servidor> -f <fila> -c <col>\n", argv[0]);
				free(aux2);
				exit(1);
			}else 
				if(strcmp(argv[4], "-c") != 0){
					fprintf(stderr,"\nerror: no se reconoce el parametro %s", argv[4]);
					fprintf(stderr,"\nuso: %s <ip-servidor> -f <fila> -c <col>\n", argv[0]);
					free(aux2);
					exit(1);
				}
				else{
					fila = atoi(argv[3]);
					columna = atoi(argv[5]);
					if (fila > 10 || fila < 1 || columna > 4 || columna < 1){
						fprintf(stderr,"\nrango invalido: -f %d, -c: %d\n",fila,columna);
						free(aux2);
						exit(1);
					}
				}
			
		}
	}



	//------------------------------------------------
	host = argv[1];
	
	

	/*
	div_t output;
	output = div(posicion, 10);
	fila = output.quot;
	columna = output.rem;*/
	
	clnt = clnt_create (host, VAGON_PROG, VAGON_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
	
	do{
		posicion = (fila -1)*10 + (columna -1);
		result_1 = reservar_1(&posicion, clnt);

		if (result_1 == (char **) NULL) {
			clnt_perror (clnt, "call failed");
			free(aux2);
			exit (2);
		}
		//--------------------------------------evaluar respuesta
		if(*result_1[0] == 'r'){
		 	printf("\nReservado\n");
	 		clnt_destroy (clnt);
	 		free(aux2);
		 	exit(0);
		 }else 
		 	if(*result_1[0] == 'f'){
		 		printf("\nVagón completo\n");
	 			clnt_destroy (clnt);
	 			free(aux2);
		 		exit(0);
			 }else if(*result_1[0] == 'o'){
		 		printf("\npuesto ocupado, intente con otra  posición");
		 		printf("\nlos asientos disponibles son los marcados con O, los ocupados estan marcados con X:\n");
		 		aux = *result_1;
		 		aux++;
		 		atoi_buffer[0]=*aux;
		 		atoi_buffer[1]='\0';
		 		filas = atoi(atoi_buffer);
		 		aux++;
		 		atoi_buffer[0]=*aux;
		 		atoi_buffer[1]='\0';		 		
		 		columnas = atoi(atoi_buffer);
		 		filas++;
		 		columnas++;
		 		aux++;
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
		                if(*aux=='0'){
		                    printf("O ");
		                    aux++;
		                }else{
		                    printf("X ");
		                    aux++;
		                }
		            }
		            printf("\n");
		        }	
			 }
		 	incorrecto = 1;
		 	do{

				printf("\nindique el asiento que desea reservar -f <fila> -c <columna>: ");
			    scanf("%s", argv[2]); /* Las cadenas NO llevan & */
			    scanf("%s", argv[3]);
			    scanf("%s", argv[4]);
			    scanf("%s", argv[5]);
			    if(strcmp(argv[2], "-f") != 0){
					fprintf(stderr,"\nerror: no se reconoce el parametro %s", argv[2]);
				}else 
					if(strcmp(argv[4], "-c") != 0){
						fprintf(stderr,"\nerror: no se reconoce el parametro %s", argv[4]);
					}
					else{
						fila = atoi(argv[3]);
						columna = atoi(argv[5]);
						if (fila > 10 || fila < 1 || columna > 4 || columna < 1){
							fprintf(stderr,"\nrango maximo: -f 10, -c 4\n");
						}else{
							incorrecto=0;
						}
					}
			}while(incorrecto);
		//------------------------------------------------
		//codigo del cliente
	}while(*result_1[0] == 'o');
	free(aux2);
	clnt_destroy (clnt);
exit (0);
}
