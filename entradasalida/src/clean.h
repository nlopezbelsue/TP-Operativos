#ifndef CLEAN_H_
#define CLEAN_H_


#include <logs.h>

extern t_log* trace_logger;
extern t_log* info_logger;
extern t_log* error_logger;
extern t_log* debug_logger;
extern t_config* configuracionEntradasalida;
extern t_config_entradaSalida* cfg_entradaSalida;
extern t_config* configFs;
extern char* nombreInterfaz;
extern bool logsCreados;

void cerrarPrograma();
void destruirLoggers();

#endif //CLEAN_H_