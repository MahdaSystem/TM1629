/**
 **********************************************************************************
 * @file   TM1629_config.h
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  Project specific configurations
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
#ifndef _TM1629_CONFIG_H_
#define _TM1629_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Configurations ---------------------------------------------------------------*/
/**
 * @brief  Enable support for Common Anode displays
 */   
#define TM1629_CONFIG_SUPPORT_COM_ANODE  1

/**
 * @brief  Define the communication interface to use
*/
#define TM1629_CONFIG_SUPPORT_GPIO   1
#define TM1629_CONFIG_SUPPORT_SPI    0


#ifdef __cplusplus
}
#endif

#endif //! _TM1629_H_