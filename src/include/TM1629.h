/**
 **********************************************************************************
 * @file   TM1629.h
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
  
/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef _TM1629_H_
#define _TM1629_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include <stdint.h>
#include "TM1629_config.h"


/* Configurations ---------------------------------------------------------------*/
#ifndef TM1629_CONFIG_SUPPORT_COM_ANODE
  #define TM1629_CONFIG_SUPPORT_COM_ANODE  1
#endif

#ifndef TM1629_CONFIG_SUPPORT_SPI
  #define TM1629_CONFIG_SUPPORT_SPI  1
#endif

#ifndef TM1629_CONFIG_SUPPORT_GPIO
  #define TM1629_CONFIG_SUPPORT_GPIO 1
#endif

#if (TM1629_CONFIG_SUPPORT_SPI == 0 && TM1629_CONFIG_SUPPORT_GPIO == 0)
  #error "TM1629: SPI and GPIO can not be both disabled!"
#endif


/* Exported Constants -----------------------------------------------------------*/
#define TM1629_DISPLAY_STATE_OFF          0
#define TM1629_DISPLAY_STATE_ON           1

#define TM1629_DECIMAL_POINT              0x80

  
/* Exported Data Types ----------------------------------------------------------*/

/**
 * @brief  Data type of library functions result
 */
typedef enum TM1629_Result_e
{
  TM1629_OK      = 0,
  TM1629_FAIL    = -1,
} TM1629_Result_t;


/**
 * @brief  Data type of display type
 */
typedef enum TM1629_DisplayType_e
{
  TM1629_DISPLAY_TYPE_COM_CATHODE = 0,
  TM1629_DISPLAY_TYPE_COM_ANODE
} TM1629_DisplayType_t;


/**
 * @brief  Communication type
 */
typedef enum TM1629_Communication_e
{
  TM1629_COMMUNICATION_GPIO = 0,
  TM1629_COMMUNICATION_SPI  = 1,
} TM1629_Communication_t;


/**
 * @brief  Function type for Initialize/Deinitialize the platform dependent layer.
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: The operation failed. 
 */
typedef int8_t (*TM1629_Platform_InitDeinit_t)(void);


/**
 * @brief  Function type for GPIO write
 * @param  State: GPIO state
 *              - 0: Low
 *              - 1: High
 * 
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: The operation failed. 
 */
typedef int8_t (*TM1629_Platform_GPIO_Write_t)(uint8_t State);


#if (TM1629_CONFIG_SUPPORT_GPIO)
/**
 * @brief  Function type for GPIO configuration
 * @param  Dir: GPIO direction
 *              - 0: Input
 *              - 1: Output
 * 
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: The operation failed. 
 */
typedef int8_t (*TM1629_Platform_GPIO_Config_t)(uint8_t Dir);


/**
 * @brief  Function type for GPIO read
 * @retval
 *         -  0: Low
 *         -  1: High
 *         - -1: The operation failed. 
 */
typedef int8_t (*TM1629_Platform_GPIO_Read_t)(void);


/**
 * @brief  Function type for Delay
 * @param  Delay: Delay time in milliseconds/microseconds
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: The operation failed. 
 */
typedef int8_t (*TM1629_Platform_Delay_t)(uint8_t Delay);
#endif


/**
 * @brief  Platform dependent layer data type
 * @note   It is optional to initialize this functions:
 *         - Init
 *         - DeInit
 * @note   If success the functions must return 0 
 */
