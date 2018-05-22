/*
 * interrupt.c
 *
 *  Created on: 9 de jan de 2018
 *      Author: Bruno
 */

#include "em_emu.h"
#include "em_cmu.h"

#include "em_gpio.h"
#include "em_timer.h"
#include "native_gecko.h"
#include "stdio.h"
#include <stdlib.h>

#include "sleep.h"

#include "native_gecko.h"
#include "em_cryotimer.h"
#include "em_usart.h"

#include "User/my_gpio.h"
#include "User/functions.h"
#include "User/my_init.h"
#include "User/my_ibeacon.h"
#include "User/my_usart.h"
#include "eeprom_emulation.h"
#include "custom_adv.h"

tsCustomPairingAdv mData;	/* our custom advertising data stored here */
tsCustomPanicAdv nData;

void TIMER0_IRQHandler(void)			//Timer 20us
{
  TIMER_IntClear(TIMER0, TIMER_IF_OF);
  //GPIO_PinOutToggle(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
}

void TIMER1_IRQHandler(void)			//Timer 1ms
{
  TIMER_IntClear(TIMER1, TIMER_IF_OF);
  //GPIO_PinOutToggle(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
}

void CRYOTIMER_IRQHandler(void)			//Timer principal do sistema
{
  CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);					// Clear the CRYOTIMER interrupt
  //GPIO_PinOutToggle(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
  if(start_cont_button_press)			//Se está fazendo a contagem de quanto tempo o botão está sendo pressionado
    {
      cont_tmr_button_pressed++;
      if(cont_tmr_button_pressed == PANIC_TIME)					//Após 2 segundos pressionado
	{
	  flag_vibra_motor = 1;							//vibra motor uma vez
	  if(status_botao == SLEEP_MODE)
	    {
	      status_botao = PRESSED_2s;
#ifdef COM_DEBUG_SERIAL
	      USART_STR((uint8_t*)"2 Segundos \r\n");
	      sprintf((char*)buffer,"STATUS_BOTAO: %d \r\n",status_botao);
	      USART_STR((uint8_t*)buffer);
#endif
	    }
	  else
	    {
	      status_botao |= PRESSED_2s;
#ifdef COM_DEBUG_SERIAL
	      USART_STR((uint8_t*)"2 Segundos (OU) \r\n");
	      sprintf((char*)buffer,"STATUS_BOTAO: %d \r\n",status_botao);
	      USART_STR((uint8_t*)buffer);
#endif
	    }
	  cont_tmr_led = 0;

	}
      /*
      if(cont_tmr_button_pressed == CHECKIN_TIME)				//Após 7 segundos pressionado
      	{
	  flag_vibra_motor = 2;							//vibra motor 2 vezes
      	  status_botao = PRESSED_4s;
      	  cont_tmr_led = 0;
      	}
      if(cont_tmr_button_pressed == RING_TIME)					//Após 7 segundos pressionado
      	{
	  flag_vibra_motor = 3;							//vibra motor 3 vezes
      	  status_botao = PRESSED_7s;
      	  cont_tmr_led = 0;
      	}
      	*/

      else if(cont_tmr_button_pressed == PAINRING_TIME)				//Após 15 segundos pressionado
	{
	  if(status_botao == PRESSED_2s)
	    {
	      status_botao = PRESSED_15s;
#ifdef COM_DEBUG_SERIAL
	      USART_STR((uint8_t*)"10 Segundos \r\n");
	      sprintf((char*)buffer,"STATUS_BOTAO: %d \r\n",status_botao);
	      USART_STR((uint8_t*)buffer);
#endif

	    }
	  else
	    {
	      status_botao |= PRESSED_15s;
#ifdef COM_DEBUG_SERIAL
	      USART_STR((uint8_t*)"10 Segundos \r\n");
	      sprintf((char*)buffer,"STATUS_BOTAO(OU): %d \r\n",status_botao);
	      USART_STR((uint8_t*)buffer);
#endif
	    }
	  cont_tmr_led = 0;

	}

      else if(cont_tmr_button_pressed == PAINRING_TIME*2)
	{
	  if(status_botao == SLEEP_MODE)
	    {
	      status_botao = PRESSED_MORE_30s;
	    }
	  else
	    {
	      status_botao |= PRESSED_MORE_30s;
	    }
	  cont_tmr_led = 0;
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"20 Segundos \r\n");
#endif
	}
    }

  if(timeout_to_connect)						//Timeout de conexão bluetooth
    {
      timeout_to_connect--;						//Fica decrementando e ciclando até que a variável seja desativada
      if(!timeout_to_connect)
	{
	  timeout_to_connect = TIME_TO_RETRY_CONNECTION;		//Começa contagem regressiva de 10 segundos aguardando uma coenxão do bluetooth
	  //if((status_botao & PAIRING_MODE) != PAIRING_MODE)
	    //{
	      timeout_to_start_beacon = TIME_TO_START_BEACON;		//Começa a contagem regressiva para ligar o beacon novamente
	    //}
	  timeout_to_start_bluetooth = TIME_TO_START_BLUETOOTH;		//Começa contagem regressiva para ligar o bluetooth novamente
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"Timeout_to_connect atingido... \r\n");
#endif
	  ciclos_tentativas++;
	}
    }

  if(timeout_to_start_beacon)
    {
      if(timeout_to_start_beacon == TIME_TO_START_BEACON)
	{
	    bcnSetupAdvBeaconing(WAKEUP_BEACON);			//começa como wakeup beacon para acordar o APP
#ifdef COM_DEBUG_SERIAL
	    USART_STR((uint8_t*)"LIGOU GENERIC iBEACON...\r\n");
#endif
	}
      timeout_to_start_beacon--;
      if(!timeout_to_start_beacon)						//Se deu o tempo necessário apra iniciar o beacon mode
	{
	  if((status_botao & PANIC_MODE) == PANIC_MODE)
	    {
	      bcnSetupAdvBeaconing(PANIC_BEACON);	//Começa com o modo panic beacon...
	    }
	  if((status_botao & CHECKIN_MODE) == CHECKIN_MODE)
	    {
	      bcnSetupAdvBeaconing(CHECKIN_BEACON);	//Começa com o modo panic beacon...
	    }
	  if((status_botao & RING_MODE) == RING_MODE)
	    {
	      bcnSetupAdvBeaconing(RING_BEACON);	//Começa com o modo panic beacon...
	    }
	  if((status_botao == LOW_BAT_ADVERTISE))
	    {
	      bcnSetupAdvBeaconing(LOW_BAT_BEACON);	//Começa com o modo panic beacon...
	    }
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"LIGOU iBEACON POR TIMEOUT...\r\n");
#endif
	}
    }

  if(timeout_to_start_bluetooth)			//Começa contagem regressiva para ligar o bluetooth, desde que o botão não esteja pessionado
    {
      timeout_to_start_bluetooth--;
      if(!timeout_to_start_bluetooth)
	{
	  //gecko_cmd_le_gap_set_mode (le_gap_general_discoverable,le_gap_undirected_connectable);	//Habilita a conexão via bluetooth
	  //Inicializa advertising do bluetooth com o nome SB e com o status do botão já no adertisement, além do UUID de serviço
	  //fill_adv_packet(&mData, 0x06, 0x02FF, (uint8)(ciclos_tentativas), "SB");

	  if(status_botao & PANIC_MODE)
	    {
	      fill_adv_panic_packet(&nData, 0x06, 0x02FF, ciclos_tentativas, my_major + 1, my_minor);
	    }
	  else if (status_botao & PAIRING_MODE)
	    {
	      fill_adv_pairing_packet(&mData, 0x06, "SafeBtn");
	      //gecko_cmd_le_gap_set_mode (le_gap_general_discoverable,le_gap_undirected_connectable);	//Habilita a conexão via bluetooth
	    }

#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"LIGOU BLUETOOTH POR TIMEOUT...\r\n");
#endif
	}
    }

  if(timeout_to_sleep)								//Contagem regressiva para desligar TUDO
    {
      timeout_to_sleep--;

      if(timeout_to_sleep == TIMEOUT_DESCONECTA_BLUETOOTH)
	{
	  if((status_botao & CONNECTED) == CONNECTED)
	    {
#ifdef COM_DEBUG_SERIAL
	      USART_STR((uint8_t*)"Encerrou conexão bluetooth \r\n");
#endif
	      desconecta_bluetooth();
	    }
	}
      if(timeout_to_sleep == TIMEOUT_DORME_BLUETOOTH)
	{
	  dorme(BLUETOOTH);
	}

      if((timeout_to_sleep == 0))
	{
	  timeout_to_connect = 0;		//Desliga o timeout para ciclar o beacon enquanto não tiver sido conectado com sucesso o bluetooth
	  timeout_to_start_beacon = 0;		//Desliga timeout para dar o beacon caso a conexão bluetooth não tenha sido estabelecida
	  timeout_to_start_bluetooth = 0;	//Desliga timeout para dar mudar para o bluetooth mdoe caso a conexão bluetooth não tenha sido estabelecida
	  timeout_to_sleep = 0;			//Desliga contagem regressiva para desligar tudo
	  flag_vibra_motor = 0;
	  conexoes_de_terceiros = 0;
	  ciclos_tentativas = 0;
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"DEU TIMEOUT PRA DORMIR \r\n");
#endif
	  GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
	  GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
	  GPIO_PinOutClear(VIBRA_CALL_GPIO_PORT, VIBRA_CALL_PIN);
	  status_botao = SLEEP_MODE;
	  dorme(BLUETOOTH);
	  dorme(MICROCONTROLADOR);

	}
    }

  if((status_botao & SLEEP_MODE))					//Se o botão está em SLEEP_MODE
    {
      //GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
      //GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
    }

  if((status_botao & CONNECTED) == CONNECTED)					//Se conectou-se por bluetooth
    {
      if((status_botao & PANIC_MODE) == PANIC_MODE)				//Se estiver em PANIC_MODE
	{
	  GPIO_PinOutSet(LED_AMARELO_PORT, LED_AMARELO_PIN);			//Acende os dois LEDS
	  if(flag_bateria_baixa)GPIO_PinOutSet(LED_VERMELHO_PORT, LED_VERMELHO_PIN);
	}

      else if ((status_botao & RING_MODE) == RING_MODE)				//Se estiver em RING_MODE
      	{
	  //GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);			//Acende o da direita
	  //GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);		//Apaga o da esquerda
      	}

      else if ((status_botao & CHECKIN_MODE) == CHECKIN_MODE)			//Se estiver em CHECKIN_MODE
      	{
	  //GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);			//Apaga o da direita
	  //GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);			//Acende o da esquerda
      	}

      else if ((status_botao & PAIRING_MODE) == PAIRING_MODE)			//Se estiver em PAIRING_MODE
      	{
	  cont_tmr_led++;							//Pisca flip flop bem rapido
	  if(cont_tmr_led <= (1*BASE_TEMPO))
	    {
	      GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
	      GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
	    }
	  else
	    {
	      GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
	      GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
	    }
	  if(cont_tmr_led >= (2*BASE_TEMPO))
	    {
	      cont_tmr_led = 0;
	    }
      	}
    }

  else										//Se ainda não está Conectado por bluetooth (Advertising)
    {
      if((status_botao & PANIC_MODE) == PANIC_MODE)				//Em Panic Mode, fica piscando os dois leds juntos até conectar
        {
          cont_tmr_led++;
          if(cont_tmr_led <= (1*(BASE_TEMPO/2)))
	    {
              if(!flag_bateria_baixa) GPIO_PinOutSet(LED_AMARELO_PORT, LED_AMARELO_PIN);
              else GPIO_PinOutSet(LED_VERMELHO_PORT, LED_VERMELHO_PIN);
	    }
	  else
	    {
	      GPIO_PinOutClear(LED_AMARELO_PORT, LED_AMARELO_PIN);
	      GPIO_PinOutClear(LED_VERMELHO_PORT, LED_VERMELHO_PIN);
	    }
	  if(cont_tmr_led >= (4*(BASE_TEMPO)))
	    {
	      cont_tmr_led = 0;
	    }
        }

      if((status_botao & RING_MODE) == RING_MODE)				//Em Ring Mode, fica piscando led da direita
        {
          cont_tmr_led++;
          if(cont_tmr_led <= (1*BASE_TEMPO))
	    {
              GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);			//Acende o da direita
              GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);			//Apaga o da esquerda
	    }
	  else
	    {
	      GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
	      GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
	    }
	  if(cont_tmr_led >= (2*BASE_TEMPO))
	    {
	      cont_tmr_led = 0;
	    }
        }

      if((status_botao & CHECKIN_MODE))				//Em Checkin Mode fica piscando o LED da esquerda
        {
          cont_tmr_led++;
          if(cont_tmr_led <= (1*BASE_TEMPO))
	    {
	      GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);			//Apaga o da direita
	      GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);			//Acende o da esquerda
	    }
	  else
	    {
	      GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
	      GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
	    }
	  if(cont_tmr_led >= (2*BASE_TEMPO))
	    {
	      cont_tmr_led = 0;
	    }
        }

      if((status_botao & PAIRING_MODE))				//Em pairing mode, fica fazendo "flip flop" até conectar
	{
	  cont_tmr_led++;
	  if(cont_tmr_led <= (2*BASE_TEMPO))
	    {
	      GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
	      GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
	    }
	  else
	    {
	      GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
	      GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
	    }
	  if(cont_tmr_led >= (4*BASE_TEMPO))
	    {
	      cont_tmr_led = 0;
	    }
	}
    }

  if(status_botao == PRESSED_15s)
    {
      cont_tmr_led++;
      if(cont_tmr_led <= (4*BASE_TEMPO))
	{
	  GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
	  GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
	}
      else
	{
	  GPIO_PinOutClear(LED_DEBUG_GPIO_PORT, LED_DEBUG_PIN);
	  GPIO_PinOutSet(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
	}
      if(cont_tmr_led >= (6*BASE_TEMPO))
	{
	  cont_tmr_led = 0;
	}
    }

  if(flag_vibra_motor)
    {
      cont_vibra_motor++;
      if(cont_vibra_motor == (1*(BASE_TEMPO/2)))
	{
	  if(!flag_bateria_baixa)
	    {
	      SLEEP_SleepBlockBegin (sleepEM2); // disable sleeping
	      Init_PWM(1);
	    }
	  //if(!flag_bateria_baixa)GPIO_PinOutSet(VIBRA_CALL_GPIO_PORT, VIBRA_CALL_PIN);
	}
      else if (cont_vibra_motor == (2*(BASE_TEMPO/2)))
	{
	  SLEEP_SleepBlockEnd (sleepEM2); // enable sleeping
	  Init_PWM(0);
	  flag_vibra_motor--;
	  cont_vibra_motor = 0;
	  //GPIO_PinOutClear(VIBRA_CALL_GPIO_PORT, VIBRA_CALL_PIN);
	}
    }

  // Flush instructions to make sure the interrupt is not re-triggered.
  // This is generally required when the peripheral clock is slower than
  // the CPU core clock.
  __DSB();
}

