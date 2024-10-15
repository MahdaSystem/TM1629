# TM1629 Library
Library for handling TM1629 LED display driver.

## Library Features
-   Support for both Common Anode and Common Cathode Seven-segment displays
-   Support for dimming display
-   Support for scan Keypad

## Hardware Support
It is easy to port this library to any platform. But now it is ready for use in:
- ESP32 (esp-idf)

## How To Use
1. Add `TM1629.h`, `TM1629_config.h` and `TM1629.c` files to your project.  It is optional to use `TM1629_platform.h` and `TM1629_platform.c` files (open and config `TM1629_platform.h` file).
2. Initialize platform-dependent part of handler.
4. Call `TM1629_Init()`.
5. Call `TM1629_ConfigDisplay()` to config display.
6. Call other functions and enjoy.

## Example
<details>
<summary>Using TM1629_platform files</summary>

```c
#include <stdio.h>
#include "TM1629.h"
#include "TM1629_platform.h"

int main(void)
{
  TM1629_Handler_t Handler;

  TM1629_Platform_Init_GPIO_3Wire(&Handler); // Initialize to use 3-wire communication through GPIO
  TM1629_Init(&Handler, TM1629_DISPLAY_TYPE_COM_CATHODE);
  TM1629_ConfigDisplay(&Handler, 7, TM1629_DISPLAY_STATE_ON);

  while (1)
  {
    // Display the number 8 and Decimal Point in the SEG1 
    TM1629_SetSingleDigit_HEX(&Handler, 8 | TM1629_DECIMAL_POINT, 0);
  }

  TM1629_DeInit(&Handler);
  return 0;
}
```
</details>


<details>
<summary>Without using TM1629_platform files (AVR 4-wire interface)</summary>

```c
#include <stdio.h>
#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include "TM1629.h"


#define TM1629_STB_DDR    DDRA
#define TM1629_STB_PORT   PORTA
#define TM1629_STB_NUM    0

#define TM1629_CLK_DDR    DDRA
#define TM1629_CLK_PORT   PORTA
#define TM1629_CLK_NUM    1

#define TM1629_DIN_DDR    DDRA
#define TM1629_DIN_PORT   PORTA
#define TM1629_DIN_NUM    2

#define TM1629_DOUT_DDR   DDRA
#define TM1629_DOUT_PORT  PORTA
#define TM1629_DOUT_PIN   PINA
#define TM1629_DOUT_NUM   3


static int8_t
TM1629_PlatformInit(void)
{
  TM1629_STB_DDR |= (1<<TM1629_STB_NUM);
  TM1629_CLK_DDR |= (1<<TM1629_CLK_NUM);
  TM1629_DIN_DDR |= (1<<TM1629_DIN_NUM);
  TM1629_DOUT_DDR &= ~(1<<TM1629_DOUT_NUM);
  return 0;
}

static int8_t
TM1629_PlatformDeInit(void)
{
  TM1629_STB_DDR &= ~(1<<TM1629_STB_NUM);
  TM1629_STB_PORT &= ~(1<<TM1629_STB_NUM);
  TM1629_CLK_DDR &= ~(1<<TM1629_CLK_NUM);
  TM1629_CLK_PORT &= ~(1<<TM1629_CLK_NUM);
  TM1629_DIN_DDR &= ~(1<<TM1629_DIN_NUM);
  TM1629_DIN_PORT &= ~(1<<TM1629_DIN_NUM);
  TM1629_DOUT_DDR &= ~(1<<TM1629_DOUT_NUM);
  TM1629_DOUT_PORT &= ~(1<<TM1629_DOUT_NUM);
  return 0;
}

static int8_t
TM1629_DirDIO(uint8_t Dir)
{
  return 0;
}

static int8_t
TM1629_WriteDIO(uint8_t State)
{
  if (State)
    TM1629_DIN_PORT |= (1<<TM1629_DIN_NUM);
  else
    TM1629_DIN_PORT &= ~(1<<TM1629_DIN_NUM);
  return 0;
}

static int8_t
TM1629_ReadDIO(void)
{
  return (TM1629_DOUT_PIN & (1<<TM1629_DOUT_NUM)) ? 1 : 0;
}

static int8_t
TM1629_WriteSTB(uint8_t State)
{
  if (State)
    TM1629_STB_PORT |= (1<<TM1629_STB_NUM);
  else
    TM1629_STB_PORT &= ~(1<<TM1629_STB_NUM);
  return 0;
}

static int8_t
TM1629_WriteCLK(uint8_t State)
{
  if (State)
    TM1629_CLK_PORT |= (1<<TM1629_CLK_NUM);
  else
    TM1629_CLK_PORT &= ~(1<<TM1629_CLK_NUM);
  return 0;
}

static int8_t
TM1629_DelayUs(uint8_t Delay)
{
  for (; Delay; --Delay)
    _delay_us(1);
}


int main(void)
{
  TM1629_Handler_t Handler;

  TM1629_PLATFORM_SET_COMMUNICATION(&Handler, TM1629_COMMUNICATION_GPIO);
  TM1629_PLATFORM_LINK_INIT(&Handler, TM1629_PlatformInit_GPIO);
  TM1629_PLATFORM_LINK_DEINIT(&Handler, TM1629_PlatformDeInit_GPIO);
  TM1629_PLATFORM_LINK_DIR_DIO(&Handler, TM1629_DirDIO);
  TM1629_PLATFORM_LINK_WRITE_DIO(&Handler, TM1629_WriteDIO);
  TM1629_PLATFORM_LINK_READ_DIO(&Handler, TM1629_ReadDIO);
  TM1629_PLATFORM_LINK_WRITE_STB(&Handler, TM1629_WriteSTB);
  TM1629_PLATFORM_LINK_WRITE_CLK(&Handler, TM1629_WriteCLK);
  TM1629_PLATFORM_LINK_DELAY_US(&Handler, TM1629_DelayUs);

  TM1629_Init(&Handler, TM1629_DISPLAY_TYPE_COM_CATHODE);
  TM1629_ConfigDisplay(&Handler, 7, TM1629_DISPLAY_STATE_ON);

  while (1)
  {
    // Display the number 8 and Decimal Point in the SEG1 
    TM1629_SetSingleDigit_HEX(&Handler, 8 | TM1629_DECIMAL_POINT, 0);
  }

  TM1629_DeInit(&Handler);
  return 0;
}
```
</details>
