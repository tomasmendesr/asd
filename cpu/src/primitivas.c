/*
 * primitivas.c
 *
 *  Created on: 17/4/2017
 *      Author: utnso
 */
#include "primitivas.h"

void setPCB(pcb_t * pcbDeCPU){
	pcb = pcbDeCPU;
}

t_puntero definirVariable(t_nombre_variable identificador_variable){

	if(pcb->stackPointer + TAMANIO_VARIABLE > tamanioStack*tamanioPagina){
		/*esta verificacion me hace ruido*/
		if(!huboStackOver){
			log_error(logger, "StackOverflow. Se finaliza el proceso");
			huboStackOver = true;
		}
		return -1;
	}
	if(!esArgumento(identificador_variable)){
		log_debug(logger, "Definir variable %c", identificador_variable);
		t_var_local* nuevaVar = malloc(sizeof(t_var_local));
		t_entrada_stack* lineaStack = list_get(pcb->indiceStack, pcb->indiceStack->elements_count-1);

		if(lineaStack == NULL){
			lineaStack = malloc(sizeof(t_entrada_stack));
			lineaStack->retVar = NULL;
			lineaStack->direcretorno = -1;
			lineaStack->argumentos = NULL;
			lineaStack->variables = list_create();
			list_add(pcb->indiceStack, lineaStack);
		}
		nuevaVar->idVariable = identificador_variable;
		nuevaVar->pagina = pcb->stackPointer / tamanioPagina + pcb->cantPaginasCodigo; /*por la posicion en memoria*/
		nuevaVar->offset = pcb->stackPointer % tamanioPagina;
		nuevaVar->size = TAMANIO_VARIABLE;
		list_add(lineaStack->variables, nuevaVar);
		pcb->stackPointer+=TAMANIO_VARIABLE;

		log_debug(logger, "memoria -> %c %i %i %i", nuevaVar->idVariable, nuevaVar->pagina,
				nuevaVar->offset, nuevaVar->size);
		log_debug(logger, "Posicion absoluta de %c: %i", identificador_variable, pcb->stackPointer-TAMANIO_VARIABLE );
		return pcb->stackPointer-TAMANIO_VARIABLE;

	}else{
		log_debug(logger, "Definir variable - argumento %c", identificador_variable);
		t_argumento* nuevoArg = malloc(sizeof(t_argumento));
		t_entrada_stack* lineaStack = list_get(pcb->indiceStack, pcb->indiceStack->elements_count -1);

		if(lineaStack == NULL){
			lineaStack = malloc(sizeof(t_entrada_stack));
			lineaStack->retVar = NULL;
			lineaStack->direcretorno = pcb->programCounter;
			lineaStack->argumentos = list_create();
			lineaStack->variables = list_create();
			list_add(pcb->indiceStack, lineaStack);
		}
		nuevoArg->pagina = pcb->stackPointer / tamanioPagina + pcb->cantPaginasCodigo;
		nuevoArg->offset = pcb->stackPointer % tamanioPagina;
		nuevoArg->size = TAMANIO_VARIABLE;
		list_add(lineaStack->argumentos, nuevoArg);
		pcb->stackPointer += TAMANIO_VARIABLE;

		log_debug(logger, "memoria -> %c %i %i %i", identificador_variable, nuevoArg->pagina, nuevoArg->offset, nuevoArg->size);
		log_debug(logger, "Posicion absoluta de %c: %i", identificador_variable, pcb->stackPointer-TAMANIO_VARIABLE );
		return pcb->stackPointer-TAMANIO_VARIABLE;
	}
}

void asignar(t_puntero direccion_variable, t_valor_variable valor){

	log_debug(logger, "Asignar. Posicion %d - Valor %d", direccion_variable, valor);
		//calculo la posicion de la variable en el stack mediante el desplazamiento
		pedido_bytes_t* enviar = malloc(sizeof(uint32_t) * TAMANIO_VARIABLE);
		enviar->pag = direccion_variable / tamanioPagina + pcb->cantPaginasCodigo;
		enviar->offset = direccion_variable % tamanioPagina;
		enviar->size = TAMANIO_VARIABLE;
		enviar->pid = pcb->pid;

//		void* buffer = malloc(sizeof(uint32_t));
//		sprintf(buffer, "%d", valor);

		if(almacenarBytes(enviar, &valor) != 0){
			log_error(logger, "La variable no pudo asignarse. Se finaliza el Proceso.");
			//por ahora pongo un error generico "error_memoria"
			enviar_paquete_vacio(FIN_ERROR_MEMORIA,socketConexionKernel);
			free(enviar);
//			free(buffer);
			//cambia de proceso antes de salir de aca
			return;
		}else{
			log_info(logger, "Variable asignada");
		}
		free(enviar);
//		free(buffer);
		return;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	printf("asignarVariableCompartida!\n");
	return 0;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){

	//calculo la posicion de la variable en el stack mediante el desplazamiento
	t_posicion posicionRet;
	void* paquete;
	//esto se podria poner en una pequeña funcion
	posicionRet.pagina = direccion_variable / tamanioPagina + pcb->cantPaginasCodigo;
	posicionRet.offset = direccion_variable % tamanioPagina;
	posicionRet.size = TAMANIO_VARIABLE;
	pedido_bytes_t* solicitar = malloc(sizeof(pedido_bytes_t));
	solicitar->pag = posicionRet.pagina;
	solicitar->size = posicionRet.size;
	solicitar->offset = posicionRet.offset;
	solicitar->pid = pcb->pid;

	if(solicitarBytes(solicitar, &paquete)!=0){
		free(solicitar);
		return -1;
	}
	free(solicitar);
	return *(t_valor_variable*)paquete;
}

