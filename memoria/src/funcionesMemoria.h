/*
 * memoria.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#ifndef FUNCIONESMEMORIA_H_
#define FUNCIONESMEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <commons/config.h>
#include <commons/sockets.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/interface.h>
#include <commons/sharedStructs.h>
#include <commons/cosas.h>
#include <pthread.h>

#define configuracionMemoria "confMemoria.init"
#define MAX_LEN_PUERTO 6
#define IP "127.0.0.1"
#define BACKLOG 10

//Defines para escribir menos
#define cant_frames config->marcos
#define frame_size config->marcos_Size
#define cache_entradas config->entradas_Cache
#define max_entradas config->cache_x_Proceso

typedef struct{
        char* puerto;
        int marcos;
        int marcos_Size;
        int entradas_Cache;
        int cache_x_Proceso;
        char* reemplazo_cache;
        int retardo_Memoria;
}t_config_memoria;

typedef struct{
	int pid;
	int pag;
}t_entrada_tabla;

typedef struct{
	int pid;
	int pag;
	char* content;
	unsigned long int time_used; //Cual fue la ultima vez que se utilizo
}t_entrada_cache;

t_config_memoria* levantarConfiguracionMemoria(char* archivo);
void crearConfig(int argc, char* argv[]);
void destruirConfiguracionMemoria(t_config_memoria* config);

//Funciones de conexionado
void esperarConexiones();
void esperarConexionKernel();
/* Esta funcion hace la creacion de la memoria y todas las estructuras
 * administrativas necesarias para que el sistema arranque
 */
void inicializarMemoria();

//Funciones administracion memoria
int framesLibres();
int buscarFrame(int pid, int pag);
int escribir(int pid, int pag, int offset, char* contenido, int size); //Devuelve codigos error
int leer(int pid, int pag, int offset, int size, char* resultado); //Devuelve codigos error

//Funciones cache
void increaseOpCount(); //Suma uno al opCount
/* Cuantas entradas tiene el pid */
int cantEntradas(int pid);
/* Busca la entrada con pid y pag. Si no existe retorna -1*/
bool buscarEntrada(int pid, int pag);
/* Esta funcion aplica el LRU y me dice que entrada debo reemplazar
 * en caso de que esten todas ocupadas. Necesita el pid para no pasarse
 * del límite de entradas por proceso*/
int entradaAReemplazar(int pid);
int reemplazoLocal(int pid);
int reemplazoGlobal();
/* Busca la entrada que coincida con pid y pag, y devuelve el puntero contenido de la entrada
 * Devuelve 0 en caso de que exista la entrada, -1 en caso contrario*/
int leerCache(int pid, int pag, char* contenido);
/* Se llena una entrada de la cache con los valores pasados por parametro.
 * Si ya existe la entrada, se usa esa misma.
 * El puntero frame apunta al comienzo del frame referenciado por pid y pag*/
int actualizarEntradaCache(int pid, int pag, char* frame);

 	 	 	 	 	 				/*Este thread maneja tanto cpus como kernel, porque la interfaz es una sola.*/
void requestHandlerKernel(int* fd);		/* Solo una de las operaciones esta restringida a Kernel,*/
void requestHandlerCpu(int* fd);		/* asi que validamos eso solo*/

int iniciarPrograma(int pid, int cantPag);
int asignarPaginas(int pid, int cantPag);
int finalizarPrograma(int pid);
int solicitudBytes(int pid, int pag, int offset, int size, void*buff);
int grabarBytes(int pid, int pag, int offset, int size,void* buff);

//Funciones de interfaz
void levantarInterfaz();
void retardo(char* comando, char* param);
void dump(char* comando, char* param);
void flush(char* comando, char* param);
void size(char* comando, char* param);

//Variables Globales
t_log* logger;
t_config_memoria* config;
int socketEscuchaConexiones;
int socketConexionKernel;
int socketConexionCpu;

char* memoria; /*Este va a ser el bloque que simula la memoria principal.
				Uso char* porque sizeof(char) = 1 y facilita la aritmetica,
				pero no tiene nada que ver con caracteres*/
t_entrada_cache* cache;

unsigned long int op_count; /*Esto vendría a ser nuestro tiempo de referencia para el algoritmo LRU.
 	 	 	 			 Cada vez que se realiza una operación en memoria, se incrementa.*/

#endif /* FUNCIONESMEMORIA_H_ */
