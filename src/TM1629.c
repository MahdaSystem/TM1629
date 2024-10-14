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


/* Private Macros ---------------------------------------------------------------*/
#define TM1629_CHECK_PLATFORM_INIT(HANDLER)       ((HANDLER)->Platform.Init)
#define TM1629_CHECK_PLATFORM_DEINIT(HANDLER)     ((HANDLER)->Platform.DeInit)
#define TM1629_CHECK_PLATFORM_WRITE_STB(HANDLER)  ((HANDLER)->Platform.WriteSTB)
#define TM1629_CHECK_PLATFORM_DIR_DIO(HANDLER)    ((HANDLER)->Platform.GPIO.DirDIO)
#define TM1629_CHECK_PLATFORM_WRITE_DIO(HANDLER)  ((HANDLER)->Platform.GPIO.WriteDIO)
#define TM1629_CHECK_PLATFORM_WRITE_CLK(HANDLER)  ((HANDLER)->Platform.GPIO.WriteCLK)
#define TM1629_CHECK_PLATFORM_READ_DIO(HANDLER)   ((HANDLER)->Platform.GPIO.ReadDIO)
#define TM1629_CHECK_PLATFORM_DELAY_US(HANDLER)   ((HANDLER)->Platform.GPIO.DelayUs)

#define TM1629_PLATFORM_INIT(HANDLER)     (HANDLER)->Platform.Init()
#define TM1629_PLATFORM_DEINIT(HANDLER)   (HANDLER)->Platform.DeInit()
#define TM1629_WRITE_STB(HANDLER, STATE)  (HANDLER)->Platform.WriteSTB(STATE)
#define TM1629_DIR_DIO(HANDLER, DIR)      (HANDLER)->Platform.GPIO.DirDIO(DIR)
#define TM1629_WRITE_DIO(HANDLER, STATE)  (HANDLER)->Platform.GPIO.WriteDIO(STATE)
#define TM1629_WRITE_CLK(HANDLER, STATE)  (HANDLER)->Platform.GPIO.WriteCLK(STATE)
#define TM1629_READ_DIO(HANDLER)          (HANDLER)->Platform.GPIO.ReadDIO()
#define TM1629_DELAY_US(HANDLER, DELAY)   (HANDLER)->Platform.GPIO.DelayUs(DELAY)

#define TM1629_CHECK_RES_PLATFORM(FUNC)        (FUNC >= 0)

#if (TM1629_CONFIG_SUPPORT_SPI && TM1629_CONFIG_SUPPORT_GPIO)
#define TM1629_IS_COMMUNICATION_GPIO(HANDLER)  ((HANDLER)->Platform.Communication == TM1629_COMMUNICATION_GPIO)
#define TM1629_IS_COMMUNICATION_SPI(HANDLER)   ((HANDLER)->Platform.Communication == TM1629_COMMUNICATION_SPI)
#elif (TM1629_CONFIG_SUPPORT_SPI)
#define TM1629_IS_COMMUNICATION_SPI(HANDLER)   1
#define TM1629_IS_COMMUNICATION_GPIO(HANDLER)  0
#elif (TM1629_CONFIG_SUPPORT_GPIO)
#define TM1629_IS_COMMUNICATION_SPI(HANDLER)   0
#define TM1629_IS_COMMUNICATION_GPIO(HANDLER)  1
#endif


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
                           ##### Private Functions #####                           
 ==================================================================================
 */

static inline void
TM1629_StartComunication(TM1629_Handler_t *Handler)
{
  TM1629_WRITE_STB(Handler, 0);
}

static inline void
TM1629_StopComunication(TM1629_Handler_t *Handler)
{
  TM1629_WRITE_STB(Handler, 1);
}

#if (TM1629_CONFIG_SUPPORT_GPIO)
static inline int8_t
TM1629_WriteBytesGPIO(TM1629_Handler_t *Handler,
                      const uint8_t *Data, uint8_t NumOfBytes)
{
  uint8_t Buff = 0;

  TM1629_DIR_DIO(Handler, 1);

  for (uint8_t j = 0; j < NumOfBytes; j++)
  {
    Buff = Data[j];
    for (uint8_t i = 0; i < 8; ++i, Buff >>= 1)
    {
      TM1629_WRITE_CLK(Handler, 0);
      TM1629_DELAY_US(Handler, 1);
      TM1629_WRITE_DIO(Handler, Buff & 0x01);
      TM1629_WRITE_CLK(Handler, 1);
      TM1629_DELAY_US(Handler, 1);
    }
  }

  return 0;
}

