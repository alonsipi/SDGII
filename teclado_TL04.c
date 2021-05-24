#include "teclado_TL04.h"

char tecladoTL04[4][4] = {
		{'1', '2', '3', 'C'},
		{'4', '5', '6', 'D'},
		{'7', '8', '9', 'E'},
		{'A', '0', 'B', 'F'}
};

// Maquina de estados: lista de transiciones
// {EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
fsm_trans_t fsm_trans_excitacion_columnas[] = {
		{ TECLADO_ESPERA_COLUMNA, CompruebaTimeoutColumna, TECLADO_ESPERA_COLUMNA, TecladoExcitaColumna },
		{-1, NULL, -1, NULL },
};

fsm_trans_t fsm_trans_deteccion_pulsaciones[] = {
		{ TECLADO_ESPERA_TECLA, CompruebaTeclaPulsada, TECLADO_ESPERA_TECLA, ProcesaTeclaPulsada},
		{-1, NULL, -1, NULL },
};

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

void InicializaTeclado(TipoTeclado *p_teclado) {
	// A completar por el alumno...
	// ...

	int i=0;

	p_teclado->columna_actual=COLUMNA_1;
	p_teclado->teclaPulsada.col=-1;
	p_teclado->teclaPulsada.row=-1;

	p_teclado->flags=0;
	p_teclado->debounceTime[FILA_1]=0;
	p_teclado->debounceTime[FILA_2]=0;
	p_teclado->debounceTime[FILA_3]=0;
	p_teclado->debounceTime[FILA_4]=0;

	for(i=0; i<4; i++){
		pinMode(p_teclado->filas[i], INPUT);
		pullUpDnControl(p_teclado->filas[i], PUD_DOWN);
		wiringPiISR(p_teclado->filas[i], INT_EDGE_RISING, p_teclado->rutinas_ISR[i]);
	}
	int j=0;

	for(j=0; i<4; j++){
		pinMode (p_teclado->columnas[j], OUTPUT);
		digitalWrite(p_teclado->columnas[j], LOW);
	}
	p_teclado->tmr_duracion_columna=tmr_new(timer_duracion_columna_isr);
	tmr_startms((tmr_t*)(p_teclado->tmr_duracion_columna),TIMEOUT_COLUMNA_TECLADO);

	//instrucciones sobre la jugabilidad y funciones del juego
	printf("\n Juego cargado correctamente. \n");
	printf("\n Para continuar pulsa una de las siguientes teclas:");
	printf("\n '1' para empezar la partida.");
	printf("\n '0' para salir.");
	printf("\n Tenga en cuenta las siguientes teclas para jugar:");
	printf("\n 'A' para mover a la izquierda la tabla.");
	printf("\n 'D' para mover a la izquierda la tabla.");
	printf("\n '7' para pausar la partida.");
	printf("\n '5' para reanudar la partida.");

	printf("\n Mejoras:\n");
	printf(" 1. - Al acabar te dice cuantos ladrillos te han quedado por destrozar\n");
	printf(" 2. - El juego va más rápido a medida que rompes ladrillos (la dificultad aumenta)\n");
	printf(" 3. - Se puede pausar y reanudar el juego.\n");

	fflush(stdout);
}

//------------------------------------------------------
// OTROS PROCEDIMIENTOS PROPIOS DE LA LIBRERIA
//------------------------------------------------------

