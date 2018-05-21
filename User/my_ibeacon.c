/*
 * my_ibeacon.c
 *
 *  Created on: 17 de jan de 2018
 *      Author: bruno
 */

#include "native_gecko.h"
#include "my_ibeacon.h"
#include "stdio.h"
#include "infrastructure.h"
#include "User/functions.h"
#include "eeprom_emulation.h"
#include "User/my_usart.h"


//unsigned char my_ibeacon_uuid[16] = {0xE2, 0xC5, 0x6D, 0xB5, 0xDF, 0xFB, 0x48, 0xD2, 0xB0, 0x60, 0xD0, 0xF5,
//				     0xA7, 0x10, 0x96, 0xE0};

unsigned char my_ibeacon_uuid[16] = {0x20, 0xCC, 0x4C, 0xE3, 0x5D, 0x0B, 0x42, 0xC8, 0xA5, 0x7C, 0xED, 0x6E,
				     0xE9, 0x45, 0x41, 0x1F};



//unsigned char my_major[2] = {0x88, 0xAB};
//unsigned char my_minor[2] = {0x04, 0x01};
unsigned short int my_major;
unsigned short int my_minor;

unsigned short int minor_aux;
unsigned short int major_aux;

unsigned char potencia_rf = 100;

unsigned int i;

