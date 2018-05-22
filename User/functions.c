/*
 * functions.c
 *
 *  Created on: 18 de jan de 2018
 *      Author: bruno
 */

/* Board headers */
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

#include <stdio.h>
#include "sleep.h"
//#include "spp_utils.h"
#include "em_usart.h"
#include "infrastructure.h"
#include "em_cryotimer.h"
#include "em_emu.h"

#include "User/my_ibeacon.h"
#include "User/my_init.h"
#include "User/my_usart.h"
#include "functions.h"
#include "eeprom_emulation.h"

/***************************************************************************************************
 Local Macros and Definitions
 **************************************************************************************************/

#define STATE_ADVERTISING 1
#define STATE_CONNECTED   2
#define STATE_SPP_MODE    3

/* maximum number of iterations when polling UART RX data before sending data over BLE connection
 * set value to 0 to disable optimization -> minimum latency but may decrease throughput */
#define UART_POLL_TIMEOUT  5000

/***************************************************************************************************
 Local Variables
 **************************************************************************************************/
int _main_state;
tsCounters _sCounters;

static uint8 _max_packet_size = 30; /* maximum bytes per one packet */
static uint8 _min_packet_size = 30; /* target minimum bytes for one packet */

int i;

/***************************************************************************************************
 Global Variables
 **************************************************************************************************/
uint8 _conn_handle = 0xFF;
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];
uint8_t boot_to_dfu = 0;	// Flag for indicating DFU Reset must be performed

unsigned long int cont_tmr_button_pressed;
unsigned char start_cont_button_press;
unsigned int status_botao = SLEEP_MODE;
unsigned long int timeout_to_sleep;
unsigned long int time_line;
unsigned short int cont_tmr_led;
unsigned char received_string[30];
unsigned char my_uuid[16];
unsigned short int timeout_to_connect;
unsigned short int timeout_to_start_beacon;
unsigned short int timeout_to_start_bluetooth;
unsigned short int timeout_to_start_wakeup;
unsigned char flag_vibra_motor;
unsigned short int cont_vibra_motor;
unsigned short int cont_pressed;
unsigned char flag_bateria_baixa = 0;
unsigned char conexoes_de_terceiros;
unsigned char ciclos_tentativas;

unsigned char status_bluetooth;
uint8_t buffer [50];
uint8_t mac_address[20];

unsigned char FW_VERSION[7] = "1.0.2.1";


struct gecko_msg_system_get_bt_address_rsp_t *pResp;


uint16_t ps_save_object(uint16_t key, void *pValue, uint8_t size)
{
	struct gecko_msg_flash_ps_save_rsp_t *pResp1;

	pResp1 = gecko_cmd_flash_ps_save(key, size, pValue);

	return(pResp1->result);
}

uint16_t ps_load_object(uint16_t key, void *pValue, uint8_t size)
{
	struct gecko_msg_flash_ps_load_rsp_t *pResp1;

	pResp1 = gecko_cmd_flash_ps_load(key);

	if(pResp1->result == 0)
	{
		memcpy(pValue, pResp1->value.data, pResp1->value.len);

		// sanity check: length of data stored in PS key must match the expected value
		if(size != pResp1->value.len)
		{
			return(bg_err_unspecified);
		}
	}

	return(pResp1->result);
}


