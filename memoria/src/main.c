#include <stdlib.h>
#include <stdio.h>
#include <utils/estructurasCompartidas.h>
#include "main.h"
#include <signal.h>

t_log* info_logger;
t_log* trace_logger;
t_log* warning_logger;
char *PUERTO;
t_config *config;
t_list* tablaGeneral;
char* PATH_INSTRUCCIONES;
t_list *instruccionesDeProcesos;

void pruebaLectoEscrituraCOmbinada();


int main(int argc, char* argv[]) {
    //decir_hola("Memoria");

	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Error al configurar el manejador de señal");
        return EXIT_FAILURE;
    }
    
    info_logger = log_create("info_logger.log","Memory", true, LOG_LEVEL_INFO);
	trace_logger = log_create("trace_logger.log", "Memory", true, LOG_LEVEL_TRACE);
	warning_logger = log_create("warning_logger.log", "Memory", true, LOG_LEVEL_WARNING);
	
    pthread_t tid;
    
    config = crearConfig(argv[1]);
	PATH_INSTRUCCIONES = config_get_string_value(config, "PATH_INSTRUCCIONES");
	RETARDO_RESPUESTA = config_get_int_value(config, "RETARDO_RESPUESTA");
	PUERTO = config_get_string_value(config, "PUERTO_ESCUCHA");

	tablaGeneral = list_create();
	instruccionesDeProcesos = list_create();
	
	iniciarMemoria();
    //bool comp2 = crearEspacioContiguoDeMemoria();

	//pruebaLectoEscrituraCOmbinada();
	
    pthread_create(&tid, NULL, recibir, NULL);
	pthread_join(tid, NULL);

	config_destroy(config);

    return 0;
}

void pruebaLectoEscrituraCOmbinada(){
	void* prueba = (void*)readline(">");
	escribirMemoria(1, prueba, 3, 5);
	char* numeroC = readline(">");
	printf("\nnumero: %s\n", numeroC);
	uint8_t numero = atoi(numeroC);
	printf("\nnumero: %d\n", numero);
	void* numeroPuntero = &numero;
	escribirMemoria(0, numeroPuntero, 1, 5);
	
	void* result = leerMemoria(0, 4, 5);
	char* result2 = (char*)leerMemoria(0, 1, 5);

	printf("\nAber: %s\n", (char*)result);

	printf("\nAber: %d\n", *result2);

}



void *recibir(){
	recibirConexion(PUERTO);
	return NULL;
}

t_config *crearConfig(char* configPath){
	t_config* config;

	config = config_create(configPath);

	if(config != NULL)
		return config;

	printf("La configuracion no se pudo crear exitosamente");
	exit(2);

	return config;
}

void sigint_handler(int sig) {
    printf("\nSe ha recibido la señal SIGINT (Ctrl+C). Cerrando sockets...\n");
    // Aquí puedes cerrar tus sockets u realizar otras tareas necesarias
	close(cpu_fd);
	close(memoria_fd);
    exit(EXIT_SUCCESS); // Puedes modificar esto según sea necesario
}