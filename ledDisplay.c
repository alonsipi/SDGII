
#include "ledDisplay.h"

tipo_pantalla pantalla_inicial = {
		.matriz = {
				{0,0,0,0,0,0,0,0},
				{0,1,1,0,0,1,1,0},
				{0,1,1,0,0,1,1,0},
				{0,0,0,0,0,0,0,0},
				{0,1,0,0,0,0,1,0},
				{0,0,1,1,1,1,0,0},
				{0,0,0,0,0,0,0,0},
		}
};

tipo_pantalla pantalla_final = {
		.matriz = {
				{0,0,0,0,0,0,0,0},
				{0,1,1,0,0,1,1,0},
				{0,1,1,0,0,1,1,0},
				{0,0,0,0,0,0,0,0},
				{0,0,1,1,1,1,0,0},
				{0,1,0,0,0,0,1,0},
				{0,0,0,0,0,0,0,0},
		}
};

// Maquina de estados: lista de transiciones
// {EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
fsm_trans_t fsm_trans_excitacion_display[] = {
		{ DISPLAY_ESPERA_COLUMNA, CompruebaTimeoutColumnaDisplay, DISPLAY_ESPERA_COLUMNA, ActualizaExcitacionDisplay },
		{-1, NULL, -1, NULL },
};

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

void InicializaLedDisplay (TipoLedDisplay *led_display) {
	// A completar por el alumno...
	// ...



	led_display->flags=0;
	led_display->p_columna=0;

	int i;
	for(i=0; i<NUM_FILAS_DISPLAY; i++){
		pinMode(led_display->filas[i], OUTPUT);
		digitalWrite(led_display->filas[i], HIGH);
	}
	int j;
	for(j=0; j<NUM_COLUMNAS_DISPLAY; j++){
		pinMode(led_display->pines_control_columnas[j], OUTPUT);
		digitalWrite(led_display->pines_control_columnas[j], LOW);
	}
	led_display->tmr_refresco_display = tmr_new(timer_refresco_display_isr);
	tmr_startms((tmr_t*)(led_display->tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);

}





//------------------------------------------------------
// OTROS PROCEDIMIENTOS PROPIOS DE LA LIBRERIA
//------------------------------------------------------

void ApagaFilas (TipoLedDisplay *led_display){
	// A completar por el alumno...
	// ...
	int i;
	for(i=0; i<NUM_FILAS_DISPLAY; i++){
		digitalWrite(led_display->filas[i], HIGH);
	}

}

void ExcitaColumnas(int columna) {

	switch(columna) {
	// A completar por el alumno...
	// ...
	case 0:
		digitalWrite(led_display.pines_control_columnas[2], LOW);
		digitalWrite(led_display.pines_control_columnas[1], LOW);
		digitalWrite(led_display.pines_control_columnas[0], LOW);
		break;

	case 1:
		digitalWrite(led_display.pines_control_columnas[2], LOW);
		digitalWrite(led_display.pines_control_columnas[1], LOW);
		digitalWrite(led_display.pines_control_columnas[0], HIGH);
		break;

	case 2:
		digitalWrite(led_display.pines_control_columnas[2], LOW);
		digitalWrite(led_display.pines_control_columnas[1], HIGH);
		digitalWrite(led_display.pines_control_columnas[0], LOW);
		break;

	case 3:
		digitalWrite(led_display.pines_control_columnas[2], LOW);
		digitalWrite(led_display.pines_control_columnas[1], HIGH);
		digitalWrite(led_display.pines_control_columnas[0], HIGH);
		break;
	case 4:
		digitalWrite(led_display.pines_control_columnas[2], HIGH);
		digitalWrite(led_display.pines_control_columnas[1], LOW);
		digitalWrite(led_display.pines_control_columnas[0], LOW);
		break;
	case 5:
		digitalWrite(led_display.pines_control_columnas[2], HIGH);
		digitalWrite(led_display.pines_control_columnas[1], LOW);
		digitalWrite(led_display.pines_control_columnas[0], HIGH);
		break;

	case 6:
		digitalWrite(led_display.pines_control_columnas[2], HIGH);
		digitalWrite(led_display.pines_control_columnas[1], HIGH);
		digitalWrite(led_display.pines_control_columnas[0], LOW);
		break;
	case 7:
		digitalWrite(led_display.pines_control_columnas[2], HIGH);
		digitalWrite(led_display.pines_control_columnas[1], HIGH);
		digitalWrite(led_display.pines_control_columnas[0], HIGH);
		break;
	}
}

void ActualizaLedDisplay (TipoLedDisplay *led_display) {
	// A completar por el alumno...
	// ...


	ApagaFilas(led_display);

	led_display->p_columna++;

	if(led_display->p_columna > NUM_COLUMNAS_DISPLAY){
		led_display->p_columna = 0;
	}

	ExcitaColumnas(led_display->p_columna);

	int i;

	for(i=0; i<NUM_FILAS_DISPLAY; i++){
		if((led_display->pantalla.matriz[i][led_display->p_columna] == 1)){
			digitalWrite(led_display->filas[i], LOW);
		}
	}


}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaTimeoutColumnaDisplay (fsm_t* this) {
	int result = 0;
	TipoLedDisplay *p_ledDisplay;
	p_ledDisplay = (TipoLedDisplay*)(this->user_data);

	// A completar por el alumno...
	// ...
	piLock(MATRIX_KEY);
	result = p_ledDisplay->flags & FLAG_TIMEOUT_COLUMNA_DISPLAY;
	piUnlock(MATRIX_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

void ActualizaExcitacionDisplay (fsm_t* this) {
	TipoLedDisplay *p_ledDisplay;
	p_ledDisplay = (TipoLedDisplay*)(this->user_data);

	// A completar por el alumno...
	// ...
	piLock(MATRIX_KEY);
	led_display.flags &= (~FLAG_TIMEOUT_COLUMNA_DISPLAY);

	piUnlock(MATRIX_KEY);

	ActualizaLedDisplay(p_ledDisplay);
	tmr_startms((tmr_t*)(led_display.tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);

}

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void timer_refresco_display_isr (union sigval value) {
	// A completar por el alumno...
	// ...
	piLock(MATRIX_KEY);
	led_display.flags |= FLAG_TIMEOUT_COLUMNA_DISPLAY;
	piUnlock (MATRIX_KEY);
}