void bcnSetupAdvBeaconing (unsigned char mode)
{
  /* This function sets up a custom advertisement package according to iBeacon specifications.
   * The advertisement package is 30 bytes long. See the iBeacon specification for further details.
   */

  if(mode == WAKEUP_BEACON)			//No modo wakeup, manda o MINOR e MAJOR default, para apenas acordar o APP
    {
      //Não mexer nesses valores nem fodendo!!!
      minor_aux = 1025;
      major_aux = 39164;
    }
  else if(mode == PANIC_BEACON)			//No modo panico... Manda o MINOR salvo na eeprom e o Major salvo na EEPROM Deslicado 2bits + o status 1
    {
      /*
      EE_Read(&eeprom_minor, &my_minor);
      EE_Read(&eeprom_major, &my_major);
      my_major = 1;
      my_major = (my_major << 2) + 1;		//1 = pânico
      */
      major_aux = my_major + 1;
      minor_aux = my_minor;

#ifdef COM_DEBUG_SERIAL
      sprintf((char*)buffer,"MINOR_BEACON_ESPECIFICO: %d \r\n",minor_aux);
      USART_STR((uint8_t*)buffer);

      sprintf((char*)buffer,"MAJOR_BEACON_ESPECIFICO: %d \r\n",major_aux);
      USART_STR((uint8_t*)buffer);

#endif
    }
  else if(mode == CHECKIN_BEACON)		//No modo panico... Manda o MINOR salvo na eeprom e o major = 1 é a indicação de que houve pânico
    {
      //EE_Read(&eeprom_minor, &my_minor);
      //EE_Read(&eeprom_major, &my_major);
      //my_major = (my_major << 2) + 2;		//2 = check-in
      //my_major = 2;
      minor_aux = my_minor;
    }
  else if(mode == RING_BEACON)			//No modo panico... Manda o MINOR salvo na eeprom e o major = 1 é a indicação de que houve pânico
    {
      //EE_Read(&eeprom_minor, &my_minor);
      //EE_Read(&eeprom_major, &my_major);
      //my_major = (my_major << 2) + 3;		//3 = RING
      my_major = 3;
      minor_aux = my_minor;
    }
  else if(mode == LOW_BAT_BEACON)
    {
      //EE_Read(&eeprom_minor, &my_minor);
      //EE_Read(&eeprom_major, &my_major);
      //my_major = (my_major << 2) + 3;
      my_major = 4;
      minor_aux = my_minor;
    }
  else if (mode == FACTORY_BEACON)
    {
      major_aux = BYTES_TO_UINT16(mac_address[2],mac_address[3]);
      minor_aux = BYTES_TO_UINT16(mac_address[0],mac_address[1]);

#ifdef COM_DEBUG_SERIAL
      sprintf((char*)buffer,"MAJOR_BEACON_FACTORY: %d \r\n",major_aux);
      USART_STR((uint8_t*)buffer);

      sprintf((char*)buffer,"MINOR_BEACON_FACTORY: %d \r\n",minor_aux);
      USART_STR((uint8_t*)buffer);
#endif
      //major_aux = my_major + 1;
      //minor_aux = mac_address[1];
    }

  struct
  {
    uint8_t flagsLen; /* Length of the Flags field. */
    uint8_t flagsType; /* Type of the Flags field. */
    uint8_t flags; /* Flags field. */
    uint8_t mandataLen; /* Length of the Manufacturer Data field. */
    uint8_t mandataType; /* Type of the Manufacturer Data field. */
    uint8_t compId[2]; /* Company ID field. */
    uint8_t beacType[2]; /* Beacon Type field. */
    uint8_t uuid[16]; /* 128-bit Universally Unique Identifier (UUID). The UUID is an identifier for the company using the beacon*/
    uint8_t majNum[2]; /* Beacon major number. Used to group related beacons. */
    uint8_t minNum[2]; /* Beacon minor number. Used to specify individual beacons within a group.*/
    uint8_t txPower; /* The Beacon's measured RSSI at 1 meter distance in dBm. See the iBeacon specification for measurement guidelines. */
  } bcnBeaconAdvData =
    {
    /* Flag bits - See Bluetooth 4.0 Core Specification , Volume 3, Appendix C, 18.1 for more details on flags. */
    2, /* length  */
    0x01, /* type */
    0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */

    /* Manufacturer specific data */
    26, /* length of field*/
    0xFF, /* type of field */

    /* The first two data octets shall contain a company identifier code from
     * the Assigned Numbers - Company Identifiers document */
    /* 0x004C = Apple */
      {UINT16_TO_BYTES(0x004C)},
    //{0x00,0x4C},

    /* Beacon type */
    /* 0x0215 is iBeacon */
      //{ UINT16_TO_BYTE1(0x0215), UINT16_TO_BYTE0(0x0215) },
      {0x02, 0x15},

    /* 128 bit / 16 byte UUID */
      //{0xE2, 0xC5, 0x6D, 0xB5, 0xDF, 0xFB, 0x48, 0xD2, 0xB0, 0x60, 0xD0, 0xF5,
       //0xA7, 0x10, 0x96, 0xE0},
       {
	   my_ibeacon_uuid[0],
	   my_ibeacon_uuid[1],
	   my_ibeacon_uuid[2],
	   my_ibeacon_uuid[3],
	   my_ibeacon_uuid[4],
	   my_ibeacon_uuid[5],
	   my_ibeacon_uuid[6],
	   my_ibeacon_uuid[7],
	   my_ibeacon_uuid[8],
	   my_ibeacon_uuid[9],
	   my_ibeacon_uuid[10],
	   my_ibeacon_uuid[11],
	   my_ibeacon_uuid[12],
	   my_ibeacon_uuid[13],
	   my_ibeacon_uuid[14],
	   my_ibeacon_uuid[15],
       },

    /* Beacon major number */
    /* Set to and converted to correct format */
      //{ UINT16_TO_BYTE1(34987), UINT16_TO_BYTE0(34987) },
       //{0x88, 0xAB},
       //{my_major[0], my_major[1]},
       { UINT16_TO_BYTE1(major_aux), UINT16_TO_BYTE0(major_aux) },

    /* Beacon minor number */
    /* Set as 1025 and converted to correct format */
      //{ UINT16_TO_BYTE1(1025), UINT16_TO_BYTE0(1025) },
      //{0x04, 0x01},
      //{my_minor[0], my_minor[1]},
      { UINT16_TO_BYTE1(minor_aux), UINT16_TO_BYTE0(minor_aux)},

    /* The Beacon's measured RSSI at 1 meter distance in dBm */
    /* 0xC3 is -61dBm */
    //0xC3};  	//0bdm
    //0xCA};	//10dbm
      cont_pressed};//envia o ID (numeração da "pressionada")


  uint8_t len = sizeof(bcnBeaconAdvData);
  uint8_t *pData = (uint8_t*) (&bcnBeaconAdvData);

  /* Set 0 dBm Transmit Power */
  gecko_cmd_system_set_tx_power(potencia_rf);

  /* Set custom advertising data */
  gecko_cmd_le_gap_set_adv_data(0, len, pData);

  /* Set advertising parameters. 100ms advertisement interval. All channels used.
   * The first two parameters are minimum and maximum advertising interval, both in
   * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */
  gecko_cmd_le_gap_set_adv_parameters(160, 160, 7);

  /* Start advertising in user mode and enable connections */
  gecko_cmd_le_gap_set_mode (le_gap_user_data, le_gap_non_connectable);
}