void GPIO_ODD_IRQHandler (void)			//Botão 0 (direito da eval)
{
  unsigned long int interrupt_mask;
  //GPIO_PinOutToggle(LED_DEBUG_GPIO_PORT, LED_DEBUG1_PIN);
  interrupt_mask = GPIO_IntGetEnabled();
  GPIO_IntClear(interrupt_mask);

  if(!GPIO_PinInGet(BUTTON_GPIO_PORT,BUTTON_PIN))			//Se for borda de descida
    {
      if(status_botao == SLEEP_MODE)
	{
	  GPIO_PinOutSet(LED_VERMELHO_PORT, LED_VERMELHO_PIN);
	  acorda(MICROCONTROLADOR);
	  cont_tmr_button_pressed = 0;					//Zera a contagem da ultima pressionada de botao..
	  start_cont_button_press = 1;					//Inicia uma nova contagem de tempo de pressionamento do botão
	}
    }
  else if(GPIO_PinInGet(BUTTON_GPIO_PORT,BUTTON_PIN))			//se foi borda de subida
    {
	GPIO_PinOutClear(LED_VERMELHO_PORT, LED_VERMELHO_PIN);
	start_cont_button_press = 0;					//Termina a contagem...

	if(status_botao == PRESSED_2s)					//Se ficou apertado mais que dois segundos...
	{
	  registra_pressionamento();
	  status_botao = PANIC_MODE;
	  acorda(BLUETOOTH);
	  timeout_to_start_beacon = TIME_TO_START_BEACON;
	  timeout_to_start_bluetooth = TIME_TO_START_BLUETOOTH;
	  timeout_to_connect = TIME_TO_RETRY_CONNECTION;
	  timeout_to_sleep = TIME_TO_SLEEP;					//Inicia contagem regressiva para entrar em DEEP SLEEP
	}
	/*
	else if(cont_tmr_button_pressed >= CHECKIN_TIME && cont_tmr_button_pressed < RING_TIME) //Se ficou apertado entre 4 e 7 segundos
	{
	  registra_pressionamento();
	  status_botao = CHECKIN_MODE;
	  acorda(BLUETOOTH);
	  bcnSetupAdvBeaconing(WAKEUP_BEACON);
	  timeout_to_start_beacon = 3;
	  timeout_to_start_bluetooth = 8;
	  timeout_to_connect = 20;
	}

	else if(cont_tmr_button_pressed >= RING_TIME && cont_tmr_button_pressed <= PAINRING_TIME) //Se ficou apertado entre 7 e 10 segundos
	{
	  registra_pressionamento();
	  status_botao = RING_MODE;
	  acorda(BLUETOOTH);
	  bcnSetupAdvBeaconing(WAKEUP_BEACON);
	  timeout_to_start_beacon = 3;
	  timeout_to_start_bluetooth = 8;
	  timeout_to_connect = 20;
	}
	*/

	if(status_botao == PRESSED_15s)
	{
	  status_botao = PAIRING_MODE;
	  acorda(BLUETOOTH);
	  //bcnSetupAdvBeaconing(WAKEUP_BEACON);
	  timeout_to_start_bluetooth = 4*BASE_TEMPO;
	  timeout_to_sleep = TIME_TO_SLEEP;					//Inicia contagem regressiva para entrar em DEEP SLEEP
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"PAIRING MODE \r\n");
	  sprintf((char*)buffer,"STATUS_BOTAO: %d \r\n",status_botao);
	  USART_STR((uint8_t*)buffer);
#endif
	}

	if(((cont_tmr_button_pressed < PANIC_TIME) && (status_botao == SLEEP_MODE)) || (status_botao & PRESSED_MORE_30s))	//Se não estiver em nenhum dos estados acima... desliga tudo e dorme
	{
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"PRESSIONADA RAPIDA \r\n");
	  sprintf((char*)buffer,"STATUS_BOTAO: %d \r\n",status_botao);
	  USART_STR((uint8_t*)buffer);
#endif
	  GPIO_PinOutClear(LED_AMARELO_PORT, LED_AMARELO_PIN);
	  GPIO_PinOutClear(LED_VERMELHO_PORT, LED_VERMELHO_PIN);
	  cont_tmr_button_pressed = 0;
	  timeout_to_start_beacon = 0;
	  timeout_to_start_bluetooth = 0;
	  status_botao = SLEEP_MODE;
	  dorme(MICROCONTROLADOR | BLUETOOTH);
	}
	cont_tmr_button_pressed = 0;					//Zera a contagem
    }

}