static inline int8_t
TM1629_ReadBytesGPIO(TM1629_Handler_t *Handler,
                     uint8_t *Data, uint8_t NumOfBytes)
{
  uint8_t Buff = 0;

  TM1629_DIR_DIO(Handler, 0);
  TM1629_DELAY_US(Handler, 5);

  for (uint8_t j = 0; j < NumOfBytes; j++)
  {
    Buff = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
      TM1629_WRITE_CLK(Handler, 0);
      TM1629_DELAY_US(Handler, 1);
      TM1629_WRITE_CLK(Handler, 1);
      Buff |= (TM1629_READ_DIO(Handler) << i);
      TM1629_DELAY_US(Handler, 1);
    }

    Data[j] = Buff;
    TM1629_DELAY_US(Handler, 2);
  }

  return 0;
}
#endif

#if (TM1629_CONFIG_SUPPORT_SPI)
static inline int8_t
TM1629_WriteBytesSPI(TM1629_Handler_t *Handler,
                     const uint8_t *Data, uint8_t NumOfBytes)
{
  // TODO: Implement SPI communication
  return -1;
}

static inline int8_t
TM1629_ReadBytesSPI(TM1629_Handler_t *Handler,
                    uint8_t *Data, uint8_t NumOfBytes)
{
  // TODO: Implement SPI communication
  return -1;
}
#endif

static int8_t
TM1629_WriteBytes(TM1629_Handler_t *Handler,
                  const uint8_t *Data, uint8_t NumOfBytes)
{
#if (TM1629_CONFIG_SUPPORT_GPIO && TM1629_CONFIG_SUPPORT_SPI)
  if (TM1629_IS_COMMUNICATION_GPIO(Handler))
    return TM1629_WriteBytesGPIO(Handler, Data, NumOfBytes);
  else
    return TM1629_WriteBytesSPI(Handler, Data, NumOfBytes);
#elif (TM1629_CONFIG_SUPPORT_GPIO)
  return TM1629_WriteBytesGPIO(Handler, Data, NumOfBytes);
#elif (TM1629_CONFIG_SUPPORT_SPI)
  return TM1629_WriteBytesSPI(Handler, Data, NumOfBytes);
#endif

  return -128;
}

static int8_t
TM1629_ReadBytes(TM1629_Handler_t *Handler,
                 uint8_t *Data, uint8_t NumOfBytes)
{
  #if (TM1629_CONFIG_SUPPORT_GPIO && TM1629_CONFIG_SUPPORT_SPI)
  if (TM1629_IS_COMMUNICATION_GPIO(Handler))
    return TM1629_ReadBytesGPIO(Handler, Data, NumOfBytes);
  else
    return TM1629_ReadBytesSPI(Handler, Data, NumOfBytes);
#elif (TM1629_CONFIG_SUPPORT_GPIO)
  return TM1629_ReadBytesGPIO(Handler, Data, NumOfBytes);
#elif (TM1629_CONFIG_SUPPORT_SPI)
  return TM1629_ReadBytesSPI(Handler, Data, NumOfBytes);
#endif

  return -128;
}

static int8_t
TM1629_SetMultipleDisplayRegister(TM1629_Handler_t *Handler,
                                  const uint8_t *DigitData,
                                  uint8_t StartAddr, uint8_t Count)
{
  uint8_t Data = COMMAND_DATA_READING_WRITING_SETTING | 
                 COMMAND_DRWS_WRITE_DATA_TO_DISPLAY_REGISTER |
                 COMMAND_DRWS_AUTO_INCREASE_OF_ADDRESS |
                 COMMAND_DRWS_NORMAL_MODE;

  TM1629_StartComunication(Handler);
  TM1629_WriteBytes(Handler, &Data, 1);
  TM1629_StopComunication(Handler);

  Data = COMMAND_ADDRESS_SETTING | StartAddr;

  TM1629_StartComunication(Handler);
  TM1629_WriteBytes(Handler, &Data, 1);
  TM1629_WriteBytes(Handler, DigitData, Count);
  TM1629_StopComunication(Handler);

  return 0;
}

