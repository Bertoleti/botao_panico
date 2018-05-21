/*
 * Init.c
 *
 *  Created on: 9 de jan de 2018
 *      Author: Bruno
 */

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include "em_cryotimer.h"

#include "em_timer.h"
#include "em_usart.h"
//#include "em_adc.h"
//#include "em_dma.h"
#include "my_init.h"
#include "my_gpio.h"

#include "bsp.h"
#include <stdint.h>
//1000       20000
/* Define PWM frequency value */
#define PWM_FREQ 10000

/* VMON rail voltage thresholds, in millivolts */
#define VMON_AVDD_RISE_THRESHOLD_mV  2800
#define VMON_AVDD_FALL_THRESHOLD_mV  2400
#define VMON_DVDD_THRESHOLD_mV       3000
#define VMON_IOVDD_THRESHOLD_mV      3000


unsigned short int intensidade_motor = 3500;	//1000 = 73% | 2000 = 50% | 3000 = 21% | 4000 = MAX

void Init_Timer0 (void)
{
  CMU_ClockEnable(cmuClock_TIMER0,true);		//Liga o clock do periférico

  TIMER_Init_TypeDef timer0_Init = TIMER_INIT_DEFAULT;	//Inicializa a struct com a macro de valores default
  timer0_Init.prescale = timerPrescale1;		//Muda o prescale para 1
  TIMER_IntEnable(TIMER0, TIMER_IF_OF);			//Habilita a interrupção do Timer 0 para interrupção por overflow
  TIMER_Enable(TIMER0,true);				//Habilita o periférico do Timer0

  NVIC_EnableIRQ(TIMER0_IRQn);				//Habilita o Handler NVIC da interrupção
  TIMER_TopSet(TIMER0, 768);				//Contador de 384 para overflow do timer0
  TIMER_Init(TIMER0, &timer0_Init);			//Inicializa o timer com o valor da struct
}

void Init_Timer1 (void)
{
  CMU_ClockEnable(cmuClock_TIMER1,true);		//Liga o clock do periférico

  TIMER_Init_TypeDef timer1_Init = TIMER_INIT_DEFAULT;	//Inicializa a struct com a macro de valores default
  timer1_Init.prescale = timerPrescale1;		//Muda o prescale para 1
  TIMER_IntEnable(TIMER1, TIMER_IF_OF);			//Habilita a interrupção do Timer 0 para interrupção por overflow
  TIMER_Enable(TIMER1,true);				//Habilita o periférico do Timer0
  /* Enable TIMER1 interrupt vector in NVIC */
  NVIC_EnableIRQ(TIMER1_IRQn);				//Habilita o Handler NVIC da interrupção
  TIMER_TopSet(TIMER1, 38400);				//Contador de 3840 para overflow do timer0
  TIMER_Init(TIMER1, &timer1_Init);			//Inicializa o timer com o valor da struct

}

void Init_CRYOTimer ()
{
  CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;
  // Clear CRYOTIMER_IF PERIOD flag; it will be set upon EM4 wake
  CRYOTIMER_IntClear (CRYOTIMER_IF_PERIOD);

  //
  // Set CRYOTIMER parameters.  Note that disabling the CRYOTIMER is
  // necessary after EM4 wake in order to reset the counter, otherwise
  // the next delay before wake won't be the full 2K periods.
  //
  init.enable = false;
  init.em4Wakeup = true;
  init.osc = cryotimerOscULFRCO;
  init.presc = cryotimerPresc_1;
  init.period = cryotimerPeriod_128; // nominally 128 mili seconds

  CRYOTIMER_Init (&init);

  CRYOTIMER_IntClear (CRYOTIMER_IF_PERIOD);
  CRYOTIMER_IntEnable (CRYOTIMER_IEN_PERIOD);
  NVIC_ClearPendingIRQ (CRYOTIMER_IRQn);
  NVIC_EnableIRQ (CRYOTIMER_IRQn);

  CRYOTIMER_Enable (true);



}

void Init_GPIO(void)
{
  GPIO_PinModeSet(VIBRA_CALL_GPIO_PORT, VIBRA_CALL_PIN,gpioModePushPull,0);

  GPIO_PinModeSet(LED_DEBUG_GPIO_PORT,LED_DEBUG_PIN,gpioModePushPull,0);	//Inicializa o o portdo LED_DEBUG como saída pushpull e em nível 1
  GPIO_PinModeSet(LED_DEBUG_GPIO_PORT,LED_DEBUG1_PIN,gpioModePushPull,0);	//Inicializa o o portdo LED_DEBUG1 como saída pushpull e em nível 1

  GPIO_PinModeSet(BUTTON_GPIO_PORT, BUTTON_PIN, gpioModeInputPullFilter, 1);
  //GPIO_PinModeSet(BUTTON_1_GPIO_PORT, BUTTON_1_PIN, gpioModeInputPullFilter, 1);

}

void Init_Clocks(void)
{
  // Enable clock for CRYOTIMER
  CMU_ClockEnable(cmuClock_CRYOTIMER, true);
  // Enable clock for PRS
  CMU_ClockEnable(cmuClock_PRS, true);
  // Enable clock for USART0
  CMU_ClockEnable(cmuClock_USART0, true);
  // Enable GPIO clock source
  CMU_ClockEnable(cmuClock_GPIO, true);
  // Enable ADC0 clock
  CMU_ClockEnable(cmuClock_ADC0,true);
  //Enable DMA clock
  CMU_ClockEnable(cmuClock_LDMA, true);

  //Enable clock for TIMER0 module
    CMU_ClockEnable(cmuClock_TIMER0, true);
}

