#include "mmu.h"

int traducir_direccion_logica(int direccion_logica) {

	uint32_t num_pagina = direccion_logica / tam_pagina;

	uint32_t desplazamiento_pagina = direccion_logica % tam_pagina;
	int marco = -1;

	if(CANTIDAD_ENTRADAS_TLB)
		marco = buscarMarcoEnTLB(PCB_Actual->id, num_pagina);

	if(marco == -1)
	{
		t_list* listaInts = list_create();

		list_add(listaInts, &PCB_Actual->id);
		list_add(listaInts, &num_pagina);


		enviarListaUint32_t(listaInts,memoria_fd, info_logger, SOLICITUDMARCO);

		op_code_cliente cod = recibir_operacion(memoria_fd);

		if(cod == SOLICITUDMARCO){        
			marco = recibirValor_uint32(memoria_fd);
			log_info(info_logger, "Marco: %d", marco);
			agregarEntradaTLB(PCB_Actual->id, num_pagina, marco);

		}
		else{
			printf("%d", cod);
			log_error(error_logger,"FALLO en la obtencion del marco");
			marco=-1; 
		}
		
		list_clean(listaInts);
		list_destroy(listaInts);

	}
	
	int direccion_fisica = marco * tam_pagina + desplazamiento_pagina;

	log_info(info_logger, "PID: <%d> - OBTENER MARCO - Página: <%d> - Marco: <%d>\n", PCB_Actual->id, num_pagina, marco);

	log_info(info_logger, "direccion fisica = marco * tam_pagina + desplazamiento: %d", direccion_fisica);

	return direccion_fisica;
}