/* Includes ------------------------------------------------------------------*/
#include "delay.h"
/*----------------------------------------------------------------------------*/
volatile uint8_t delayOK;

void Delay_us(uint32_t i)
{
	//��ѯ��
//	uint8_t temp;
//	TIM3->ARR = (i);
//	TIM3->CNT = 0x00;
//	HAL_TIM_Base_Start(&htim3);
//	do 
//	{
//		temp = TIM3->CNT;
//	}while(temp<(i));
//	HAL_TIM_Base_Stop(&htim3);
	//�жϷ�
	htim3.Instance->ARR = i-2;
	HAL_TIM_Base_Start(&htim3);
	delayOK = 0;
	while(0==delayOK);
};
//�����Ҫ��ʱ1ms��ֱ��ʹ��HAL_Delay��������
//Ҳ������Delay_us�������±�дһ��

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim3)
		delayOK = 1;
	HAL_TIM_Base_Stop(&htim3);
}
