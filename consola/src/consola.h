/*
 * consola.h
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <commons/string.h>
#include <commons/sockets.h>
#include <commons/log.h>
#include <commons/config.h>

typedef struct{
        char* ip_Kernel;
        char* puerto_Kernel;

}t_config_consola;

t_config_consola* levantarConfiguracionConsola(char * archivo);
int crearLog();
int verificarExistenciaDeArchivo(char*);

//Funciones de interfaz
void levantarInterfaz();
void iniciarPrograma(char* comando, char* param);
void finalizarPrograma(char* comando, char* param);
void desconectarConsola(char* comando, char* param);
void limpiarMensajes(char* comando, char* param);

//Variables Globales

t_log* logger;
t_config_consola* config;

#endif /* CONSOLA_H_ */