static int8_t
TM1629_ScanKeyRegs(TM1629_Handler_t *Handler, uint8_t *KeyRegs)
{
  uint8_t Data = COMMAND_DATA_READING_WRITING_SETTING | 
                 COMMAND_DRWS_READ_KEY_SCANNING_DATA |
                 COMMAND_DRWS_AUTO_INCREASE_OF_ADDRESS |
                 COMMAND_DRWS_NORMAL_MODE;

  TM1629_StartComunication(Handler);
  TM1629_WriteBytes(Handler, &Data, 1);
  TM1629_ReadBytes(Handler, KeyRegs, 4);
  TM1629_StopComunication(Handler);

  return 0;
}



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

  if (TM1629_CHECK_PLATFORM_INIT(Handler))
    if (!TM1629_CHECK_RES_PLATFORM(TM1629_PLATFORM_INIT(Handler)))
      return TM1629_FAIL;

  if (TM1629_IS_COMMUNICATION_GPIO(Handler))
  {
    if (!TM1629_CHECK_PLATFORM_DIR_DIO(Handler) ||
        !TM1629_CHECK_PLATFORM_WRITE_STB(Handler) ||
        !TM1629_CHECK_PLATFORM_WRITE_DIO(Handler) ||
        !TM1629_CHECK_PLATFORM_WRITE_CLK(Handler) ||
        !TM1629_CHECK_PLATFORM_READ_DIO(Handler) ||
        !TM1629_CHECK_PLATFORM_DELAY_US(Handler))
      return TM1629_FAIL;
  }
  else if (TM1629_IS_COMMUNICATION_SPI(Handler))
  {
    return TM1629_FAIL;
  }

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
  if (TM1629_CHECK_PLATFORM_DEINIT(Handler))
    if (TM1629_CHECK_RES_PLATFORM(TM1629_PLATFORM_DEINIT(Handler)))
      return TM1629_FAIL;

  return TM1629_OK;
}