void eddySetupAdvBeaconing (unsigned char mode)
{
  /* This function sets up a custom advertisement package according to iBeacon specifications.
   * The advertisement package is 30 bytes long. See the iBeacon specification for further details.
   */


  if(mode == WAKEUP_BEACON)			//No modo wakeup, manda o MINOR e MAJOR default, para apenas acordar o APP
    {
      //Não mexer nesses valores nem fodendo!!!
      my_minor = 1025;
      my_major = 39164;
      cont_pressed = 1;
    }
  else if(mode == PANIC_BEACON)			//No modo panico... Manda o MINOR salvo na eeprom e o Major salvo na EEPROM Deslicado 2bits + o status 1
    {
      EE_Read(&eeprom_minor, &my_minor);
      EE_Read(&eeprom_major, &my_major);
      my_major = 1;
      EE_Read(&eeprom_cont_pressed, &cont_pressed);
      //my_major = (my_major << 2) + 1;		//1 = pânico
    }
  else if(mode == CHECKIN_BEACON)		//No modo panico... Manda o MINOR salvo na eeprom e o major = 1 é a indicação de que houve pânico
    {
      EE_Read(&eeprom_minor, &my_minor);
      EE_Read(&eeprom_major, &my_major);
      EE_Read(&eeprom_cont_pressed, &cont_pressed);
      //my_major = (my_major << 2) + 2;		//2 = check-in
      my_major = 2;
    }
  else if(mode == RING_BEACON)			//No modo panico... Manda o MINOR salvo na eeprom e o major = 1 é a indicação de que houve pânico
    {
      EE_Read(&eeprom_minor, &my_minor);
      EE_Read(&eeprom_major, &my_major);
      EE_Read(&eeprom_cont_pressed, &cont_pressed);
      //my_major = (my_major << 2) + 3;		//2 = RING
      my_major = 3;
    }


  unsigned char eddyBeaconAdvData[] =
      {
	  //Padrão do Eddystone
	  0x02,
	  0x01,
	  0x06,
	  0x03,
	  0x03,
	  0xAA,
	  0xFE,
	  0x17,
	  0x16,
	  0xAA,
	  0xFE,

	  0x00,				//Frame Type
	  0xF4,				//TX Power

	  //NID
	  my_ibeacon_uuid[0],
	  my_ibeacon_uuid[1],
	  my_ibeacon_uuid[2],
	  my_ibeacon_uuid[3],
	  my_ibeacon_uuid[10],
	  my_ibeacon_uuid[11],
	  my_ibeacon_uuid[12],
	  my_ibeacon_uuid[13],
	  my_ibeacon_uuid[14],
	  my_ibeacon_uuid[15],

	  //BID
	  UINT16_TO_BYTE1(my_major),
	  UINT16_TO_BYTE0(my_major),
	  UINT16_TO_BYTE1(my_minor),
	  UINT16_TO_BYTE0(my_minor),
	  0x00,
	  cont_pressed,

	  //Reserved, must be 0x00
	  0x00,
	  0x00};


  uint8_t len = sizeof(eddyBeaconAdvData);
  uint8_t *pData = (uint8_t*) (&eddyBeaconAdvData);

  /* Set 0 dBm Transmit Power */
  gecko_cmd_system_set_tx_power(potencia_rf);

  /* Set custom advertising data */
  gecko_cmd_le_gap_set_adv_data(0, len, pData);

  /* Set advertising parameters. 100ms advertisement interval. All channels used.
   * The first two parameters are minimum and maximum advertising interval, both in
   * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */
  gecko_cmd_le_gap_set_adv_parameters(160, 160, 7);

  /* Start advertising in user mode and enable connections */
  gecko_cmd_le_gap_set_mode (le_gap_user_data, le_gap_non_connectable);
}
