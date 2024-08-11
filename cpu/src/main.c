#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include <signal.h>

t_log* info_logger;
t_log* error_logger;
t_log* warning_logger;
char* PUERTO_DISPATCH;
char* PUERTO_INTERRUPT;
char* ALGORITMO_TLB;
int CANTIDAD_ENTRADAS_TLB;
t_list* TLB;
t_config *config;
sem_t bin_ciclo;



int main(int argc, char* argv[]) {
    //decir_hola("CPU");
    
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Error al configurar el manejador de señal");
        return EXIT_FAILURE;
    }

    config = crearConfig(argv[1]);
    info_logger = log_create("info_logger.log","Cpu", true, LOG_LEVEL_INFO);
	error_logger = log_create("error_logger.log","Cpu", true, LOG_LEVEL_ERROR);
	warning_logger = log_create("warning_logger.log", "Memory", true, LOG_LEVEL_WARNING);

	PUERTO_DISPATCH = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
	PUERTO_INTERRUPT = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
	
	ALGORITMO_TLB = config_get_string_value(config, "ALGORITMO_TLB");
	//CANTIDAD_ENTRADAS_TLB = atoi(config_get_string_value(config, "CANTIDAD_ENTRADAS_TLB"));
	CANTIDAD_ENTRADAS_TLB = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");

	TLB = list_create();

	sem_init(&bin_ciclo,0,1);

    pthread_t tid[3];
	pthread_create(&tid[1], NULL, conectarMemoria, NULL);
    pthread_join(tid[1], NULL);

    pthread_create(&tid[0], NULL, recibir, NULL);
	pthread_create(&tid[2], NULL, recibirInterrupt, NULL);

	pthread_join(tid[0], NULL);
	pthread_join(tid[2], NULL);


    return 0;
}

void *recibir(){
	recibirConexion(PUERTO_DISPATCH);
	return NULL;
}

void *recibirInterrupt(){
	recibirConexionInterrupt(PUERTO_INTERRUPT);
	return NULL;
}

void *conectarMemoria(){
	conectarModulo("MEMORIA");
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
	close(memoria_fd);
	close(kernel_fd);
	close(kernel_interrupt_fd);
    exit(EXIT_SUCCESS); // Puedes modificar esto según sea necesario
}