void GPIO_EVEN_IRQHandler(void)			//Botão 1 (esquerdo da eval)
{
  unsigned long int interrupt_mask;
  interrupt_mask = GPIO_IntGetEnabled();
  GPIO_IntClear(interrupt_mask);

  //desconecta_bluetooth();
  //printf("BOTAO PRA DESCONECTAR O BLUETOOTH \r\n");
  //bd_addr dado;
  //dado.addr[6] = 0x20;
  //gecko_cmd_system_set_bt_address(dado);

  //unsigned char send[20] = "OI";
  //gecko_cmd_gatt_server_send_characteristic_notification(_conn_handle, gattdb_gatt_ssp_data, 2, send);


  //printf("Endereço: %p \r\n", gecko_cmd_system_get_bt_address());

}

void EMU_IRQHandler(void)
{
  uint32_t flags = EMU_IntGet();
  EMU_IntClear(flags);
  /*
  //GPIO_PinOutToggle(LED_VERMELHO_PORT, LED_VERMELHO_PIN);

  if (flags & EMU_IF_VMONAVDDFALL)
    {
      leitura_bateria();
#ifdef COM_DEBUG_SERIAL
      USART_STR((uint8_t*)"VMONAVDD_FALL\r\n");
#endif
      if(status_botao == SLEEP_MODE)					//Se o botão estava em sleep mode...
	{
	  acorda(MICROCONTROLADOR | BLUETOOTH);				//Inicia a transmissão do beacon de low bat
	  timeout_to_sleep = TIME_TO_SLEEP;				//Inicia contagem regressiva para entrar em DEEP SLEEP
	  leitura_bateria();
	  registra_pressionamento();
	  status_botao = LOW_BAT_ADVERTISE;
	  bcnSetupAdvBeaconing(WAKEUP_BEACON);				//começa como wakeup beacon para acordar o APP
	  timeout_to_start_beacon = TIME_TO_START_BEACON;
	  timeout_to_connect = TIME_TO_RETRY_CONNECTION;
#ifdef COM_DEBUG_SERIAL
	  USART_STR((uint8_t*)"MANDOU ADVERTISE LOW BAT\r\n");
#endif
	}
    }
  else if (flags & EMU_IF_VMONAVDDRISE)
    {
      leitura_bateria();
#ifdef COM_DEBUG_SERIAL
      USART_STR((uint8_t*)"VMONAVDD_RISE\r\n");
#endif
    }
  if (flags & EMU_IF_VMONDVDDFALL)
    {
#ifdef COM_DEBUG_SERIAL
      USART_STR((uint8_t*)"VMONDVDD_FALL\r\n");
#endif
    }
  else if (flags & EMU_IF_VMONDVDDRISE)
    {
#ifdef COM_DEBUG_SERIAL
      USART_STR((uint8_t*)"VMONDVDD_RISE\r\n");
#endif
    }
  if (flags & EMU_IF_VMONIO0FALL)
    {
#ifdef COM_DEBUG_SERIAL
      USART_STR((uint8_t*)"VMONIO0_FALL\r\n");
#endif
    }
  else if (flags & EMU_IF_VMONIO0RISE)
    {
#ifdef COM_DEBUG_SERIAL
      USART_STR((uint8_t*)"VMONIO0_RISE\r\n");
#endif
    }

  */

}

