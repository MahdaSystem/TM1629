/**
 **********************************************************************************
 * @file   TM1629_platform.c
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
  
/* Includes ---------------------------------------------------------------------*/
#include "TM1629_platform.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"



/**
 ==================================================================================
                           ##### Private Functions #####                           
 ==================================================================================
 */

static int8_t
TM1629_SetGPIO_OUT(gpio_num_t GPIO_Pad)
{
  gpio_reset_pin(GPIO_Pad);
  return gpio_set_direction(GPIO_Pad, GPIO_MODE_OUTPUT) == ESP_OK ? 0 : -1;
}

static int8_t
TM1629_SetGPIO_IN_PU(gpio_num_t GPIO_Pad)
{
  gpio_reset_pin(GPIO_Pad);
  gpio_set_direction(GPIO_Pad, GPIO_MODE_INPUT);
  return gpio_set_pull_mode(GPIO_Pad, GPIO_PULLUP_ONLY) == ESP_OK ? 0 : -1;
}


static int8_t
TM1629_PlatformInit_GPIO_3Wire(void)
{
  TM1629_SetGPIO_OUT(TM1629_CLK_GPIO);
  return TM1629_SetGPIO_OUT(TM1629_STB_GPIO);
}

static int8_t
TM1629_PlatformDeInit_GPIO_3Wire(void)
{
  gpio_reset_pin(TM1629_CLK_GPIO);
  gpio_reset_pin(TM1629_STB_GPIO);
  return gpio_reset_pin(TM1629_DIO_GPIO) == ESP_OK ? 0 : -1;
}

static int8_t
TM1629_DirDIO_3Wire(uint8_t Dir)
{
  if (Dir)
  {
    return TM1629_SetGPIO_OUT(TM1629_DIO_GPIO);
  }

  return TM1629_SetGPIO_IN_PU(TM1629_DIO_GPIO);
}

static int8_t
TM1629_WriteDIO_3Wire(uint8_t State)
{
  return gpio_set_level(TM1629_DIO_GPIO, State) == ESP_OK ? 0 : -1;
}

static int8_t
TM1629_ReadDIO_3Wire(void)
{
  return gpio_get_level(TM1629_DIO_GPIO);
}


static int8_t
TM1629_PlatformInit_GPIO_4Wire(void)
{
  TM1629_SetGPIO_OUT(TM1629_CLK_GPIO);
  TM1629_SetGPIO_OUT(TM1629_STB_GPIO);
  TM1629_SetGPIO_OUT(TM1629_DIN_GPIO);
  return TM1629_SetGPIO_IN_PU(TM1629_DOUT_GPIO);
}

static int8_t
TM1629_PlatformDeInit_GPIO_4Wire(void)
{
  gpio_reset_pin(TM1629_CLK_GPIO);
  gpio_reset_pin(TM1629_STB_GPIO);
  gpio_reset_pin(TM1629_DIN_GPIO);
  return gpio_reset_pin(TM1629_DOUT_GPIO) == ESP_OK ? 0 : -1;
}


static int8_t
TM1629_DirDIO_4Wire(uint8_t Dir)
{
  return 0;
}

static int8_t
TM1629_WriteDIO_4Wire(uint8_t State)
{
  return gpio_set_level(TM1629_DIN_GPIO, State) == ESP_OK ? 0 : -1;
}

static int8_t
TM1629_ReadDIO_4Wire(void)
{
  return gpio_get_level(TM1629_DOUT_GPIO);
}


static int8_t
TM1629_WriteSTB(uint8_t State)
{
  return gpio_set_level(TM1629_STB_GPIO, State) == ESP_OK ? 0 : -1;
}

static int8_t
TM1629_WriteCLK(uint8_t State)
{
  return gpio_set_level(TM1629_CLK_GPIO, State) == ESP_OK ? 0 : -1;
}

static int8_t
TM1629_DelayUs(uint8_t Delay)
{
  ets_delay_us(Delay);
  return 0;
}



/**
 ==================================================================================
                            ##### Public Functions #####                           
 ==================================================================================
 */
 
/**
 * @brief  Initialize platform device to communicate TM1629 using 3-wire interface
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
TM1629_Platform_Init_GPIO_3Wire(TM1629_Handler_t *Handler)
{
  TM1629_PLATFORM_SET_COMMUNICATION(Handler, TM1629_COMMUNICATION_GPIO);
  TM1629_PLATFORM_LINK_INIT(Handler, TM1629_PlatformInit_GPIO_3Wire);
  TM1629_PLATFORM_LINK_DEINIT(Handler, TM1629_PlatformDeInit_GPIO_3Wire);
  TM1629_PLATFORM_LINK_DIR_DIO(Handler, TM1629_DirDIO_3Wire);
  TM1629_PLATFORM_LINK_WRITE_DIO(Handler, TM1629_WriteDIO_3Wire);
  TM1629_PLATFORM_LINK_READ_DIO(Handler, TM1629_ReadDIO_3Wire);
  TM1629_PLATFORM_LINK_WRITE_STB(Handler, TM1629_WriteSTB);
  TM1629_PLATFORM_LINK_WRITE_CLK(Handler, TM1629_WriteCLK);
  TM1629_PLATFORM_LINK_DELAY_US(Handler, TM1629_DelayUs);
}

/**
 * @brief  Initialize platform device to communicate TM1629 using 4-wire interface
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
TM1629_Platform_Init_GPIO_4Wire(TM1629_Handler_t *Handler)
{
  TM1629_PLATFORM_SET_COMMUNICATION(Handler, TM1629_COMMUNICATION_GPIO);
  TM1629_PLATFORM_LINK_INIT(Handler, TM1629_PlatformInit_GPIO_4Wire);
  TM1629_PLATFORM_LINK_DEINIT(Handler, TM1629_PlatformDeInit_GPIO_4Wire);
  TM1629_PLATFORM_LINK_DIR_DIO(Handler, TM1629_DirDIO_4Wire);
  TM1629_PLATFORM_LINK_WRITE_DIO(Handler, TM1629_WriteDIO_4Wire);
  TM1629_PLATFORM_LINK_READ_DIO(Handler, TM1629_ReadDIO_4Wire);
  TM1629_PLATFORM_LINK_WRITE_STB(Handler, TM1629_WriteSTB);
  TM1629_PLATFORM_LINK_WRITE_CLK(Handler, TM1629_WriteCLK);
  TM1629_PLATFORM_LINK_DELAY_US(Handler, TM1629_DelayUs);
}