/************************************************************************/
/*            Função: verifica_eventos_bluetooth                        */
/* Descrição: Faz a tratativa dos eventos gerado pelo bluetooth         */
/* Entrada:                                                             */
/* Saída:                                                               */
/************************************************************************/
void verifica_eventos_bluetooth (void)
{
  /* Event pointer for handling events */
  struct gecko_cmd_packet* evt;

  if (_main_state == STATE_SPP_MODE)
    {
      /* if SPP data mode is active, use non-blocking gecko_peek_event() */
      evt = gecko_peek_event ();
      if (evt == NULL)
	{
	  //send_spp_data ();
	  //continue; /* jump directly to next iteration i.e. call gecko_peek_event() again */
	}
    }
  else
    {
      /* SPP data mode not active -> check for stack events using the blocking API */
      evt = gecko_wait_event ();
    }

  /* Handle events */
  switch (BGLIB_MSG_ID(evt->header))
    {

    /* This boot event is generated when the system boots up after reset.
     * Here the system is set to start advertising immediately after boot procedure. */
    case gecko_evt_system_boot_id:
      leitura_mac_address();
      reset_variables();

      // load the value of F from PS:

      ps_load_object(EEPROM_MINOR_ADDRESS, &my_minor, sizeof(my_minor));
      ps_load_object(EEPROM_MAJOR_ADDRESS, &my_major, sizeof(my_major));
      ps_load_object(EEPROM_PRESSED_ADDRESS, &cont_pressed, sizeof(cont_pressed));

      sprintf((char*)buffer,"READ_EEPROM_MINOR: %d \r\n",my_minor);
      USART_STR((uint8_t*)buffer);

      sprintf((char*)buffer,"READ_EEPROM_MAJOR: %d \r\n",my_major);
      USART_STR((uint8_t*)buffer);

      sprintf((char*)buffer,"READ_CONT_PRESSED: %d \r\n",cont_pressed);
      USART_STR((uint8_t*)buffer);

      //gecko_cmd_gatt_set_max_mtu (247);
      //gecko_cmd_le_gap_set_mode (le_gap_general_discoverable,le_gap_undirected_connectable);

      break;

      /* Connection opened event */
    case gecko_evt_le_connection_opened_id:

      _conn_handle = evt->data.evt_le_connection_opened.connection;

#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"Connectou Bluetooth \r\n");
#endif

      _main_state = STATE_CONNECTED;

      /* request connection parameter update.
       * conn.interval min 20ms, max 40ms, slave latency 4 intervals,
       * supervision timeout 2 seconds
       * (These should be compliant with Apple Bluetooth Accessory Design Guidelines, both R7 and R8) */
      gecko_cmd_le_connection_set_parameters (_conn_handle, 24, 40, 0, 200);
      timeout_to_connect = 0;					//Zera a contagem do timout para conexão após a conexão ser realizada...
      status_botao |= CONNECTED;

      break;

    case gecko_evt_le_connection_parameters_id:
      //printf ("Conn.parameters: interval %u units, txsize %u\r\n",
	      //evt->data.evt_le_connection_parameters.interval,
	      //evt->data.evt_le_connection_parameters.txsize);
      break;

    case gecko_evt_gatt_mtu_exchanged_id:
      /* calculate maximum data per one notification / write-without-response, this depends on the MTU.
       * up to ATT_MTU-3 bytes can be sent at once  */
      _max_packet_size = evt->data.evt_gatt_mtu_exchanged.mtu - 3;
      _min_packet_size = _max_packet_size; /* try to send maximum length packets whenever possible */
      //printf ("MTU exchanged: %d\r\n", evt->data.evt_gatt_mtu_exchanged.mtu);
      break;

    case gecko_evt_le_connection_closed_id:
      if(boot_to_dfu)
	{

#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"Desconectou Bluetooth e foi pro OTA \r\n");
#endif

	  boot_to_dfu = 0;
	  gecko_cmd_system_reset(2);
	}
      else
	{
	  if(!conexoes_de_terceiros)				//Se não houve conexão de terceiros...
	    {
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"Desconectou Bluetooth.. \r\n");
#endif

	  //printf ("DISCONNECTED \r\n");
	  reset_variables ();

	  status_botao &= 0xFFEF;	//Zera o flag de connected (vai píscar os led após a desconexão)
	  //printf("status atual: %d \r\n", status_botao);

	  dorme(BLUETOOTH);
	  timeout_to_sleep = 5*BASE_TEMPO;		//Dorme 2 segundos após ter se desconectado.

	  SLEEP_SleepBlockEnd (sleepEM2); // enable sleeping
	  /* restart advertising */
	  //gecko_cmd_le_gap_set_mode (le_gap_general_discoverable, le_gap_undirected_connectable);
	    }
	  else
	    {
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"Desconectou Bluetooth de terceiros.. \r\n");
#endif

	  //printf ("DISCONNECTED \r\n");
	  reset_variables ();
	  timeout_to_connect = 5;

	  status_botao &= 0xFFEF;	//Zera o flag de connected (vai píscar os led após a desconexão)
	  //printf("status atual: %d \r\n", status_botao);

	  //dorme(BLUETOOTH);
	  //timeout_to_sleep = 5*BASE_TEMPO;		//Dorme 2 segundos após ter se desconectado.

	  //SLEEP_SleepBlockEnd (sleepEM2); // enable sleeping
	  /* restart advertising */
	  //gecko_cmd_le_gap_set_mode (le_gap_general_discoverable, le_gap_undirected_connectable);
	    }
	}
      break;

    case gecko_evt_gatt_server_characteristic_status_id:
      {
	struct gecko_msg_gatt_server_characteristic_status_evt_t *pStatus;
	pStatus = &(evt->data.evt_gatt_server_characteristic_status);

	if (pStatus->characteristic == gattdb_gatt_ssp_data)
	  {
	    if (pStatus->status_flags == gatt_server_client_config)
	      {
		// Characteristic client configuration (CCC) for spp_data has been changed
		if (pStatus->client_config_flags == gatt_notification)
		  {
#ifdef COM_DEBUG_SERIAL
		    USART_STR((uint8_t*)"SPP mode ON\r\n");
#endif
		    _main_state = STATE_SPP_MODE;
		    //SLEEP_SleepBlockBegin (sleepEM2); // disable sleeping
		  }
		else
		  {
		    //USART_STR ("SPP mode OFF\r\n");
		    _main_state = STATE_CONNECTED;
		    SLEEP_SleepBlockEnd (sleepEM2); // enable sleeping
		  }

	      }
	  }
      }
      break;

    case gecko_evt_gatt_server_attribute_value_id:
      {
	for (i = 0; i < evt->data.evt_gatt_server_attribute_value.value.len; i++)
	  {
	    //USART_Tx(RETARGET_UART, evt->data.evt_gatt_server_attribute_value.value.data[i]);
	    received_string[i] = evt->data.evt_gatt_server_attribute_value.value.data[i];

	  }
	//printf("Recebido pelo bluetooth: %s \r\n",received_string);
	trata_string_bluetooth();

	//_sCounters.num_pack_received++;
	//_sCounters.num_bytes_received += evt->data.evt_gatt_server_attribute_value.value.len;
      }
      break;

      // Events related to OTA upgrading
      //Checks if the user-type OTA Control Characteristic was written.
      //If written, boots the device into Device Firmware Upgrade (DFU) mode.
      case gecko_evt_gatt_server_user_write_request_id:
        if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control)
          {
            if((status_botao & PAIRING_MODE) == PAIRING_MODE)
              {
                /* Set flag to enter to OTA mode */
                boot_to_dfu = 1;
                /* Send response to Write Request */
                gecko_cmd_gatt_server_send_user_write_response(evt->data.evt_gatt_server_user_write_request.connection,gattdb_ota_control,bg_err_success);
                /* Close connection to enter to DFU OTA mode */
                gecko_cmd_endpoint_close(evt->data.evt_gatt_server_user_write_request.connection);
              }
          }
      break;


    default:
      break;
    }
}
/************************************************************************/

