/*
 * my_ibeacon.h
 *
 *  Created on: 17 de jan de 2018
 *      Author: bruno
 */

#ifndef USER_MY_IBEACON_H_
#define USER_MY_IBEACON_H_



extern unsigned short int my_major;
extern unsigned short int my_minor;

extern unsigned char my_ibeacon_uuid[16];
extern unsigned char potencia_rf;


enum beacon_mode
{
  WAKEUP_BEACON		= 1,
  PANIC_BEACON 		= 2,
  CHECKIN_BEACON	= 4,
  RING_BEACON		= 8,
  LOW_BAT_BEACON	= 16,
  FACTORY_BEACON	= 32,
};


void bcnSetupAdvBeaconing (unsigned char mode);
void eddySetupAdvBeaconing (unsigned char mode);


#endif /* USER_MY_IBEACON_H_ */