void ActualizaExcitacionTecladoGPIO (int columna) {
	// A completar por el alumno
	// ...
	switch(columna){
	// ...
	case COLUMNA_1:
		digitalWrite(GPIO_KEYBOARD_COL_1, HIGH);
		digitalWrite(GPIO_KEYBOARD_COL_2, LOW);
		digitalWrite(GPIO_KEYBOARD_COL_3, LOW);
		digitalWrite(GPIO_KEYBOARD_COL_4, LOW);
		break;
	case COLUMNA_2:
		digitalWrite(GPIO_KEYBOARD_COL_1, LOW);
		digitalWrite(GPIO_KEYBOARD_COL_2, HIGH);
		digitalWrite(GPIO_KEYBOARD_COL_3, LOW);
		digitalWrite(GPIO_KEYBOARD_COL_4, LOW);
		break;
	case COLUMNA_3:
		digitalWrite(GPIO_KEYBOARD_COL_1, LOW);
		digitalWrite(GPIO_KEYBOARD_COL_2, LOW);
		digitalWrite(GPIO_KEYBOARD_COL_3, HIGH);
		digitalWrite(GPIO_KEYBOARD_COL_4, LOW);
		break;
	case COLUMNA_4:
		digitalWrite(GPIO_KEYBOARD_COL_1, LOW);
		digitalWrite(GPIO_KEYBOARD_COL_2, LOW);
		digitalWrite(GPIO_KEYBOARD_COL_3, LOW);
		digitalWrite(GPIO_KEYBOARD_COL_4, HIGH);
		break;
	}
	//la siguiente linea deberia estar comentada
	//tmr_startms((tmr_t*)(teclado.tmr_duracion_columna),TIMEOUT_COLUMNA_TECLADO);
}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaTimeoutColumna (fsm_t* this) {
	int result = 0;
	TipoTeclado *p_teclado;
	p_teclado = (TipoTeclado*)(this->user_data);

	// A completar por el alumno...
	// ...
	piLock(KEYBOARD_KEY);
	result=p_teclado->flags & FLAG_TIMEOUT_COLUMNA_TECLADO;
	piUnlock(KEYBOARD_KEY);

	return result;
}