/************************************************************************/
/*            Função: reset_variables		                        */
/* Descrição: Reseta variáveis após uma desconexão do bluetooth         */
/* Entrada:                                                             */
/* Saída:                                                               */
/************************************************************************/
void reset_variables (void)
{
  _conn_handle = 0xFF;
  _main_state = STATE_ADVERTISING;

  _max_packet_size = 20;

  memset (&_sCounters, 0, sizeof(_sCounters));
}
/************************************************************************/

/************************************************************************/
/*            Função: configura_bluetooth_sleep	                        */
/* Descrição: Configura e inicializa o stack do bluetooth, deixando-o   */
/* em modo de sleep quando ligado					*/
/* Entrada:                                                             */
/* Saída:                                                               */
/************************************************************************/
void configura_bluetooth_sleep (void)
{
  const gecko_configuration_t config =
    {
        .config_flags = 0,
        .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
        .bluetooth.max_connections = MAX_CONNECTIONS,
        .bluetooth.heap = bluetooth_stack_heap,
        .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
        .bluetooth.sleep_clock_accuracy = 100, // ppm
        .gattdb = &bg_gattdb_data,
        .ota.flags = 0,
        .ota.device_name_len = 4,
        .ota.device_name_ptr = "OTA",
  #if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
        .pa.config_enable = 1, // Enable high power PA
        .pa.input = GECKO_RADIO_PA_INPUT_VBAT,// Configure PA input to VBAT
  #endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
    };

  // Initialize stack
    gecko_init(&config);
}
/************************************************************************/