void Init_External_Interrupt(void)
{
  GPIO_IntConfig(BUTTON_GPIO_PORT,BUTTON_PIN,true,true,true);
  //GPIO_IntConfig(BUTTON_1_GPIO_PORT,BUTTON_1_PIN,false,true,true);

  //NVIC_EnableIRQ(GPIO_EVEN_IRQn);					//Habilita o handler NVIC da interrupção externapara pinos Ímpares
  NVIC_EnableIRQ(GPIO_ODD_IRQn);					//Habilita o handler NVIC da interrupção externapara pinos Pares

  GPIO_IntEnable(true);
  GPIO_IntSet(true);
}

void Voltage_Monitor(void)
{
  /*
  EMU_VmonInit_TypeDef voltage_monitor = EMU_VMONINIT_DEFAULT;
  voltage_monitor.channel = emuVmonChannel_DVDD;
  voltage_monitor.threshold = 3100;
  EMU_VmonInit(&voltage_monitor);
  NVIC_EnableIRQ(EMU_IRQn);
  EMU_VmonEnable(emuVmonChannel_DVDD,true);
  if(!EMU_VmonChannelStatusGet(emuVmonChannel_DVDD))
    {
      GPIO_PinOutToggle(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
    }
*/
  /* Initialize AVDD thresholds */
  EMU_VmonHystInit_TypeDef avddInit = EMU_VMONHYSTINIT_DEFAULT;
  avddInit.channel = emuVmonChannel_AVDD;
  avddInit.riseThreshold = VMON_AVDD_RISE_THRESHOLD_mV;
  avddInit.fallThreshold = VMON_AVDD_FALL_THRESHOLD_mV;
  EMU_VmonHystInit(&avddInit);
  //EMU_VmonInit(&avddInit);

  /* Initialize DVDD threshold */
  EMU_VmonInit_TypeDef dvddInit = EMU_VMONINIT_DEFAULT;
  dvddInit.channel = emuVmonChannel_DVDD;
  dvddInit.threshold = VMON_DVDD_THRESHOLD_mV;
  EMU_VmonInit(&dvddInit);

  /* Initialize IOVDD threshold */
  EMU_VmonInit_TypeDef iovddInit = EMU_VMONINIT_DEFAULT;
  iovddInit.channel = emuVmonChannel_IOVDD0;
  iovddInit.threshold = VMON_IOVDD_THRESHOLD_mV;
  EMU_VmonInit(&iovddInit);

  /* Enable all VMON interrupts */
  uint32_t flags = EMU_IEN_VMONAVDDFALL
                 | EMU_IEN_VMONAVDDRISE
                 //| EMU_IEN_VMONIO0FALL
                 //| EMU_IEN_VMONIO0RISE
                 //| EMU_IEN_VMONDVDDFALL
                 //| EMU_IEN_VMONDVDDRISE
		 ;
  EMU_IntClear(flags);
  EMU_IntEnable(flags);
  NVIC_EnableIRQ(EMU_IRQn);
  NVIC_ClearPendingIRQ(EMU_IRQn);


}

void Init_PWM(unsigned char state)
{

  // Select CC channel parameters
   TIMER_InitCC_TypeDef timerCCInit =
   {
     .eventCtrl  = timerEventEveryEdge,
     .edge       = timerEdgeBoth,
     .prsSel     = timerPRSSELCh0,
     .cufoa      = timerOutputActionNone,
     .cofoa      = timerOutputActionNone,
     .cmoa       = timerOutputActionToggle,
     .mode       = timerCCModePWM,
     .filter     = false,
     .prsInput   = false,
     .coist      = false,
     .outInvert  = false,
   };
   // Configure CC channel 0
   TIMER_InitCC(TIMER0, 0, &timerCCInit);


  if(state)
  {
    TIMER0->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN;
    TIMER0->ROUTELOC0 = _TIMER_ROUTELOC0_CC0LOC_LOC6;
  }



  /* Set Top Value */
  TIMER_TopSet(TIMER0, CMU_ClockFreqGet(cmuClock_HFPER)/PWM_FREQ);

  /* Set compare value starting at 0 - it will be incremented in the interrupt handler */
  TIMER_CompareBufSet(TIMER0, 0, intensidade_motor);

  if(state)
    {
      /* Select timer parameters */
      TIMER_Init_TypeDef timerInit =
      {
        .enable     = true,
        .debugRun   = true,
        .prescale   = timerPrescale2,
        .clkSel     = timerClkSelHFPerClk,
        .fallAction = timerInputActionNone,
        .riseAction = timerInputActionNone,
        .mode       = timerModeUp,
        .dmaClrAct  = false,
        .quadModeX4 = false,
        .oneShot    = false,
        .sync       = false,
      };

      /* Enable overflow interrupt */
      TIMER_IntEnable(TIMER0, TIMER_IF_OF);

      /* Enable TIMER0 interrupt vector in NVIC */
      NVIC_EnableIRQ(TIMER0_IRQn);

      /* Configure timer */
      TIMER_Init(TIMER0, &timerInit);
    }
  else
    {
      /* Select timer parameters */
      TIMER_Init_TypeDef timerInit =
      {
        .enable     = false,
        .debugRun   = true,
        .prescale   = timerPrescale64,
        .clkSel     = timerClkSelHFPerClk,
        .fallAction = timerInputActionNone,
        .riseAction = timerInputActionNone,
        .mode       = timerModeUp,
        .dmaClrAct  = false,
        .quadModeX4 = false,
        .oneShot    = false,
        .sync       = false,
      };

      /* Enable overflow interrupt */
      TIMER_IntEnable(TIMER0, TIMER_IF_OF);

      /* Enable TIMER0 interrupt vector in NVIC */
      NVIC_EnableIRQ(TIMER0_IRQn);

      /* Configure timer */
      TIMER_Init(TIMER0, &timerInit);

    }
}