void finalizar(void){
	printf("finalizar!\n");
	return;
}
void irAlLabel(t_nombre_etiqueta t_nombre_etiqueta){
	printf("irAlLabel!\n");
	return;
}
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){
	printf("llamarConRetorno!\n");
	return;
}
void llamarSinRetorno(t_nombre_etiqueta etiqueta){
	printf("llamarSinRetorno!\n");
	return;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){

	uint32_t i;
	t_entrada_stack* entrada;
	t_var_local* var_local;
	t_argumento* argumento;
	t_puntero puntero;

	if(pcb->indiceStack->elements_count==0){
		return EXIT_FAILURE;
	}else{
		entrada = list_get(pcb->indiceStack, pcb->indiceStack->elements_count-1);

		if(!esArgumento(identificador_variable)){

			for(i=0; i<entrada->variables->elements_count; i++){
				var_local=list_get(entrada->variables,i);
				if(var_local->idVariable==identificador_variable)
					break;
			}
			if(entrada->variables->elements_count==i)
				return EXIT_FAILURE;
			else{
				puntero=(var_local->pagina-pcb->cantPaginasCodigo)*tamanioPagina+var_local->offset;
			}
		}
		else{
			if(identificador_variable > entrada->argumentos->elements_count){
				return EXIT_FAILURE;
			}else{
				argumento=list_get(entrada->argumentos,identificador_variable);
				puntero=(argumento->pagina-pcb->cantPaginasCodigo)*tamanioPagina+argumento->offset;
			}
		}
	}

	log_debug(logger, "la posicion absoluta de la variable es: %d", puntero);
	return puntero;
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	printf("obtenerValorCompartida!\n");
	return 0;
}
void retornar(t_valor_variable retorno){
	printf("retornar!\n");
	return;
}
t_descriptor_archivo abrir(t_direccion_archivo direccion, t_banderas flags){
	printf("abrir!\n");
	return 0;
}
void borrar(t_descriptor_archivo direccion){
	printf("borrar!\n");
}
void cerrar(t_descriptor_archivo descriptor_archivo){
	printf("cerrar!\n");
}
void escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio){
	printf("escribir!\n");
}
void leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio){
	printf("leer!\n");
}
void liberar(t_puntero puntero){
	printf("liberar!\n");
}
void moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion){
	printf("moverCursor!\n");
}
t_puntero reservar(t_valor_variable espacio){
	printf("reservar!\n");
	return 0;
}
void signalAux(t_nombre_semaforo identificador_semaforo){
	printf("signal!\n");
}
void wait(t_nombre_semaforo identificador_semaforo){
	printf("wait!\n");
}

void inicializarFunciones(void){
	funciones = malloc(sizeof(AnSISOP_funciones));
	funcionesKernel = malloc(sizeof(AnSISOP_funciones));

	funciones->AnSISOP_asignar = asignar;
	funciones->AnSISOP_asignarValorCompartida = asignarValorCompartida;
	funciones->AnSISOP_definirVariable = definirVariable;
	funciones->AnSISOP_dereferenciar = dereferenciar;
	funciones->AnSISOP_finalizar = finalizar;
	funciones->AnSISOP_irAlLabel = irAlLabel;
	funciones->AnSISOP_llamarConRetorno = llamarConRetorno;
	funciones->AnSISOP_llamarSinRetorno = llamarSinRetorno;
	funciones->AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable;
	funciones->AnSISOP_obtenerValorCompartida = obtenerValorCompartida;
	funciones->AnSISOP_retornar = retornar;
	funcionesKernel->AnSISOP_abrir = abrir;
	funcionesKernel->AnSISOP_borrar = borrar;
	funcionesKernel->AnSISOP_cerrar = cerrar;
	funcionesKernel->AnSISOP_escribir = escribir;
	funcionesKernel->AnSISOP_leer = leer;
	funcionesKernel->AnSISOP_liberar = liberar;
	funcionesKernel->AnSISOP_moverCursor = moverCursor;
	funcionesKernel->AnSISOP_reservar = reservar;
	funcionesKernel->AnSISOP_signal = signal;
	funcionesKernel->AnSISOP_wait = wait;
}

bool esArgumento(t_nombre_variable identificador_variable){
	if(isdigit(identificador_variable)){
		return true;
	}else{
		return false;
	}
}