void configura_bluetooth_acordado(void)
{
  const gecko_configuration_t config =
    {
        .config_flags = 0,
        .sleep.flags = 4,
        .bluetooth.max_connections = MAX_CONNECTIONS,
        .bluetooth.heap = bluetooth_stack_heap,
        .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
        .bluetooth.sleep_clock_accuracy = 100, // ppm
        .gattdb = &bg_gattdb_data,
        .ota.flags = 0,
        .ota.device_name_len = 4,
        .ota.device_name_ptr = "OTA",
  #if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
        .pa.config_enable = 1, // Enable high power PA
        .pa.input = GECKO_RADIO_PA_INPUT_VBAT,// Configure PA input to VBAT
  #endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
    };

  // Initialize stack
    gecko_init(&config);
}

/************************************************************************/
/*            Função: acorda			                        */
/* Descrição: Acorda o microcontrolador e/ou bluetooth			*/
/* Entrada: Qual módulo se deseja acordar(MICROCONTROLADOR ou BLUETOOTH */
/* Saída:                                                               */
/************************************************************************/
void acorda(unsigned char modulos)
{
  if((modulos & MICROCONTROLADOR) == MICROCONTROLADOR)
    {
      Init_CRYOTimer();
      //USART_STR("ACORDEI o MICRO \r\n");
      //Se não inicializar o MSC_init... não consigo mais gravar com sucesso na EEPROM após o micro dormir...
      MSC_Init();					//Enables the flash controller for writing.
    }

  if((modulos & BLUETOOTH) == BLUETOOTH)
    {
      status_bluetooth = 1;
      //gecko_cmd_system_halt(0);						//Liga Bluetooth (0 = Liga)
      //USART_STR("ACORDEI O RF... \r\n");

    }


}
/************************************************************************/

/************************************************************************/
/*            Função: dorme			                        */
/* Descrição: Dorme o microcontrolador e/ou bluetooth e acorda 1 vez	*/
/* a cada 16 horas, aproximadamente					*/
/* Entrada: Qual módulo se deseja dormir (MICROCONTROLADOR ou BLUETOOTH */
/* Saída:                                                               */
/************************************************************************/
void dorme(unsigned char modulos)
{
  if ((modulos & BLUETOOTH) && (status_bluetooth == 1))
    {

      //gecko_cmd_system_halt(1);						//Desliga Bluetooth (1 = Desliga)
      //Para todos os advertisements para o módulo entrar em sleep automaticamente.
      gecko_cmd_le_gap_set_mode(le_gap_non_discoverable,le_gap_non_connectable);
      status_bluetooth = 0;

      //USART_STR("Desligou o RF do bluetooth... \r\n");
    }

  if((modulos & MICROCONTROLADOR) == MICROCONTROLADOR)
    {
      SLEEP_SleepBlockEnd (sleepEM2); 	// enable sleeping

      CRYOTIMER_Init_TypeDef init;
      init.enable = false;
      init.em4Wakeup = false;
      init.osc = cryotimerOscULFRCO;
      init.presc = cryotimerPresc_1;
      init.period = cryotimerPeriod_128; // nominally 128 mili seconds

      CRYOTIMER_IntDisable(CRYOTIMER_IEN_PERIOD);
      NVIC_ClearPendingIRQ (CRYOTIMER_IRQn);

      CRYOTIMER_Enable(false);

      CRYOTIMER_Init (&init);
    }


}
/************************************************************************/

/************************************************************************/
/*            Função: desconecta_bluetooth	                        */
/* Descrição: Faz a desconexão do bluetooth				*/
/* Entrada: 								*/
/* Saída:                                                               */
/************************************************************************/
void desconecta_bluetooth (void)
{
  //USART_STR("EU DESCONECTEI O BLUETOOTH \r\n");
  gecko_cmd_le_connection_close(_conn_handle);				//Encerra a conexão ou tentativa de conexão
  timeout_to_sleep = 5;							//Agenda para desligara após 2.5s
}
/************************************************************************/

