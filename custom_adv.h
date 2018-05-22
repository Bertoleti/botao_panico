#ifndef _CUSTOM_ADV_H_
#define _CUSTOM_ADV_H_

#include "bg_types.h"

#define NAME_MAX_LENGTH 20

typedef struct
{
  uint8 len_flags;
  uint8 type_flags;
  uint8 val_flags;

  uint8 len_uuid;
  uint8 type_uuid;
  uint8 uuid[16];

  uint8 len_manuf;
  uint8 type_manuf;
  /* First two bytes must contain the manufacturer ID (little-endian order) */
  uint8 company_LO;
  uint8 company_HI;

  /* the next bytes are freely configurable - using one byte for counter value and one byte for last button press */
  //uint8 num_presses;
  uint8 data;

  /* length of the name AD element is variable, adding it last to keep things simple */
  uint8 len_name;
  uint8 type_name;

  char name[NAME_MAX_LENGTH]; // NAME_MAX_LENGTH must be sized so that total length of data does not exceed 31 bytes

  /* these values are NOT included in the actual advertising payload, just for bookkeeping */
  char dummy;    // space for null terminator
  uint8 data_size; // actual length of advertising data
} tsCustomAdv;

typedef struct
{
  uint8 len_flags;
  uint8 type_flags;
  uint8 val_flags;

  uint8 len_uuid;
  uint8 type_uuid;
  uint8 uuid[16];

  /* length of the name AD element is variable, adding it last to keep things simple */
  uint8 len_name;
  uint8 type_name;

  char name[NAME_MAX_LENGTH]; // NAME_MAX_LENGTH must be sized so that total length of data does not exceed 31 bytes

  /* these values are NOT included in the actual advertising payload, just for bookkeeping */
  char dummy;    // space for null terminator
  uint8 data_size; // actual length of advertising data
} tsCustomPairingAdv;

typedef struct
{
  uint8 len_flags;
  uint8 type_flags;
  uint8 val_flags;

  uint8 len_uuid;
  uint8 type_uuid;
  uint8 uuid[16];

  uint8 len_manuf;
  uint8 type_manuf;
  /* First two bytes must contain the manufacturer ID (little-endian order) */
  uint8 company_LO;
  uint8 company_HI;

  /* the next bytes are freely configurable - using one byte for counter value and one byte for last button press */
  //uint8 num_presses;
  uint8 data;


  uint8 majorMSB;
  uint8 majorLSB;
  uint8 minorMSB;
  uint8 minorLSB;


  /* these values are NOT included in the actual advertising payload, just for bookkeeping */
  char dummy;    // space for null terminator
  uint8 data_size; // actual length of advertising data
} tsCustomPanicAdv;


void fill_adv_packet (tsCustomAdv *pData, uint8 flags, uint16 companyID, uint8 data, char *name);
void fill_adv_pairing_packet(tsCustomPairingAdv *rData, uint8 flags, char *name);
void fill_adv_panic_packet(tsCustomPanicAdv *pData, uint8 flags, uint16 companyID, uint8 data, uint16 major, uint16 minor);
void start_adv (tsCustomAdv *pData);
void update_adv_data (tsCustomAdv *pData, uint8 num_presses, uint8 last_press);

#endif
