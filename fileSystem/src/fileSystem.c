#include "funcionesFs.h"

int main(int argc, char** argv){

	logger = log_create(getenv("../logFS"),"FS", 1, 0);

	crearConfig(argc, argv);

	esperarConexionKernel();

	destruirConfiguracionFS(conf);
	return EXIT_SUCCESS;
}

void esperarConexionKernel(){
	socketEscucha = createServer(IP, conf->puertoEscucha, BACKLOG);
	if(socketEscucha != -1){
		log_info(logger,"Esperando conexion del kernel...");
	}else{
		log_error(logger,"Error al levantar el servidor");
	}

	socketConexionKernel = acceptSocket(socketEscucha);

	void* paquete;
	int tipo_mensaje;

	if(recibir_info(socketConexionKernel, &paquete, &tipo_mensaje)){
		log_info(logger, "Conexion con kernel establecida");
	}
}
