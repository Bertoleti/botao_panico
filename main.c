/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"


/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"

/* Device initialization header */
#include "hal-config.h"

#include "bsp.h"
/*
#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#include "retargetserialhalconfig.h"
#else
#include "bspconfig.h"
#include "retargetserialconfig.h"
#endif
*/

//#include "bsp_trace.h"
#include "bg_types.h"
//#include "retargetserial.h"

//#include "spp_utils.h"
//#include "em_timer.h"
#include <stdio.h>


#include "sleep.h"
#include "User/my_init.h"
#include "User/my_ibeacon.h"
#include "User/functions.h"
#include "User/my_gpio.h"
#include "User/my_usart.h"
#include "eeprom_emulation.h"

unsigned long int *endereco_teste;



int main(void)
{
  initMcu();					//Initialize device
  initBoard();					//Initialize board
  Init_Clocks();				//Inicializa o Clocks dos periféricos
  Init_GPIO();					//Inicializa GPIO
  Init_External_Interrupt();			//Inicializa as interrupções externas
  //Init_Timer0();				//Inicializa o Timer0
  //Init_Timer1();				//Inicializa o Timer1 (1ms)
#ifdef COM_DEBUG_SERIAL
  Init_Usart();
#endif
  Init_CRYOTimer();				//nicializa CRYOTimer
  Init_PWM(0);					//Inicializa PWM com ele desligado
  //Voltage_Monitor();

  MSC_Init();					//Enables the flash controller for writing.
  /*
  if ( !EE_Init(2) )
    {
       // If the initialization fails we have to take some measure
	// to obtain a valid set of pages. In this example we simply
	// format the pages
       EE_Format(2);
#ifdef COM_DEBUG_SERIAL
       USART_STR((uint8_t*)"ERRO AO INICIALIZAR EEPROM /r/n");
#endif
    }
    */

  //leitura_bateria();

  //EE_DeclareVariable(&eeprom_minor);		//All variables should be declared prior to any writes.
  //EE_DeclareVariable(&eeprom_major);
  //EE_DeclareVariable(&eeprom_cont_pressed);

  //EE_Read(&eeprom_minor, &my_minor);
  //EE_Read(&eeprom_major, &my_major);
  //EE_Read(&eeprom_cont_pressed, &cont_pressed);


  /*
  if(my_minor == 0)
    {
      my_minor = MINOR_DEFAULT;
      EE_Write(&eeprom_minor, my_minor);				//Salva o valor em decimal na eeprom (eeprom_minor)
    }
  if(my_major == 0)
    {
      my_major = MAJOR_DEFAULT;
      EE_Write(&eeprom_major, my_major);
    }
    */

  /*
#ifdef COM_DEBUG_SERIAL
  sprintf((char*)buffer,"READ_EEPROM_MINOR: %d \r\n",my_minor);
  USART_STR((uint8_t*)buffer);

  sprintf((char*)buffer,"READ_EEPROM_MAJOR: %d \r\n",my_major);
  USART_STR((uint8_t*)buffer);

  sprintf((char*)buffer,"READ_CONT_PRESSED: %d \r\n",cont_pressed);
  USART_STR((uint8_t*)buffer);

  unsigned short int erase_cycles;
  erase_cycles = EE_GetEraseCount();

  sprintf((char*)buffer,"VEZES QUE A MEMORIA FOI APAGADA: %d \r\n",erase_cycles);
    USART_STR((uint8_t*)buffer);

#endif
*/


  //RETARGET_SerialInit();

  configura_bluetooth_sleep();			//Configura e inicializa o stack do bluetooth para sleep mode

  dorme(MICROCONTROLADOR | BLUETOOTH);		//Começa com o microcontrolador em sleep

  while (1)
  {
      verifica_eventos_bluetooth();
  }
}

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */
