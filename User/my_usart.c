

#include "my_usart.h"
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


/* Define termination character */
#define TERMINATION_CHAR    '.'

/* Declare a circular buffer structure to use for Rx and Tx queues */
#define BUFFERSIZE          256

volatile struct circularBuffer
{
  uint8_t  data[BUFFERSIZE];  /* data buffer */
  uint32_t rdI;               /* read index */
  uint32_t wrI;               /* write index */
  uint32_t pendingBytes;      /* count of how many bytes are not yet handled */
  bool     overflow;          /* buffer overflow indicator */
} rxBuf, txBuf = { {0}, 0, 0, 0, false };

static USART_TypeDef           * uart   = USART0;



void Init_Usart(void)
{
    /* Enable clock for GPIO module (required for pin configuration) */
    CMU_ClockEnable(cmuClock_GPIO, true);
    /* Configure GPIO pins */
    GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 1);
    GPIO_PinModeSet(gpioPortA, 1, gpioModeInput, 0);

USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

    /* Prepare struct for initializing UART in asynchronous mode*/
    uartInit.enable       = usartEnable;    /* Don't enable UART upon intialization */
    uartInit.refFreq      = 0;              /* Provide information on reference frequency. When set to 0, the reference frequency is */
    uartInit.baudrate     = 115200;         /* Baud rate */
    uartInit.oversampling = usartOVS16;     /* Oversampling. Range is 4x, 6x, 8x or 16x */
    uartInit.databits     = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
    uartInit.parity       = usartNoParity;  /* Parity mode */
    uartInit.stopbits     = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
    uartInit.mvdis        = false;          /* Disable majority voting */
    uartInit.prsRxEnable  = false;          /* Enable USART Rx via Peripheral Reflex System */
    uartInit.prsRxCh      = usartPrsRxCh0;  /* Select PRS channel if enabled */

    /* Initialize USART with uartInit struct */
    USART_InitAsync(uart, &uartInit);

    /* Prepare UART Rx and Tx interrupts */
    USART_IntClear(uart, _USART_IFC_MASK);
    USART_IntEnable(uart, USART_IEN_RXDATAV);
    NVIC_ClearPendingIRQ(USART0_RX_IRQn);
    NVIC_ClearPendingIRQ(USART0_TX_IRQn);
    NVIC_EnableIRQ(USART0_RX_IRQn);
    NVIC_EnableIRQ(USART0_TX_IRQn);

    /* Enable I/O pins at UART1 location #2 */
    //uart->ROUTELOC0 = USART0_ROUTE_RXPEN | USART0_ROUTE_TXPEN | USART0_ROUTE_LOCATION_LOC2;
    uart->ROUTEPEN = USART_ROUTEPEN_RXPEN;
    uart->ROUTEPEN = USART_ROUTEPEN_TXPEN;
    uart->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC0;
    uart->ROUTELOC0 = USART_ROUTELOC0_TXLOC_LOC0;


    /* Enable UART */
    USART_Enable(uart, usartEnable);
}

/******************************************************************************
 * @brief  uartPutChar function
 *
 *****************************************************************************/
void uartPutChar(uint8_t ch)
{
  /* Check if Tx queue has room for new data */
  if ((txBuf.pendingBytes + 1) > BUFFERSIZE)
  {
    /* Wait until there is room in queue */
    while ((txBuf.pendingBytes + 1) > BUFFERSIZE) ;
  }

  /* Copy ch into txBuffer */
  txBuf.data[txBuf.wrI] = ch;
  txBuf.wrI             = (txBuf.wrI + 1) % BUFFERSIZE;

  /* Increment pending byte counter */
  txBuf.pendingBytes++;

  /* Enable interrupt on USART TX Buffer*/
  USART_IntEnable(uart, USART_IEN_TXBL);
}

/******************************************************************************
 * @brief  uartPutData function
 *
 *****************************************************************************/
void uartPutData(uint8_t * dataPtr, uint32_t dataLen)
{
  uint32_t i = 0;

  /* Check if buffer is large enough for data */
  if (dataLen > BUFFERSIZE)
  {
    /* Buffer can never fit the requested amount of data */
    return;
  }

  /* Check if buffer has room for new data */
  if ((txBuf.pendingBytes + dataLen) > BUFFERSIZE)
  {
    /* Wait until room */
    while ((txBuf.pendingBytes + dataLen) > BUFFERSIZE) ;
  }

  /* Fill dataPtr[0:dataLen-1] into txBuffer */
  while (i < dataLen)
  {
    txBuf.data[txBuf.wrI] = *(dataPtr + i);
    txBuf.wrI             = (txBuf.wrI + 1) % BUFFERSIZE;
    i++;
  }

  /* Increment pending byte counter */
  txBuf.pendingBytes += dataLen;

  /* Enable interrupt on USART TX Buffer*/
  USART_IntEnable(uart, USART_IEN_TXBL);
}

/******************************************************************************
 * @brief  uartGetData function
 *
 *****************************************************************************/
uint32_t uartGetData(uint8_t * dataPtr, uint32_t dataLen)
{
  uint32_t i = 0;

  /* Wait until the requested number of bytes are available */
  if (rxBuf.pendingBytes < dataLen)
  {
    while (rxBuf.pendingBytes < dataLen) ;
  }

  if (dataLen == 0)
  {
    dataLen = rxBuf.pendingBytes;
  }

  /* Copy data from Rx buffer to dataPtr */
  while (i < dataLen)
  {
    *(dataPtr + i) = rxBuf.data[rxBuf.rdI];
    rxBuf.rdI      = (rxBuf.rdI + 1) % BUFFERSIZE;
    i++;
  }

  /* Decrement pending byte counter */
  rxBuf.pendingBytes -= dataLen;

  return i;
}

/**************************************************************************//**
 * @brief UART1 RX IRQ Handler
 *
 * Set up the interrupt prior to use
 *
 * Note that this function handles overflows in a very simple way.
 *
 *****************************************************************************/
void USART0_RX_IRQHandler(void)
{
  /* Check for RX data valid interrupt */
  if (uart->IF & USART_IF_RXDATAV)
  {
    /* Copy data into RX Buffer */
    uint8_t rxData = USART_Rx(uart);
    rxBuf.data[rxBuf.wrI] = rxData;
    rxBuf.wrI             = (rxBuf.wrI + 1) % BUFFERSIZE;
    rxBuf.pendingBytes++;

    /* Flag Rx overflow */
    if (rxBuf.pendingBytes > BUFFERSIZE)
    {
      rxBuf.overflow = true;
    }
  }
}

/**************************************************************************//**
 * @brief UART1 TX IRQ Handler
 *
 * Set up the interrupt prior to use
 *
 *****************************************************************************/

void USART0_TX_IRQHandler(void)
{
  // Check TX buffer level status
  if (uart->IF & USART_IF_TXBL)
  {
    if (txBuf.pendingBytes > 0)
    {
      // Transmit pending character
      USART_Tx(uart, txBuf.data[txBuf.rdI]);
      txBuf.rdI = (txBuf.rdI + 1) % BUFFERSIZE;
      txBuf.pendingBytes--;
    }

    // Disable Tx interrupt if no more bytes in queue
    if (txBuf.pendingBytes == 0)
    {
      USART_IntDisable(uart, USART_IEN_TXBL);
    }
  }
}

void USART_STR (uint8_t *message)         //Escreve uma string via RS-232
{
  uint8_t *ch = message;

  while (*ch)
  {
    USART_Tx(USART0,(uint8_t)*ch);           //  Put the next character into the data transmission
    ch++;                                     //  Grab the next character.
  }

}


