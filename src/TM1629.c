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
