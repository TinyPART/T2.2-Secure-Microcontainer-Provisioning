/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samd10-xmini
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM D10 Xplained
 *              Mini board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default Baudrate for stdio
 *          mEDBG does not support 115200 baud.
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (57600)
#endif
/** @} */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 9)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 9)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PORT           PORT->Group[PA]
#define BTN0_PIN            GPIO_PIN(PA, 25)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
