/*
 * Update_Graph.c
 *
 *  Created on: Aug 18, 2021
 *      Author: Mahdi
 */

#include "main.h"
#include <stdlib.h>
#include <string.h>
#include "cmsis_os.h"


extern __IO uint8_t End_Of_Capture;
extern __IO uint16_t ADC_Buffer[buff_len2];
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern DMA_HandleTypeDef hdma_adc1;
//extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim10;
extern __IO uint32_t Buffer_Addr_At_Trigger;
extern uint32_t value_dac;
extern uint16_t buff2[buff_len2];
extern __IO uint8_t Data_Is_Updated;
extern void All_Init();
__IO uint8_t delay_before_EXTI = 15;
__IO uint32_t BufferAddressForLCD = 0;
extern __IO uint16_t LCDgraphBuffer[320];

static uint16_t _UpdateAddress(signed int temp_address) {
    if (temp_address >= buff_len2)
        temp_address = temp_address - buff_len2;
    else if (temp_address < 0)
        temp_address = temp_address + buff_len2;
    return temp_address;
}

void Update_Graph_Content () {
//	while (1)
//	{
//		if (End_Of_Capture) {
//			Buffer_Addr_At_Trigger = buff_len - 1 - Buffer_Addr_At_Trigger;
//			End_Of_Capture = 0;
//			if (Data_Is_Updated == 0) { //if the flag was cleared
//				for (int i = 0; i < 320; i++) //buffer for LCD
//					LCDgraphBuffer[i] = ADC_Buffer[_UpdateAddress(Buffer_Addr_At_Trigger*2+i-160)];
//			}
//			osDelay(delay_before_EXTI);
//			Data_Is_Updated = 1;
//			memset(ADC_Buffer, 0, buff_len2 * sizeof(unsigned short));
//			All_Init();
//			HAL_ADC_Start(&hadc3);
//			HAL_ADC_Start(&hadc2);
//			HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t *)ADC_Buffer, buff_len);
//			osDelay(1);
//			EXTI->IMR |= (1U<<2);
//		} else
//			osDelay(3);
//	}
}
