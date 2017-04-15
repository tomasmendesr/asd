/*
 ============================================================================
 Name        : memoria.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "funcionesMemoria.h"

int main(int argc, char** argv){

	logger = log_create("logMemoria","memoria",true,LOG_LEVEL_TRACE);

	crearConfig(argc, argv);

	inicializarMemoria();

	socketEscuchaConexiones = createServer(IP, config->puerto, BACKLOG);

	esperarConexionKernel();

	levantarInterfaz();

	esperarConexiones();

    destruirConfiguracionMemoria(config);

    return EXIT_SUCCESS;
}

void esperarConexionKernel(){

	void* paquete;
	int tipo_mensaje;
	int check;

	socketConexionKernel = acceptSocket(socketEscuchaConexiones);

	if(socketConexionKernel == -1){
		perror("Error al aceptar conexion con el kernel");
		exit(1);
	}

	check = recibir_info(socketConexionKernel, &paquete, &tipo_mensaje);

	//Chequeo de errores
	if (check <= 0){
		perror("recv");
		close(socketConexionKernel);
		exit(1);
	}

	if(tipo_mensaje == HANDSHAKE_KERNEL){
		printf("Conexion con el kernel establecido\n");
		enviar_paquete_vacio(HANDSHAKE_MEMORIA,socketConexionKernel);
		pthread_t threadKernel;
		pthread_create(&threadKernel, NULL, (void*)requestHandlerKernel, &socketConexionKernel);
		pthread_detach(threadKernel);
	}else{
		exit(1);
	}

}

void esperarConexiones(){


	int newSocket;
	void* paquete;
	int tipo_mensaje;
	int check;

	while(1){

		newSocket = acceptSocket(socketEscuchaConexiones);

		if(newSocket == -1)
			perror("Error al aceptar");

		check = recibir_paquete(newSocket, &paquete, &tipo_mensaje);

		//Chequeo de errores
		if (check == 0){
			printf("Se cerro el socket %d\n", newSocket);
			close(newSocket);
		}
		if(check == -1){
			perror("recv");
			close(newSocket);
		}
		//Fin chequeo de errores

		if(check > 0){
			if(tipo_mensaje == HANDSHAKE_CPU){

				printf("Conexion con la CPU establecido\n");
				enviar_paquete_vacio(HANDSHAKE_MEMORIA,newSocket);
				pthread_t threadCpu;
				pthread_create(&threadCpu, NULL, (void*)requestHandlerCpu, &newSocket);
				pthread_detach(threadCpu);

			}else{
				close(newSocket);
				printf("Solo se aceptan conexiones de Cpu\n");
			}
		}
	}
}

