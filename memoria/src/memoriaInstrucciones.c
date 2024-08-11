#include "memoriaInstrucciones.h"


void* espacio_contiguo;
//uint32_t *marcosAsignados;
char* charAux[50];

bool memoriaInicializada;
bool tablaPaginasCreada;
bool pagina0Creada;
bool semaforosCreados;

t_list* tablasPaginas;

pthread_mutex_t mutex_espacioContiguo;
pthread_mutex_t mutex_tablasPaginas;
pthread_mutex_t mutex_idPagina;
pthread_mutex_t mutex_espacioDisponible;

//t_log* trace_logger;
t_log* debug_logger;
//t_log* warning_logger;
t_log* error_logger;
Pagina* pagina0;
t_list* tablasPaginas;
Pagina *paginaParaAnalizar;

int TAM_MEMORIA;
int TAM_PAGINA;
int CANT_MARCOS;

bool inicializarBitmap() {
    TAM_PAGINA = config_get_int_value(config, "TAM_PAGINA");
    TAM_MEMORIA = config_get_int_value(config, "TAM_MEMORIA");
    CANT_MARCOS = TAM_MEMORIA / TAM_PAGINA;

    marcosAsignados = (uint32_t*)malloc(CANT_MARCOS * sizeof(uint32_t));

    if (marcosAsignados == NULL)
        return false;

    for (int i = 0; i < CANT_MARCOS; i++) {
        marcosAsignados[i] = 0;
    }
    return true;
}

void liberarMemory() {
    free(marcosAsignados);
    marcosAsignados = NULL;
}

void liberarPagina(Pagina* pagina) {
    free(pagina); //AVISAR A FS
}

bool crearSemaforos(){
    int comprobacionEspacioContiguo = pthread_mutex_init(&mutex_espacioContiguo,NULL);
    int comprobacionEspacioDisponible = pthread_mutex_init(&mutex_espacioDisponible,NULL);
    int comprobacionIdPagina = pthread_mutex_init(&mutex_idPagina,NULL);
    int comprobacionTablasPaginas = pthread_mutex_init(&mutex_tablasPaginas,NULL);
    int comprobacionFSMutex = pthread_mutex_init(&mutexFS, NULL);


    bool noEsIgualACero(int numero){
        return numero != 0;
    }
    if(noEsIgualACero(comprobacionFSMutex)){
    	log_error(error_logger, "No se pudieron inicializar los semaforos");
    	return false;
    }
    if(noEsIgualACero(comprobacionEspacioContiguo) || noEsIgualACero(comprobacionEspacioDisponible)){
        log_error(error_logger, "No se pudieron inicializar los semaforos");
        return false;
    }
    if(noEsIgualACero(comprobacionIdPagina) || noEsIgualACero(comprobacionTablasPaginas)){
        log_error(error_logger, "No se pudieron inicializar los semaforos");
        return false;
    }
    semaforosCreados = true;
    return true;
}

bool crearEspacioContiguoDeMemoria(){

    espacio_contiguo = malloc(TAM_MEMORIA);
    memset(espacio_contiguo,0,TAM_MEMORIA);
    memoriaInicializada = true;
    return true;
}

//REVISAR
void liberarTablaDePaginas(uint32_t pid){
    TablaDePaginas* tabla = obtenerTablaPorPID(pid);
    TablaDePaginas* tabla2;
    for (int i=0 ; i<list_size(tablaGeneral); i++){
        tabla2 = list_get(tablaGeneral,i);
        if (tabla->pid == tabla2->pid){
            pthread_mutex_lock(&mutex_tablasPaginas);
            void* element = list_remove(tablaGeneral, i);
            pthread_mutex_unlock(&mutex_tablasPaginas);
            list_clean_and_destroy_elements(tabla->paginas, free);
            free(tabla);
            log_trace(trace_logger, "Tabla con PID <%d> eliminada de tablaGeneral", pid);
            return;
        }
    }
    log_trace(trace_logger, "No se encontró una tabla con PID <%d> en tablaGeneral", pid);
}


bool crearEstructurasAdministrativas(){
    bool comp1 = crearSemaforos();
    bool comp2 = crearEspacioContiguoDeMemoria();
    bool comp3 = inicializarBitmap();
    instruccionesDeProcesos = list_create();
    return comp1 && comp2 && comp3;
}

bool iniciarMemoria(){
    bool estructurasAdministrativas = crearEstructurasAdministrativas();
    return estructurasAdministrativas;
}

// Guarda en una lista de procesos pid y lista de instrucciones
void GuardarInstrucsDeProceso(uint32_t pid, char* file_name){
    Proceso* newProceso = malloc(sizeof(Proceso));
    newProceso->pid = pid;

    t_list* listaInstrucciones = list_create();
    char* path = string_new();
    string_append(&path, PATH_INSTRUCCIONES);
	string_append(&path, file_name);

    FILE* archivoPseudocodigo;
	archivoPseudocodigo = fopen(path, "rb");
	if(archivoPseudocodigo == NULL){
        log_error(error_logger, "No se pudo abrir el archivo");
        exit(EXIT_FAILURE);
    }
    char linea[60];
    while (!feof(archivoPseudocodigo)) {
        fgets(linea, sizeof(linea), archivoPseudocodigo);
        char** lineaLeida = string_split(linea, "\n");
        list_add(listaInstrucciones, FormatearInstruccion(lineaLeida[0]));
        //list_add(listaInstrucciones, FormatearInstruccion(linea));

    }
    log_trace(trace_logger, "Guardo %d instrucciones del proceso %d", list_size(listaInstrucciones), pid);

    fclose(archivoPseudocodigo);
    newProceso->instrucciones = listaInstrucciones;
    
    list_add(instruccionesDeProcesos, newProceso);
    
}

//Pasa una linea de pseudocodigo a nuestro formato Instruccion
Instruccion* FormatearInstruccion(char* linea){ 
    Instruccion * instruccion = malloc(sizeof(Instruccion));
    char** partesInstruccion = string_split(linea, " ");
    instruccion->idLength = string_length(partesInstruccion[0]);
    instruccion->id = partesInstruccion[0];
    int cantParametros = string_array_size(partesInstruccion)-1;
    instruccion->cantidadParametros = cantParametros;
    
    while (cantParametros != 0){
        switch (cantParametros){
        case 5: 
            instruccion->param5Length = strlen(partesInstruccion[5]);
            instruccion->param5 = partesInstruccion[5];
            instruccion->param4Length = strlen(partesInstruccion[4]);
            instruccion->param4 = partesInstruccion[4];
            break;
        case 3:
            instruccion->param3Length = strlen(partesInstruccion[3]);
            instruccion->param3 = partesInstruccion[3];
            break;
        case 2:
            instruccion->param2Length = strlen(partesInstruccion[2]);
            instruccion->param2 = partesInstruccion[2];
            break;
        case 1:
            instruccion->param1Length = strlen(partesInstruccion[1]);
            instruccion->param1 = partesInstruccion[1];
            break;
        }
        cantParametros--;
    }
	return instruccion;
}

Instruccion* retornarInstruccionACPU(uint32_t pid,uint32_t pc){
	bool coincidePid(void* proceso){
		return ((Proceso*)proceso)->pid == pid;
	}
    
	Proceso* proceso = list_find(instruccionesDeProcesos, coincidePid);
	Instruccion* instruccion = list_get(proceso->instrucciones, pc);
	return instruccion;
}