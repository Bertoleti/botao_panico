/*
 * Init.h
 *
 *  Created on: 9 de jan de 2018
 *      Author: Bruno
 */

#ifndef USER_INC_MY_INIT_H_
#define USER_INC_MY_INIT_H_

void Init_Timer0 (void);
void Init_Timer1 (void);
void Init_CRYOTimer ();
void Init_GPIO(void);
void Init_Clocks(void);
void Voltage_Monitor(void);
void Init_PWM(unsigned char state);


void Init_External_Interrupt(void);



extern unsigned short int intensidade_motor;

#endif /* USER_INC_MY_INIT_H_ */