typedef struct TM1629_Platform_s
{
#if (TM1629_CONFIG_SUPPORT_GPIO && TM1629_CONFIG_SUPPORT_SPI)
  // Communication type
  TM1629_Communication_t Communication;
#endif

  // Initialize platform dependent layer
  TM1629_Platform_InitDeinit_t Init;
  // De-initialize platform dependent layer
  TM1629_Platform_InitDeinit_t DeInit;

  // Write STB pin
  TM1629_Platform_GPIO_Write_t WriteSTB;

  union
  {
#if TM1629_CONFIG_SUPPORT_GPIO
    // It is It is up to the user to use a 3-wire interface (by shorting the DIN
    // and DOUT signals together) or 4-wire (separate DIN and DOUT signals).
    struct
    {
      // DIO pin(s) configuration
      TM1629_Platform_GPIO_Config_t DirDIO;
      // DIO pin write
      TM1629_Platform_GPIO_Write_t WriteDIO;
      // DIO pin read
      TM1629_Platform_GPIO_Read_t ReadDIO;

      // CLK pin write
      TM1629_Platform_GPIO_Write_t WriteCLK;

      // Delay function in microseconds
      TM1629_Platform_Delay_t DelayUs;
    } GPIO;
#endif

#if TM1629_CONFIG_SUPPORT_SPI
    struct
    {
      // Will be used in future
    } SPI;
#endif
  };
} TM1629_Platform_t;


/**
 * @brief  Handler data type
 * @note   User must initialize platform dependent layer functions
 */
typedef struct TM1629_Handler_s
{
  // Display type (Common-Cathode or Common-Anode)
  TM1629_DisplayType_t DisplayType;

#if (TM1629_CONFIG_SUPPORT_COM_ANODE)
  uint8_t DisplayRegister[16];
#endif

  // Platform dependent layer
  TM1629_Platform_t Platform;
} TM1629_Handler_t;


/* Exported Macros --------------------------------------------------------------*/
#if (TM1629_CONFIG_SUPPORT_SPI && TM1629_CONFIG_SUPPORT_GPIO)
/**
 * @brief  Link platform dependent layer communication type
 * @param  HANDLER: Pointer to handler
 * @param  COM: Communication type
 *         - TM1629_COMMUNICATION_GPIO
 *         - TM1629_COMMUNICATION_SPI
 * 
 */
#define TM1629_PLATFORM_SET_COMMUNICATION(HANDLER, COM) \
  (HANDLER)->Platform.Communication = COM
#else
#define TM1629_PLATFORM_SET_COMMUNICATION(HANDLER, COM) \
  do {} while(0)
#endif

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define TM1629_PLATFORM_LINK_INIT(HANDLER, FUNC) \
  (HANDLER)->Platform.Init = FUNC

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define TM1629_PLATFORM_LINK_DEINIT(HANDLER, FUNC) \
  (HANDLER)->Platform.DeInit = FUNC

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define TM1629_PLATFORM_LINK_WRITE_STB(HANDLER, FUNC) \
  (HANDLER)->Platform.WriteSTB = FUNC

#if (TM1629_CONFIG_SUPPORT_GPIO)
/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define TM1629_PLATFORM_LINK_DIR_DIO(HANDLER, FUNC) \
  (HANDLER)->Platform.GPIO.DirDIO = FUNC

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define TM1629_PLATFORM_LINK_WRITE_DIO(HANDLER, FUNC) \
  (HANDLER)->Platform.GPIO.WriteDIO = FUNC

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */

#define TM1629_PLATFORM_LINK_READ_DIO(HANDLER, FUNC) \
  (HANDLER)->Platform.GPIO.ReadDIO = FUNC

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define TM1629_PLATFORM_LINK_WRITE_CLK(HANDLER, FUNC) \
  (HANDLER)->Platform.GPIO.WriteCLK = FUNC

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define TM1629_PLATFORM_LINK_DELAY_US(HANDLER, FUNC) \
  (HANDLER)->Platform.GPIO.DelayUs = FUNC
#endif



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
TM1629_Init(TM1629_Handler_t *Handler, TM1629_DisplayType_t Type);


/**
 * @brief  De-Initialize TM1629.
 * @param  Handler: Pointer to handler
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful.
 */
TM1629_Result_t
TM1629_DeInit(TM1629_Handler_t *Handler);



