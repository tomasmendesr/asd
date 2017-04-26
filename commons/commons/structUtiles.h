/*
 * StructsUtiles.h
 *
 *  Created on: 11/4/2017
 *      Author: utnso
 */

#ifndef STRUCTSUTILES_H_
#define STRUCTSUTILES_H_

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "collections/list.h"
#include "collections/queue.h"
#include "config.h"
#include "log.h"
#include "collections/dictionary.h"

typedef struct{
	uint32_t pagina;
	uint32_t offset;
	uint32_t size;
}__attribute__((__packed__)) t_argumento;

typedef struct{
	uint32_t pagina;
	uint32_t offset;
	uint32_t size;
}__attribute__((__packed__)) t_posicion;

typedef struct{
	uint32_t pid;
	uint32_t pag;
	uint32_t offset;
	uint32_t size;
}__attribute__((__packed__)) pedido_bytes_t;

typedef struct{
	char idVariable;
	uint32_t pagina;
	uint32_t offset;
	uint32_t size;
}__attribute__((__packed__)) t_var_local;

typedef struct{
	t_list* argumentos;
	t_list* variables;
	int32_t direcretorno;
	t_posicion * retVar;
}__attribute__((__packed__)) t_entrada_stack;

//t_dictionary* etiquetas;

typedef struct indiceCodigo{
	uint32_t offset;
	uint32_t size;
}__attribute__((__packed__)) t_indice_codigo;


typedef struct{
	uint32_t pid;  //Identificador único del Programa en el sistema
	uint32_t programCounter; //Número de la próxima instrucción a ejecutar
	uint32_t cantPaginasCodigo;
	t_list* indiceCodigo;
	t_list* indiceStack;
	uint32_t exitCode;
	uint32_t consolaFd;
	char* etiquetas;
	uint32_t stackPointer; // el ultimo offset
	uint32_t tamanioEtiquetas;
	uint32_t codigo; // cant de instrucciones
}__attribute__((__packed__)) pcb_t;

typedef enum{
	ERROR, NOTHING, SUCCESS
} opciones_generales_ops;


//Auxiliares
typedef struct {
	uint32_t tamanioBuffer;
	char *buffer;
}__attribute__((__packed__))  t_buffer_tamanio;

typedef struct {
	uint32_t tamanioStack;
	void * stack;
}__attribute__((__packed__)) t_tamanio_stack_stack;



#endif /* STRUCTSUTILES_H_ */
