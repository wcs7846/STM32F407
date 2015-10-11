#ifndef __RTC_H
#define __RTC_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "I2C.h"
#include "OLED_I2C.h"
#include "delay.h"
/*----------------------------------------------------------------------------*/
void RTC_Config(void);
//�����ʾʱ��
void RTC_TimeShow(uint8_t* Time);
void RTC_DateShow(uint8_t* Date);
void RTC_AlarmShow(void);
//����ʱ��
void RTC_SetTime(uint16_t SetHour,uint16_t SetMinture,uint16_t SetSecond);
void RTC_SetDate(uint16_t SetYear,uint16_t SetMonth,uint16_t SetDay);
//�ص�����
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
#endif