/**
 ==================================================================================
                           ##### Display Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Config display parameters
 * @param  Handler: Pointer to handler
 * @param  Brightness: Set brightness level
 *         - 0: Display pulse width is set as 1/16
 *         - 1: Display pulse width is set as 2/16
 *         - 2: Display pulse width is set as 4/16
 *         - 3: Display pulse width is set as 10/16
 *         - 4: Display pulse width is set as 11/16
 *         - 5: Display pulse width is set as 12/16
 *         - 6: Display pulse width is set as 13/16
 *         - 7: Display pulse width is set as 14/16
 * 
 * @param  DisplayState: Set display ON or OFF
 *         - TM1629_DISPLAY_STATE_OFF: Set display state OFF
 *         - TM1629_DISPLAY_STATE_ON: Set display state ON
 * 
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful
 */
TM1629_Result_t
TM1629_ConfigDisplay(TM1629_Handler_t *Handler,
                     uint8_t Brightness, uint8_t DisplayState);


/**
 * @brief  Set data to single digit in 7-segment format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Digit data
 * @param  DigitPos: Digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful
 */
TM1629_Result_t
TM1629_SetSingleDigit(TM1629_Handler_t *Handler,
                      uint8_t DigitData, uint8_t DigitPos);


/**
 * @brief  Set data to multiple digits in 7-segment format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Array to Digits data
 * @param  StartAddr: First digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @param  Count: Number of segments to write data
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful
 */
TM1629_Result_t
TM1629_SetMultipleDigit(TM1629_Handler_t *Handler, const uint8_t *DigitData,
                        uint8_t StartAddr, uint8_t Count);


/**
 * @brief  Set data to single digit in hexadecimal format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Digit data (0, 1, ... , 15, a, A, b, B, ... , f, F) 
 * @param  DigitPos: Digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful
 */
TM1629_Result_t
TM1629_SetSingleDigit_HEX(TM1629_Handler_t *Handler,
                          uint8_t DigitData, uint8_t DigitPos);


/**
 * @brief  Set data to multiple digits in hexadecimal format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Array to Digits data. 
 *                    (0, 1, ... , 15, a, A, b, B, ... , f, F)
 * 
 * @param  StartAddr: First digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @param  Count: Number of segments to write data
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful
 */
TM1629_Result_t
TM1629_SetMultipleDigit_HEX(TM1629_Handler_t *Handler, const uint8_t *DigitData,
                            uint8_t StartAddr, uint8_t Count);


/**
 * @brief  Set data to single digit in char format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Digit data
 *                    Supported chars 0,1,2,3,4,5,6,7,8,9
 *                                    A,b,C,d,E,F,g,G,h,H,i,I,j,l,L,n,N,o,O,P,q,r,S,
 *                                    t,u,U,y,_,-,Overscore (use ~ to set)
 * @param  DigitPos: Digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful
 */
TM1629_Result_t
TM1629_SetSingleDigit_CHAR(TM1629_Handler_t *Handler,
                           uint8_t DigitData, uint8_t DigitPos);


/**
 * @brief  Set data to multiple digits in char format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Array to Digits data. 
 *                    Supported chars 0,1,2,3,4,5,6,7,8,9
 *                                    A,b,C,d,E,F,g,G,h,H,i,I,j,l,L,n,N,o,O,P,q,r,S,
 *                                    t,u,U,y,_,-,Overscore (use ~ to set)
 * 
 * @param  StartAddr: First digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @param  Count: Number of segments to write data
 * @retval TM1629_Result_t
 *         - TM1629_OK: Operation was successful
 */
TM1629_Result_t
TM1629_SetMultipleDigit_CHAR(TM1629_Handler_t *Handler, const uint8_t *DigitData,
                             uint8_t StartAddr, uint8_t Count);



#ifdef __cplusplus
}
#endif

#endif //! _TM1629_H_
