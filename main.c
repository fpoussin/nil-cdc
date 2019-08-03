/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "usbcfg.h"

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/


/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

SPIConfig spicfg = {
    false,
    0,
    GPIOB,
    0,
    SPI_CR1_BR_2 | SPI_CR1_BR_1,
    0
};


/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(UARTDriver *uartp) {

    (void)uartp;
}

/*
 * This callback is invoked when a transmission has physically completed.
 */
static void txend2(UARTDriver *uartp) {

    (void)uartp;
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerr(UARTDriver *uartp, uartflags_t e) {

    (void)uartp;
    (void)e;
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(UARTDriver *uartp, uint16_t c) {

    (void)uartp;
    (void)c;
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void rxend(UARTDriver *uartp) {

    (void)uartp;
}

/*
 * UART driver configuration structure.
 */
static UARTConfig uartcfg = {
    txend1,
    txend2,
    rxend,
    rxchar,
    rxerr,
    0,
    0,
    38400,
    0,
    USART_CR2_LINEN,
    0
};

/*
 * SPI TX buffer.
 */
static uint8_t txbuf[6];

/*
 * RGB LED thread
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  while (true) {

    chThdSleepMilliseconds(250);
  }
}

THD_TABLE_BEGIN
  THD_TABLE_ENTRY(waThread1, "spiled", Thread1, NULL)
THD_TABLE_END

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Initializes a serial-over-USB CDC driver.
   */
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  uartStart(&UARTD2, &uartcfg);
  spiStart(&SPID1, &spicfg);

  spiStartSend(&SPID1, 6, txbuf);

  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  /*
   * Normal main() thread activity, spawning shells.
   */
  while (true) {
    chThdSleepMilliseconds(1000);
  }
}