int CompruebaTeclaPulsada (fsm_t* this) {
	int result = 0;
	TipoTeclado *p_teclado;
	p_teclado = (TipoTeclado*)(this->user_data);

	// A completar por el alumno
	// ...
	piLock(KEYBOARD_KEY);
	result=p_teclado->flags & FLAG_TECLA_PULSADA;
	piUnlock(KEYBOARD_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LAS MAQUINAS DE ESTADOS
//------------------------------------------------------

void TecladoExcitaColumna (fsm_t* this) {
	TipoTeclado *p_teclado;
	p_teclado = (TipoTeclado*)(this->user_data);

	// A completar por el alumno
	// ...
	piLock(KEYBOARD_KEY);
	p_teclado->flags&=(~FLAG_TIMEOUT_COLUMNA_TECLADO);
	p_teclado->columna_actual++;

	if(p_teclado->columna_actual>=4){
		p_teclado->columna_actual=0;
	}

	// Llamada a ActualizaExcitacionTecladoGPIO con columna a activar como argumento
	ActualizaExcitacionTecladoGPIO(p_teclado->columna_actual);
	piUnlock(KEYBOARD_KEY);

	tmr_startms((tmr_t*)(teclado.tmr_duracion_columna), TIMEOUT_COLUMNA_TECLADO);
}

void ProcesaTeclaPulsada (fsm_t* this) {
	TipoTeclado *p_teclado;
	p_teclado = (TipoTeclado*)(this->user_data);

	// A completar por el alumno
	// ...
	piLock (KEYBOARD_KEY);
	p_teclado->flags &=(~FLAG_TECLA_PULSADA);

	switch(p_teclado->teclaPulsada.col){

	case COLUMNA_1:

		if (p_teclado->teclaPulsada.row == FILA_1) {
			//CASO TECLA 1 para iniciar el juego y tambien para volver a jugar en caso de que perdamos
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			p_teclado->teclaPulsada.row = -1;
			p_teclado->teclaPulsada.col = -1;

		}else if(p_teclado->teclaPulsada.row == FILA_4){
			//CASO TECLA A para mover a la izquierda
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_MOV_IZQUIERDA;
			piUnlock(SYSTEM_FLAGS_KEY);
			p_teclado->teclaPulsada.row = -1;
			p_teclado->teclaPulsada.col = -1;

		}else if(p_teclado->teclaPulsada.row == FILA_3){
			//CASO TECLA 7 para pausar el juego en el momento en el que se desee
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_PAUSA_JUEGO;
			piUnlock(SYSTEM_FLAGS_KEY);
			p_teclado->teclaPulsada.row = -1;
			p_teclado->teclaPulsada.col = -1;

		}

		break;

	case COLUMNA_2:
		//CASO TECLA 0 para salir del juego
		if(p_teclado->teclaPulsada.row == FILA_4){

			exit(0);

			p_teclado->teclaPulsada.row = -1;
			p_teclado->teclaPulsada.col = -1;

		}else if(p_teclado->teclaPulsada.row == FILA_2){
			//CASO TECLA 5 para reanudar el juego en caso de que se haya pulsado previamente el 7 que implica el estado de pausa
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			p_teclado->teclaPulsada.row = -1;
			p_teclado->teclaPulsada.col = -1;

		}

		break;

	case COLUMNA_4:

		if(p_teclado->teclaPulsada.row == FILA_2){
			//CASO TECLA D para mover a la derecha
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_MOV_DERECHA;
			piUnlock(SYSTEM_FLAGS_KEY);
			p_teclado->teclaPulsada.row = -1;
			p_teclado->teclaPulsada.col = -1;

		}

		break;
	default:
		break;
	}



	piUnlock(KEYBOARD_KEY);
}


//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void teclado_fila_1_isr (void) {
	// A completar por el alumno
	// ...
	if(millis ()< teclado.debounceTime[FILA_1]){

		teclado.debounceTime[FILA_1] = millis ()+ DEBOUNCE_TIME ;
		return;
	}

	piLock (KEYBOARD_KEY);
	teclado.teclaPulsada.row = FILA_1;
	teclado.teclaPulsada.col = teclado.columna_actual;
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock (KEYBOARD_KEY);

	teclado.debounceTime[FILA_1]= millis ()+ DEBOUNCE_TIME ;
}

void teclado_fila_2_isr (void) {
	// A completar por el alumno
	// ...
	if(millis ()< teclado.debounceTime[FILA_2]){

		teclado.debounceTime[FILA_2] = millis ()+ DEBOUNCE_TIME ;
		return;
	}

	piLock (KEYBOARD_KEY);
	teclado.teclaPulsada.row = FILA_2;
	teclado.teclaPulsada.col = teclado.columna_actual;
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock (KEYBOARD_KEY);

	teclado.debounceTime[FILA_2]= millis ()+ DEBOUNCE_TIME ;
}

void teclado_fila_3_isr (void) {
	// A completar por el alumno
	// ...
	if(millis ()< teclado.debounceTime[FILA_3]){

		teclado.debounceTime[FILA_3] = millis ()+ DEBOUNCE_TIME ;
		return;
	}

	piLock (KEYBOARD_KEY);
	teclado.teclaPulsada.row = FILA_3;
	teclado.teclaPulsada.col = teclado.columna_actual;
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock (KEYBOARD_KEY);

	teclado.debounceTime[FILA_3]= millis ()+ DEBOUNCE_TIME ;
}

void teclado_fila_4_isr (void) {
	// A completar por el alumno
	// ...
	if(millis ()< teclado.debounceTime[FILA_4]){

		teclado.debounceTime[FILA_4] = millis ()+ DEBOUNCE_TIME ;
		return;
	}

	piLock (KEYBOARD_KEY);
	teclado.teclaPulsada.row = FILA_4;
	teclado.teclaPulsada.col = teclado.columna_actual;
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock (KEYBOARD_KEY);

	teclado.debounceTime[FILA_4]= millis ()+ DEBOUNCE_TIME ;
}

void timer_duracion_columna_isr (union sigval value) {
	// A completar por el alumno
	// ...
	piLock (KEYBOARD_KEY);
	teclado.flags |= FLAG_TIMEOUT_COLUMNA_TECLADO;
	piUnlock (KEYBOARD_KEY);
}
