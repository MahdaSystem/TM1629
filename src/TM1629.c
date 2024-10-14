/**
 **********************************************************************************
 * @file   TM1629.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  TM1629 chip driver
 *         Functionalities of the this file:
 *          + Display config and control functions
 *          + Keypad scan functions
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

/* Includes ---------------------------------------------------------------------*/
#include "TM1629.h"


/* Private Constants ------------------------------------------------------------*/
/**
 * @brief  Commands
 */
#define COMMAND_DATA_READING_WRITING_SETTING    0x40  // 0b01000000
#define COMMAND_DISPLAY_CONTROL                 0x80  // 0b10000000
#define COMMAND_ADDRESS_SETTING                 0xC0  // 0b11000000

/**
 * @brief  Data reading/writing setting command
 */
#define COMMAND_DRWS_WRITE_DATA_TO_DISPLAY_REGISTER   0x00  // 0b00000000
#define COMMAND_DRWS_READ_KEY_SCANNING_DATA           0x02  // 0b00000010
#define COMMAND_DRWS_AUTO_INCREASE_OF_ADDRESS         0x00  // 0b00000000
#define COMMAND_DRWS_FIXED_ADDRESS                    0x04  // 0b00000100
#define COMMAND_DRWS_NORMAL_MODE                      0x00  // 0b00000000
#define COMMAND_DRWS_TEST_MODE                        0x08  // 0b00001000

/**
 * @brief  Display control command
 */
#define COMMAND_DC_DISPLAY_IS_OFF   0x00  // 0b00000000
#define COMMAND_DC_DISPLAY_IS_ON    0x08  // 0b00001000


/* Private variables ------------------------------------------------------------*/
/**
 * @brief  Convert HEX number to Seven-Segment code
 */
const uint8_t HexTo7Seg[40] =
{
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F, // 9
  0x77, // A
  0x7c, // b
  0x39, // C
  0x5E, // d
  0x79, // E
  0x71, // F
  0x6F, // g
  0x3D, // G
  0x74, // h
  0x76, // H
  0x05, // i
  0x06, // I
  0x0D, // j
  0x30, // l
  0x38, // L
  0x54, // n
  0x37, // N
  0x5C, // o
  0x3F, // O
  0x73, // P
  0x67, // q
  0x50, // r
  0x6D, // S
  0x78, // t
  0x1C, // u
  0x3E, // U
  0x66, // y
  0x08, // _
  0x40, // -
  0x01  // Overscore
};



/**
 ==================================================================================
                           ##### Common Functions #####                            
 ==================================================================================
 */

/**
 * @brief  Initialize TM1629.
 * @param  Handler: Pointer to handler
 * @param  Type: Determine the type of display
 *         - TM1629_DISPLAY_TYPE_COM_CATHODE: Common-Cathode
 *         - TM1629_DISPLAY_TYPE_COM_ANODE:   Common-Anode
 * @note   If 'TM1629_CONFIG_SUPPORT_COM_ANODE' switch is set to 0, the 'Type'
 *         argument will be ignored 
 *         
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful.
 */
TM1629_Result_t
TM1629_Init(TM1629_Handler_t *Handler, TM1629_DisplayType_t Type)
{
  Handler->DisplayType = TM1629_DISPLAY_TYPE_COM_CATHODE;

#if TM1629_CONFIG_SUPPORT_COM_ANODE
  if (Type == TM1629_DISPLAY_TYPE_COM_CATHODE)
    Handler->DisplayType = TM1629_DISPLAY_TYPE_COM_CATHODE;
  else
    Handler->DisplayType = TM1629_DISPLAY_TYPE_COM_ANODE;
#endif

  if (Handler->Platform.Init)
    Handler->Platform.Init();

  return TM1629_OK;
}

/**
 * @brief  De-Initialize TM1629.
 * @param  Handler: Pointer to handler
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful.
 */
TM1629_Result_t
TM1629_DeInit(TM1629_Handler_t *Handler)
{
  if (Handler->Platform.DeInit)
    Handler->Platform.DeInit();

  return TM1629_OK;
}
