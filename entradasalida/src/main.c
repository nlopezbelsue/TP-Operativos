#include <main.h>

t_config* config;

int main(int argc, char* argv[]) {

	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Error al configurar el manejador de señal");
        return EXIT_FAILURE;
    }
  
    config = crearConfig(argv[1]);
    init_loggers_config(argv[1]);
    cargar_configuracion();
	if(!strcmp(cfg_entradaSalida->TIPO_INTERFAZ, "DIALFS")){
		crearEstructurasFs();
	}

	// for(int i=0; i<cfg_entradaSalida->BLOCK_COUNT; i++){
	// 	log_info(info_logger, "bit %d en pos %d", bitarray_test_bit(bitmap, i), i);
	// }

	
	// crearArchivo("salida.txt");
	// crearArchivo("archivo1.txt");
	// crearArchivo("cronologico.txt");
	// crearArchivo("archivo2.txt");

	// truncarArchivo("salida.txt", 80);
	// truncarArchivo("archivo1.txt", 10);
	// truncarArchivo("cronologico.txt", 80);
	// truncarArchivo("archivo2.txt", 10);

	// escribirArchivo("salida.txt", "Fallout 1 Fallout 2 Fallout 3 Fallout: New Vegas Fallout 4 Fallout 76", 0, 69);

	// void* datos = leerArchivo("salida.txt", 0, 80);

	// log_info(info_logger, "Datos leidos de salida %s", datos);

	// escribirArchivo("cronologico.txt", "Fallout 76 Fallout 1 Fallout 2 Fallout 3 Fallout: New Vegas Fallout 4", 0, 69);

	// void* datos2 = leerArchivo("cronologico.txt", 0, 80);

	// log_info(info_logger, "datos leidos de crono %s", datos2);

	// crearArchivo("pesado.txt");
	// truncarArchivo("pesado.txt", 250);
	
	// eliminarArchivo("archivo1.txt");

	// truncarArchivo("archivo2.txt", 70);

	//crearArchivo("duhndaud.txt");
	//eliminarArchivo("duhndaud.txt");
	//manejarInterfazStdin(2, 30);
	
/*
	//listaDeArchivos = list_create();
	crearArchivo("pruebaLoca.txt");
	//list_add(listaDeArchivos, "pruebaLoca.txt");
	truncarArchivo("pruebaLoca.txt", 64); 
	escribirArchivo("hola ", "HOLA COMO ESTAS HOLA COMO ESTAS HOLA COMO ESTAS HOLA COMO ESTAS", 0, 64);
	crearArchivo("pruebaLoca2.txt");
	//list_add(listaDeArchivos, "pruebaLoca2.txt");

	//msync(bitmap, cfg_entradaSalida->BLOCK_COUNT, MS_SYNC);
	truncarArchivo("pruebaLoca2.txt", 128);
	truncarArchivo("pruebaLoca.txt", 128);
	//truncarArchivo("pruebaLoca.txt", 0);
	// for(int i=0; i<bitarray_get_max_bit(bitmap); i++){
    //    log_info(info_logger, "%d", bitarray_test_bit(bitmap, i));
    // }
*/
	
	pthread_t kernel;
	pthread_t memoria;

    pthread_create(&kernel, NULL, iniciarKernel, NULL);

	if(cfg_entradaSalida->TIPO_INTERFAZ_INT == 1 || cfg_entradaSalida->TIPO_INTERFAZ_INT == 2 || cfg_entradaSalida->TIPO_INTERFAZ_INT == 0){ 
		pthread_create(&memoria, NULL, iniciarMemoria, NULL);
		pthread_join(memoria, NULL);
	}

	pthread_join(kernel, NULL);

	// cerrarPrograma();
    return 0;
	
}

t_config *crearConfig(char* configPath){		//Nombre interfaz
	
	nombreInterfaz = string_new();

	string_append(&nombreInterfaz, configPath);

	char* path = string_new();

	string_append(&path, "/home/utnso/tp-2024-1c-CANCH/configs/");
	string_append(&path, configPath);
	string_append(&path, ".config");

	printf("Path: %s\n", path);
	t_config* config;

	config = config_create(path);

	if(config != NULL)
		return config;

	printf("La configuracion no se pudo crear exitosamente");
	exit(2);

	return config;
}

void sigint_handler(int sig) {
    printf("\nSe ha recibido la señal SIGINT (Ctrl+C). Cerrando sockets...\n");
    // Aquí puedes cerrar tus sockets u realizar otras tareas necesarias
    exit(EXIT_SUCCESS); // Puedes modificar esto según sea necesario
}