/************************************************************************/
/*            Função: trata_string_bluetooth	                        */
/* Descrição: Faz a tratativa para cada string recebida por bluetooth	*/
/* Entrada: 								*/
/* Saída:                                                               */
/************************************************************************/
void trata_string_bluetooth(void)
{
  //Se recebeu: STI (Status Information)
  if (received_string[0] == 'S' && received_string[1] == 'T' && received_string[2] == 'I')
    {
      if((status_botao & PANIC_MODE) 	== PANIC_MODE)
	{
	  unsigned char send[20] = "PANICO";
	  gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 6, send);	//Envia byte por bluetooth
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"PANIC_SENT \r\n");
#endif
	}
      if((status_botao & CHECKIN_MODE) 	== CHECKIN_MODE)
      	{
      	  unsigned char send[20] = "CHECK-IN";
      	  gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 8, send);	//Envia byte por bluetooth
      	  //USART_STR("CHECK-IN_SENT \r\n");
      	}
      if((status_botao & RING_MODE) 	== RING_MODE)
	{
	  unsigned char send[20] = "RING";
	  gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 4, send);	//Envia byte por bluetooth
	  //USART_STR("RING_SENT \r\n");
	}
      if((status_botao & PAIRING_MODE) 	== PAIRING_MODE)
	{
	  unsigned char send[20] = "PAIRING_MODE";
	  gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 12, send);	//Envia byte por bluetooth
	  //USART_STR("PAIRING_MODE_SENT \r\n");
	}

    }

  //Se recebeu MINOR:
  else if (received_string[0] == 'M' && received_string[1] == 'I' && received_string[2] == 'N' && received_string[3] == 'O' && received_string[4] == 'R' && received_string[5] == ':')
      {
      unsigned int length = strlen((const char*)received_string);
      if(length == 11 && (((status_botao & PAIRING_MODE)) || ((status_botao & PANIC_MODE) && ((!my_minor) || (!my_major)))) )
	{
	  unsigned short int aux;
	  unsigned char temp[5];
	  for(i = 0; i < 5 ; i++)
	    {
	      temp[4 - i] = received_string[6 + i];
	    }
	  my_minor = ((temp[4] - 0x30) * 10000) + ((temp[3] - 0x30) * 1000) + ((temp[2] - 0x30) * 100) + ((temp[1] - 0x30) * 10) + ((temp[0] - 0x30) * 1);
#ifdef COM_DEBUG_SERIAL
	  sprintf((char*)buffer,"NEW MINOR: %d \r\n",my_minor);
	  USART_STR((uint8_t*)buffer);
#endif

	  ps_save_object(EEPROM_MINOR_ADDRESS, &my_minor, sizeof(my_minor));
	  ps_load_object(EEPROM_MINOR_ADDRESS, &aux, sizeof(aux));
	  ///EE_Write(&eeprom_minor, my_minor);				//Salva o valor em decimal na eeprom (eeprom_minor)
	  //EE_Read(&eeprom_minor, &aux);					//Faz a leitura da EEPROM e coloca numa variável auxiliar
#ifdef COM_DEBUG_SERIAL
	  sprintf((char*)buffer,"READ_EEPROM_MINOR_AFTER_RECEIVE: %d \r\n",aux);
	  USART_STR((uint8_t*)buffer);
#endif
	  if(aux == my_minor)						//Compara o que recebeu com o que leu da eeprom após fazer a gravação nela
	    {
#ifdef COM_DEBUG_SERIAL
	      sprintf((char*)buffer,"NEW_MINOR_OK \r\n");
	      USART_STR((uint8_t*)buffer);
#endif
	      unsigned char send[20] = "NEW_MINOR_OK";			//Envia confirmação para o APP
	      gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 12, send);	//Envia byte por bluetooth
	    }
	  else
	    {
#ifdef COM_DEBUG_SERIAL
	      sprintf((char*)buffer,"REPITA_O_MINOR \r\n");
	      USART_STR((uint8_t*)buffer);
#endif
	      unsigned char send[20] = "REPITA_O_MINOR";			//Envia confirmação para o APP
	      gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 14, send);	//Envia byte por bluetooth
	    }
	}
      else
	{
#ifdef COM_DEBUG_SERIAL
	  sprintf((char*)buffer,"NEW_MINOR_ERROR \r\n");
	  USART_STR((uint8_t*)buffer);
#endif
	  unsigned char send[20] = "NEW_MINOR_ERROR";			//Envia confirmação para o APP
	  gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 15, send);	//Envia byte por bluetooth
	}
      }

  //Se recebeu MAJOR:
  else if (received_string[0] == 'M' && received_string[1] == 'A' && received_string[2] == 'J' && received_string[3] == 'O' && received_string[4] == 'R' && received_string[5] == ':')
      {
      unsigned int length = strlen((const char*)received_string);
      if(length == 11 && (((status_botao & PAIRING_MODE)) || ((status_botao & PANIC_MODE) && ((!my_minor) || (!my_major)))) )
	{
	  unsigned char temp[5];
	  unsigned short int aux;
	  for(i = 0; i < 5 ; i++)
	    {
	      temp[4 - i] = received_string[6 + i];
	    }
	  my_major = ((temp[4] - 0x30) * 10000) + ((temp[3] - 0x30) * 1000) + ((temp[2] - 0x30) * 100) + ((temp[1] - 0x30) * 10) + ((temp[0] - 0x30) * 1);
#ifdef COM_DEBUG_SERIAL
	  sprintf((char*)buffer,"NEW MAJOR: %d \r\n",my_major);
	  USART_STR((uint8_t*)buffer);
#endif

	  ps_save_object(EEPROM_MAJOR_ADDRESS, &my_major, sizeof(my_major));
	  ps_load_object(EEPROM_MAJOR_ADDRESS, &aux, sizeof(aux));
	  //EE_Write(&eeprom_major, my_major);					//Grava na EEPROM o MAJOR RECEBIDO
	  //EE_Read(&eeprom_major, &aux);						//Faz a leitura da EEPROM E GRAVA NA VARIÁVEL AUXILIAR

#ifdef COM_DEBUG_SERIAL
	  sprintf((char*)buffer,"READ_EEPROM_MAJOR_AFTER_RECEIVE: %d \r\n",aux);
	  USART_STR((uint8_t*)buffer);
#endif
	  if(aux == my_major)							//Compara o que recebeu com o que leu da eeprom após fazer a gravação nela
	    {
#ifdef COM_DEBUG_SERIAL
	      sprintf((char*)buffer,"NEW_MAJOR_OK \r\n");
	      USART_STR((uint8_t*)buffer);
#endif
	      unsigned char send[20] = "NEW_MAJOR_OK";				//Envia confirmação para o APP
	      gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 12, send);	//Envia byte por bluetooth
	    }
	  else
	    {
#ifdef COM_DEBUG_SERIAL
	      sprintf((char*)buffer,"REPITA_O_MAJOR \r\n");
	      USART_STR((uint8_t*)buffer);
#endif
	      unsigned char send[20] = "REPITA_O_MAJOR";			//Envia confirmação para o APP
	      gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 14, send);	//Envia byte por bluetooth
	    }
	}
      else
	{
#ifdef COM_DEBUG_SERIAL
	  sprintf((char*)buffer,"NEW_MAJOR_ERROR \r\n");
	  USART_STR((uint8_t*)buffer);
#endif
	  unsigned char send[20] = "NEW_MAJOR_ERROR";			//Envia confirmação para o APP
	  gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 15, send);	//Envia byte por bluetooth
	}
      }

  //Se recebeu VBR
  else if (received_string[0] == 'V' && received_string[1] == 'B' && received_string[2] == 'R')
    {
      if (received_string[3] == ':')
	{
	  if (received_string[4] == '1')
	    {

	      flag_vibra_motor = 1;
#ifdef COM_DEBUG_SERIAL
	      USART_STR ((uint8_t*) "VBR1_RECEBIDO...\r\n");
#endif
	    }
	  else if (received_string[4] == '2')//Só recebe VBR:2 se for conexão de terceiro
	    {
	      flag_vibra_motor = 2;
	      conexoes_de_terceiros = 0;
#ifdef COM_DEBUG_SERIAL
	      USART_STR ((uint8_t*) "VBR2_RECEBIDO...\r\n");
#endif
	    }
	  else
	    {
	      flag_vibra_motor = (received_string[4] - 0x30);
#ifdef COM_DEBUG_SERIAL
	      USART_STR ((uint8_t*) "VBRN_RECEBIDO...\r\n");
#endif
	    }
	}

      else
	{
	  flag_vibra_motor = 2;
	  conexoes_de_terceiros = 0;
#ifdef COM_DEBUG_SERIAL
	  USART_STR ((uint8_t*) "VBR_RECEBIDO...\r\n");
#endif
	}
    }

  //Se recebeu GET_MAC
  else if (received_string[0] == 'G' && received_string[1] == 'E' && received_string[2] == 'T' && received_string[3] == '_' && received_string[4] == 'M' && received_string[5] == 'A' && received_string[6] == 'C')
    {
      if(status_botao & PAIRING_MODE)
      {
	  leitura_mac_address();
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"ENVIANDO MAC_ADDRESS: ");
#endif
	  sprintf((char*)buffer,"%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",mac_address[5],mac_address[4],mac_address[3],mac_address[2],mac_address[1],mac_address[0]);
	  //USART_STR((uint8_t*)address);
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)buffer);
	  USART_STR((uint8_t*)"\r\n");
