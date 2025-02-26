/**
 **********************************************************************************
 * @file   TM1629_platform.h
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  A sample Platform dependent layer for TM1629 Driver
 **********************************************************************************
 *
 * Copyright (c) 2024 Mahda Embedded System (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */
  
/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef _TM1629_PLATFORM_H_
#define _TM1629_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include "TM1629.h"
#include <stdint.h>


/* Functionality Options --------------------------------------------------------*/
/**
 * @brief  Specify IO Pins of ESP32 connected to TM1629
 * @note   In 3-wire interface, connect DIN and DOUT pins of TM1629 to the same 
 *         GPIO pin and use 'TM1629_DIO_GPIO' for both.
 * 
 * @note   In 4-wire interface, connect DIN and DOUT pins of TM1629 to separate
 *         GPIO pins and use 'TM1629_DIN_GPIO' and 'TM1629_DOUT_GPIO' for each.
 */
#define TM1629_CLK_GPIO     GPIO_NUM_18
#define TM1629_STB_GPIO     GPIO_NUM_5
#define TM1629_DIN_GPIO     GPIO_NUM_23
#define TM1629_DOUT_GPIO    GPIO_NUM_19
#define TM1629_DIO_GPIO     GPIO_NUM_23



/**
 ==================================================================================
                               ##### Functions #####                               
 ==================================================================================
 */

/**
 * @brief  Initialize platform device to communicate TM1629 using 3-wire interface
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
TM1629_Platform_Init_GPIO_3Wire(TM1629_Handler_t *Handler);


/**
 * @brief  Initialize platform device to communicate TM1629 using 4-wire interface
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
TM1629_Platform_Init_GPIO_4Wire(TM1629_Handler_t *Handler);



#ifdef __cplusplus
}
#endif

#endif //! _TM1629_PLATFORM_H_
