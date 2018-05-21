/*
 * functions.h
 *
 *  Created on: 18 de jan de 2018
 *      Author: bruno
 */

#ifndef USER_FUNCTIONS_H_
#define USER_FUNCTIONS_H_

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 1
#endif

#include "gatt_db.h"
#include "eeprom_emulation.h"


#define MICROCONTROLADOR	1
#define BLUETOOTH		2

#define MAJOR_DEFAULT		39164
#define MINOR_DEFAULT		1025

#define BASE_TEMPO		4

#define EEPROM_MINOR_ADDRESS	4000
#define EEPROM_MAJOR_ADDRESS	4003
#define EEPROM_PRESSED_ADDRESS	4006


#define TIME_TO_SLEEP			180*BASE_TEMPO		//120 = 60 segundos
#define TIME_TO_START_BEACON		6*BASE_TEMPO		//3 segundos
#define TIME_TO_START_BLUETOOTH		12*BASE_TEMPO		//3 segundos + 3 segundos = 6 segundos
#define TIME_TO_RETRY_CONNECTION	18*BASE_TEMPO		//9 segundos

/* Tempos antigos
#define TIME_TO_SLEEP			180*BASE_TEMPO		//120 = 60 segundos
#define TIME_TO_START_BEACON		12*BASE_TEMPO		//6 segundos
#define TIME_TO_START_BLUETOOTH		20*BASE_TEMPO		//4 segundos + 6 segundos = 10 segundos
#define TIME_TO_RETRY_CONNECTION	30*BASE_TEMPO		//15 segundos
*/
#define TIMEOUT_DESCONECTA_BLUETOOTH	4*BASE_TEMPO
#define TIMEOUT_DORME_BLUETOOTH		1*BASE_TEMPO


#define PANIC_TIME			4*BASE_TEMPO		//2 segundos
#define CHECKIN_TIME			8*BASE_TEMPO		//4 segundos
#define RING_TIME			14*BASE_TEMPO		//7 segundos
#define PAINRING_TIME			20*BASE_TEMPO		//10 segundos

#define COM_DEBUG_SERIAL


enum maquina_estados
{
  SLEEP_MODE 		= 1,
  BEACON_MODE 		= 2,
  PANIC_MODE 		= 4,
  PAIRING_MODE 		= 8,
  CONNECTED 		= 16,
  PRESSED_2s 		= 32,
  PRESSED_4s 		= 64,
  PRESSED_7s		= 128,
  PRESSED_15s 		= 256,
  PRESSED_MORE_30s	= 512,
  PRESSED_LESS_2s	= 1024,
  CHECKIN_MODE 		= 2048,
  RING_MODE 		= 4096,
  LOW_BAT_ADVERTISE 	= 8192,
};


typedef struct
{
	uint32 num_pack_sent;
	uint32 num_bytes_sent;
	uint32 num_pack_received;
	uint32 num_bytes_received;
	uint32 num_writes; /* total number of send attempts */
} tsCounters;




extern unsigned long int cont_tmr_button_pressed;
extern unsigned char start_cont_button_press;
extern unsigned int status_botao;
unsigned long int timeout_to_sleep;
extern uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];
extern uint8_t boot_to_dfu;
extern uint8 _conn_handle;
extern unsigned long int time_line;
extern unsigned short int cont_tmr_led;
extern unsigned short int timeout_to_connect;
EE_Variable_TypeDef eeprom_minor, eeprom_major, eeprom_cont_pressed;
extern unsigned char flag_grava_eeprom;
extern unsigned short int timeout_to_start_beacon;
extern unsigned short int timeout_to_start_bluetooth;
extern unsigned short int timeout_to_start_wakeup;
extern unsigned char status_bluetooth;
extern unsigned char flag_vibra_motor;
extern unsigned short int cont_vibra_motor;
extern unsigned short int cont_pressed;
extern unsigned char flag_bateria_baixa;
extern uint8_t mac_address[20];
extern uint8_t buffer[50];
extern unsigned char conexoes_de_terceiros;



void verifica_eventos_bluetooth (void);
void reset_variables (void);
void configura_bluetooth_sleep (void);
void configura_bluetooth_acordado (void);
void acorda(unsigned char modulos);
void dorme(unsigned char modulos);
void desconecta_bluetooth (void);
void trata_string_bluetooth(void);
void registra_pressionamento(void);
void leitura_bateria(void);
void leitura_mac_address(void);

#endif /* USER_FUNCTIONS_H_ */