/**
 ==================================================================================
                        ##### Public Display Functions #####                       
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
                     uint8_t Brightness, uint8_t DisplayState)
{
  uint8_t Data = COMMAND_DISPLAY_CONTROL;
  Data |= (Brightness & 0x07);
  Data |= (DisplayState != TM1629_DISPLAY_STATE_OFF) ? (COMMAND_DC_DISPLAY_IS_ON) : (COMMAND_DC_DISPLAY_IS_OFF);

  TM1629_StartComunication(Handler);
  TM1629_WriteBytes(Handler, &Data, 1);
  TM1629_StopComunication(Handler);

  return TM1629_OK;
}


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
                      uint8_t DigitData, uint8_t DigitPos)
{ 
  if (Handler->DisplayType == TM1629_DISPLAY_TYPE_COM_CATHODE)
    TM1629_SetMultipleDisplayRegister(Handler, &DigitData, DigitPos, 1);
#if (TM1629_CONFIG_SUPPORT_COM_ANODE)
  else
    TM1629_SetMultipleDigit(Handler, &DigitData, DigitPos, 1);
#endif
  return TM1629_OK;
}


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
                        uint8_t StartAddr, uint8_t Count)
{
  uint8_t Shift = 0;
  uint8_t DigitDataBuff = 0;
  uint8_t i = 0;

  if (Handler->DisplayType == TM1629_DISPLAY_TYPE_COM_CATHODE)
    TM1629_SetMultipleDisplayRegister(Handler, DigitData, StartAddr, Count);
#if (TM1629_CONFIG_SUPPORT_COM_ANODE)
  else
  {
    for (uint8_t j = 0; j < Count; j++)
    {
      DigitDataBuff = DigitData[j];

      if ((j + StartAddr) >= 0 && (j + StartAddr) <= 7)
      {
        Shift = j + StartAddr;
        i = 0;
      }
      else
      {
        Shift = (j + StartAddr) - 8;
        i = 1;
      }

      for (; i < 16; i += 2, DigitDataBuff >>= 1)
      {
        if (DigitDataBuff & 0x01)
          Handler->DisplayRegister[i] |= (1 << Shift);
        else
          Handler->DisplayRegister[i] &= ~(1 << Shift);
      }
    }
    TM1629_SetMultipleDisplayRegister(Handler, Handler->DisplayRegister, 0, 16);
  }
#endif

  return TM1629_OK;
}


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
                          uint8_t DigitData, uint8_t DigitPos)
{
  uint8_t DigitDataHEX = 0;
  uint8_t DecimalPoint = DigitData & 0x80;

  DigitData &= 0x7F;

  if (DigitData <= 15)
  {
    DigitDataHEX = HexTo7Seg[DigitData] | DecimalPoint;
  }
  else
  {
    switch (DigitData)
    {
    case 'A':
    case 'a':
      DigitDataHEX = HexTo7Seg[0x0A] | DecimalPoint;
      break;

    case 'B':
    case 'b':
      DigitDataHEX = HexTo7Seg[0x0B] | DecimalPoint;
      break;

    case 'C':
    case 'c':
      DigitDataHEX = HexTo7Seg[0x0C] | DecimalPoint;
      break;

    case 'D':
    case 'd':
      DigitDataHEX = HexTo7Seg[0x0D] | DecimalPoint;
      break;

    case 'E':
    case 'e':
      DigitDataHEX = HexTo7Seg[0x0E] | DecimalPoint;
      break;

    case 'F':
    case 'f':
      DigitDataHEX = HexTo7Seg[0x0F] | DecimalPoint;
      break;

    default:
      DigitDataHEX = 0;
      break;
    }
  }

  return TM1629_SetSingleDigit(Handler, DigitDataHEX, DigitPos);
}


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
                            uint8_t StartAddr, uint8_t Count)
{
  uint8_t DigitDataHEX[10];
  uint8_t DecimalPoint = 0;

  for (uint8_t i = 0; i < Count; i++)
  {
    DecimalPoint = DigitData[i] & 0x80;

    if ((DigitData[i] & 0x7F) >= 0 && (DigitData[i] & 0x7F) <= 15)
    {
      DigitDataHEX[i] = HexTo7Seg[DigitData[i] & 0x7F] | DecimalPoint;
    }
    else
    {
      switch (DigitData[i] & 0x7F)
      {
      case 'A':
      case 'a':
        DigitDataHEX[i] = HexTo7Seg[0x0A] | DecimalPoint;
        break;

      case 'B':
      case 'b':
        DigitDataHEX[i] = HexTo7Seg[0x0B] | DecimalPoint;
        break;

      case 'C':
      case 'c':
        DigitDataHEX[i] = HexTo7Seg[0x0C] | DecimalPoint;
        break;

      case 'D':
      case 'd':
        DigitDataHEX[i] = HexTo7Seg[0x0D] | DecimalPoint;
        break;

      case 'E':
      case 'e':
        DigitDataHEX[i] = HexTo7Seg[0x0E] | DecimalPoint;
        break;

      case 'F':
      case 'f':
        DigitDataHEX[i] = HexTo7Seg[0x0F] | DecimalPoint;
        break;

      default:
        DigitDataHEX[i] = 0;
        break;
      }
    }
  }

  return TM1629_SetMultipleDigit(Handler,
                                 (const uint8_t *)DigitDataHEX, StartAddr, Count);
}


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
                           uint8_t DigitData, uint8_t DigitPos)
{
  uint8_t DigitDataHEX = 0;
  uint8_t DecimalPoint = DigitData & 0x80;

  DigitData &= 0x7F;

  // numbers 0 - 9
  if ((DigitData & 0x7F) >= (uint8_t)'0' && (DigitData & 0x7F) <= (uint8_t)'9')
  {
    DigitDataHEX = HexTo7Seg[(DigitData - 48) & 0x7F] | DecimalPoint;
  }
  else
  {
    switch (DigitData & 0x7F)
    {
    case 'A':
    case 'a':
      DigitDataHEX = HexTo7Seg[0x0A] | DecimalPoint;
      break;

    case 'B':
    case 'b':
      DigitDataHEX = HexTo7Seg[0x0B] | DecimalPoint;
      break;

    case 'C':
    case 'c':
      DigitDataHEX = HexTo7Seg[0x0C] | DecimalPoint;
      break;

    case 'D':
    case 'd':
      DigitDataHEX = HexTo7Seg[0x0D] | DecimalPoint;
      break;

    case 'E':
    case 'e':
      DigitDataHEX = HexTo7Seg[0x0E] | DecimalPoint;
      break;

    case 'F':
    case 'f':
      DigitDataHEX = HexTo7Seg[0x0F] | DecimalPoint;
      break;

    case 'g':
      DigitDataHEX = HexTo7Seg[0x10] | DecimalPoint;
      break;

    case 'G':
      DigitDataHEX = HexTo7Seg[0x11] | DecimalPoint;
      break;

    case 'h':
      DigitDataHEX = HexTo7Seg[0x12] | DecimalPoint;
      break;

    case 'H':
      DigitDataHEX = HexTo7Seg[0x13] | DecimalPoint;
      break;

    case 'i':
      DigitDataHEX = HexTo7Seg[0x14] | DecimalPoint;
      break;

    case 'I':
      DigitDataHEX = HexTo7Seg[0x15] | DecimalPoint;
      break;

    case 'j':
    case 'J':
      DigitDataHEX = HexTo7Seg[0x16] | DecimalPoint;
      break;

    case 'l':
      DigitDataHEX = HexTo7Seg[0x17] | DecimalPoint;
      break;

    case 'L':
      DigitDataHEX = HexTo7Seg[0x18] | DecimalPoint;
      break;

    case 'n':
      DigitDataHEX = HexTo7Seg[0x19] | DecimalPoint;
      break;

    case 'N':
      DigitDataHEX = HexTo7Seg[0x1A] | DecimalPoint;
      break;

    case 'o':
      DigitDataHEX = HexTo7Seg[0x1B] | DecimalPoint;
      break;

    case 'O':
      DigitDataHEX = HexTo7Seg[0x1C] | DecimalPoint;
      break;

    case 'p':
    case 'P':
      DigitDataHEX = HexTo7Seg[0x1D] | DecimalPoint;
      break;

    case 'q':
    case 'Q':
      DigitDataHEX = HexTo7Seg[0x1E] | DecimalPoint;
      break;

    case 'r':
    case 'R':
      DigitDataHEX = HexTo7Seg[0x1F] | DecimalPoint;
      break;

    case 's':
    case 'S':
      DigitDataHEX = HexTo7Seg[0x20] | DecimalPoint;
      break;

    case 't':
    case 'T':
      DigitDataHEX = HexTo7Seg[0x21] | DecimalPoint;
      break;

    case 'u':
      DigitDataHEX = HexTo7Seg[0x22] | DecimalPoint;
      break;

    case 'U':
      DigitDataHEX = HexTo7Seg[0x23] | DecimalPoint;
      break;

    case 'y':
    case 'Y':
      DigitDataHEX = HexTo7Seg[0x24] | DecimalPoint;
      break;

    case '_':
      DigitDataHEX = HexTo7Seg[0x25] | DecimalPoint;
      break;

    case '-':
      DigitDataHEX = HexTo7Seg[0x26] | DecimalPoint;
      break;

    case '~':
      DigitDataHEX = HexTo7Seg[0x27] | DecimalPoint;
      break;

    default:
      DigitDataHEX = 0;
      break;
    }
  }

  return TM1629_SetSingleDigit(Handler, DigitDataHEX, DigitPos);
}

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
                            uint8_t StartAddr, uint8_t Count)
{
  uint8_t DigitDataHEX[10];
  uint8_t DecimalPoint = 0;

  for (uint8_t i = 0; i < Count; i++)
  {
    DecimalPoint = DigitData[i] & 0x80;

    // numbers 0 - 9
    if ((DigitData[i] & 0x7F) >= (uint8_t)'0' && (DigitData[i] & 0x7F) <= (uint8_t)'9')
    {
      DigitDataHEX[i] = HexTo7Seg[(DigitData[i]-48) & 0x7F] | DecimalPoint;
    }
    else
    {
      switch (DigitData[i] & 0x7F)
      {
      case 'A':
      case 'a':
        DigitDataHEX[i] = HexTo7Seg[0x0A] | DecimalPoint;
        break;

      case 'B':
      case 'b':
        DigitDataHEX[i] = HexTo7Seg[0x0B] | DecimalPoint;
        break;

      case 'C':
      case 'c':
        DigitDataHEX[i] = HexTo7Seg[0x0C] | DecimalPoint;
        break;

      case 'D':
      case 'd':
        DigitDataHEX[i] = HexTo7Seg[0x0D] | DecimalPoint;
        break;

      case 'E':
      case 'e':
        DigitDataHEX[i] = HexTo7Seg[0x0E] | DecimalPoint;
        break;

      case 'F':
      case 'f':
        DigitDataHEX[i] = HexTo7Seg[0x0F] | DecimalPoint;
        break;

      case 'g':
        DigitDataHEX[i] = HexTo7Seg[0x10] | DecimalPoint;
      break;
      
      case 'G':
        DigitDataHEX[i] = HexTo7Seg[0x11] | DecimalPoint;
      break;

      case 'h':
        DigitDataHEX[i] = HexTo7Seg[0x12] | DecimalPoint;
      break;
      
      case 'H':
        DigitDataHEX[i] = HexTo7Seg[0x13] | DecimalPoint;
      break;

      case 'i':
        DigitDataHEX[i] = HexTo7Seg[0x14] | DecimalPoint;
      break;
      
      case 'I':
        DigitDataHEX[i] = HexTo7Seg[0x15] | DecimalPoint;
      break;

      case 'j':
      case 'J':
        DigitDataHEX[i] = HexTo7Seg[0x16] | DecimalPoint;
      break;

      case 'l':
        DigitDataHEX[i] = HexTo7Seg[0x17] | DecimalPoint;
      break;

      case 'L':
        DigitDataHEX[i] = HexTo7Seg[0x18] | DecimalPoint;
      break;

      case 'n':
        DigitDataHEX[i] = HexTo7Seg[0x19] | DecimalPoint;
      break;
      
      case 'N':
        DigitDataHEX[i] = HexTo7Seg[0x1A] | DecimalPoint;
      break;

      case 'o':
        DigitDataHEX[i] = HexTo7Seg[0x1B] | DecimalPoint;
      break;
      
      case 'O':
        DigitDataHEX[i] = HexTo7Seg[0x1C] | DecimalPoint;
      break;

      case 'p':
      case 'P':
        DigitDataHEX[i] = HexTo7Seg[0x1D] | DecimalPoint;
      break;

      case 'q':
      case 'Q':
        DigitDataHEX[i] = HexTo7Seg[0x1E] | DecimalPoint;
      break;

      case 'r':
      case 'R':
        DigitDataHEX[i] = HexTo7Seg[0x1F] | DecimalPoint;
      break;

      case 's':
      case 'S':
        DigitDataHEX[i] = HexTo7Seg[0x20] | DecimalPoint;
      break;

      case 't':
      case 'T':
        DigitDataHEX[i] = HexTo7Seg[0x21] | DecimalPoint;
      break;

      case 'u':
        DigitDataHEX[i] = HexTo7Seg[0x22] | DecimalPoint;
      break;

      case 'U':
        DigitDataHEX[i] = HexTo7Seg[0x23] | DecimalPoint;
      break;

      case 'y':
      case 'Y':
        DigitDataHEX[i] = HexTo7Seg[0x24] | DecimalPoint;
      break;

      case '_':
        DigitDataHEX[i] = HexTo7Seg[0x25] | DecimalPoint;
      break;

      case '-':
        DigitDataHEX[i] = HexTo7Seg[0x26] | DecimalPoint;
      break;

      case '~':
        DigitDataHEX[i] = HexTo7Seg[0x27] | DecimalPoint;
      break;

      default:
        DigitDataHEX[i] = 0;
        break;
      }
    }
  }

  return TM1629_SetMultipleDigit(Handler,
                                 (const uint8_t *)DigitDataHEX, StartAddr, Count);
}
