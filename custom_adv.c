
#include "custom_adv.h"
#include "native_gecko.h"

#include <string.h>

void fill_adv_packet(tsCustomAdv *pData, uint8 flags, uint16 companyID, uint8 data, char *name)
{
  int n;

  pData->len_flags = 0x02;
  pData->type_flags = 0x01;
  pData->val_flags = flags;

  pData->len_uuid = 17;
  pData->type_uuid = 0x07;

  pData->uuid[0] = 0x07;
  pData->uuid[1] = 0xb9;
  pData->uuid[2] = 0xf9;
  pData->uuid[3] = 0xd7;
  pData->uuid[4] = 0x50;
  pData->uuid[5] = 0xa4;
  pData->uuid[6] = 0x20;
  pData->uuid[7] = 0x89;
  pData->uuid[8] = 0x77;
  pData->uuid[9] = 0x40;
  pData->uuid[10] = 0xcb;
  pData->uuid[11] = 0xfd;
  pData->uuid[12] = 0x2c;
  pData->uuid[13] = 0xc1;
  pData->uuid[14] = 0x80;
  pData->uuid[15] = 0x48;

  pData->len_manuf = 4; /* 1+2+1 bytes for type, company ID and the payload */
  pData->type_manuf = 0xFF;
  pData->company_LO = companyID & 0xFF;
  pData->company_HI = (companyID >> 8) & 0xFF;

  //pData->num_presses = num_presses;
  pData->data = data;

  n = strlen (name); // name length, excluding null terminator
  if (n > NAME_MAX_LENGTH)
    {
      pData->type_name = 0x08; // incomplete name
    }
  else
    {
      pData->type_name = 0x09;
    }

  strncpy (pData->name, name, NAME_MAX_LENGTH);

  if (n > NAME_MAX_LENGTH)
    {
      n = NAME_MAX_LENGTH;
    }

  pData->len_name = 1 + n; /* length of name element is the name string length + 1 for the AD type */

  /* calculate total length of advertising data*/
  pData->data_size = 3 + (1 + pData->len_manuf) + (1 + pData->len_uuid) + (1 + pData->len_name);

  gecko_cmd_le_gap_set_adv_parameters (160, 160, 7);

  /* set custom advertising payload */
  gecko_cmd_le_gap_set_adv_data (0, pData->data_size, (const uint8 *) pData);

  /* start advertising using custom data */
  gecko_cmd_le_gap_set_mode (le_gap_user_data, le_gap_undirected_connectable);

}

void start_adv (tsCustomAdv *pData)
{
  /* set custom advertising payload */
  gecko_cmd_le_gap_set_adv_data (0, pData->data_size, (const uint8 *) pData);

  /* start advertising using custom data */
  gecko_cmd_le_gap_set_mode (le_gap_user_data, le_gap_undirected_connectable);
}

void update_adv_data (tsCustomAdv *pData, uint8 data, uint8 last_press)
{

  /* update the two variable fields in the custom advertising packet */
  //pData->num_presses = num_presses;
  pData->data = data;

  /* set custom advertising payload */
  gecko_cmd_le_gap_set_adv_data (0, pData->data_size, (const uint8 *) pData);

  /* note: no need to call gecko_cmd_le_gap_set_mode again here. */
}