#endif

	  gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 17, buffer);	//Envia byte por bluetooth
      }

    }

  //Se recebeu GET_FW (solicitação da versão de FW)
  else if(received_string[0] == 'G' && received_string[1] == 'E' && received_string[2] == 'T' && received_string[3] == '_' && received_string[4] == 'F' && received_string[5] == 'W')
    {
#ifdef COM_DEBUG_SERIAL
	        USART_STR((uint8_t*)"GET_FW RECEBIDO...\r\n");
#endif
      gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 7, FW_VERSION);	//Envia byte por bluetooth
    }

  //Se recebeu GET_CON
  else if (received_string[0] == 'G' && received_string[1] == 'E' && received_string[2] == 'T' && received_string[3] == '_' && received_string[4] == 'C' && received_string[5] == 'O' && received_string[6] == 'N')
    {
#ifdef COM_DEBUG_SERIAL
	        USART_STR((uint8_t*)"GET_CON_RECEBIDO...\r\n");
#endif
	        sprintf((char*)buffer,"CON:%d \r\n",conexoes_de_terceiros);
	        unsigned short int len = strlen((const char*)buffer);
	        gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, len, buffer);	//Envia byte por bluetooth
	        conexoes_de_terceiros++;
    }
}

/************************************************************************/
/*            Função: registra_pressionamento	                        */
/* Descrição: Faz a contagem de quantas vezes o botão ja foi pressionado*/
/* Entrada: 								*/
/* Saída:                                                               */
/************************************************************************/
void registra_pressionamento(void)
{
  cont_pressed++;
  if(cont_pressed == 255)cont_pressed = 0;
  //EE_Write(&eeprom_cont_pressed, cont_pressed);
  ps_save_object(EEPROM_PRESSED_ADDRESS, &cont_pressed, sizeof(cont_pressed));
}


void leitura_bateria(void)
{
  if(EMU_VmonChannelStatusGet(emuVmonChannel_AVDD))  flag_bateria_baixa = 0;
  else flag_bateria_baixa = 1;
}

/************************************************************************/
/*            Função: leitura_mac_address	                        */
/* Descrição: Faz a leitura do MAC ADDRESS do botão			*/
/* Entrada: 								*/
/* Saída:                                                               */
/************************************************************************/
void leitura_mac_address(void)
{
  pResp = gecko_cmd_system_get_bt_address();
  mac_address[0] = pResp->address.addr[0];
  mac_address[1] = pResp->address.addr[1];
  mac_address[2] = pResp->address.addr[2];
  mac_address[3] = pResp->address.addr[3];
  mac_address[4] = pResp->address.addr[4];
  mac_address[5] = pResp->address.addr[5];

#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"LENDO MAC_ADDRESS: ");
	  sprintf((char*)buffer,"%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",mac_address[5],mac_address[4],mac_address[3],mac_address[2],mac_address[1],mac_address[0]);
	  //USART_STR((uint8_t*)address);
	  USART_STR((uint8_t*)buffer);
	  USART_STR((uint8_t*)"\r\n");
#endif
}



