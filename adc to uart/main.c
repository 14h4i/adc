#include "stm32f10x.h"
#include "stdio.h"


void Delay1Ms(void);
void Delay_Ms(uint32_t u32DelayInMs);
int main(void)
{
	GPIO_InitTypeDef gpioInit;
	USART_InitTypeDef uartInit;
	ADC_InitTypeDef adcInit;
	TIM_TimeBaseInitTypeDef timerInit;
	float value_adc1=0,sum_adc1=0,adc_tb=0;

	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//bat Clock Port B
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		//bat Clock Usart3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	
	timerInit.TIM_CounterMode = TIM_CounterMode_Up;			//Che do dem len
	timerInit.TIM_Period = 0xFFFF;											//Gia tri tai lai
	timerInit.TIM_Prescaler = 72 - 1;										//Gia tri chia dem, 72/72+1 -> 72 -1 , moi bo dem bang 1ms
	
	TIM_TimeBaseInit(TIM2, &timerInit);
	TIM_Cmd(TIM2,ENABLE);
	
	gpioInit.GPIO_Pin = GPIO_Pin_9 ;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &gpioInit);
	
	/* PA0 */
	gpioInit.GPIO_Mode = GPIO_Mode_AIN;
	gpioInit.GPIO_Pin = GPIO_Pin_0;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &gpioInit);
	
	
	
	/* PB10 TX */
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;											//Cau hinh chan ra AF PushPull
	gpioInit.GPIO_Pin = GPIO_Pin_2;													//Pin 10
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;										//Toc do 50MHz
	
	GPIO_Init(GPIOA, &gpioInit);															
	
	/* PB11 RX */
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;								//Cau hinh chan ra AF PushPull
	gpioInit.GPIO_Pin = GPIO_Pin_3;													

	
	GPIO_Init(GPIOA, &gpioInit);													
	
	/* Init Uart */
	uartInit.USART_BaudRate = 115200;														//Toc do truyen
	uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//OFF
	uartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//Cau hinh che do vua truyen vua nhan
	uartInit.USART_Parity = USART_Parity_No;									//Off kiem tra chan le
	uartInit.USART_StopBits = USART_StopBits_1;								//cau hinh bit stop =1
	uartInit.USART_WordLength = USART_WordLength_8b;					//8 bit du lieu
	USART_Init(USART2, &uartInit);			
	USART_Cmd(USART2, ENABLE);		//Khoi tao Usart voi cau hinh tren
	
	/* Init ADC */
	adcInit.ADC_ContinuousConvMode = ENABLE;
	adcInit.ADC_DataAlign = ADC_DataAlign_Right;
	adcInit.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adcInit.ADC_Mode = ADC_Mode_Independent;
	adcInit.ADC_NbrOfChannel = 1;
	adcInit.ADC_ScanConvMode = DISABLE;
	
	ADC_Init(ADC1,&adcInit);
	/* Cau hinh chanel, rank, time, calib */
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);	//Cau hinh chanel,rank,time,calib
	ADC_Cmd(ADC1,ENABLE);													//Cho phep ADC hoat dong
	ADC_TempSensorVrefintCmd(ENABLE);							//Cho phep cam bien nhiet hoat dong
	ADC_ResetCalibration(ADC1);										//RESET thanh ghi cablib	
	while (ADC_GetResetCalibrationStatus(ADC1));	//Cho thghi cablib reset xong
	ADC_StartCalibration(ADC1);										//Khoi dong bo ADC
	while (ADC_GetCalibrationStatus(ADC1));				//Cho trang thai cablib dc bat
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);			//Bat chuyen doi ADC
	
	/* Enabale */
															//Bat Usart 3
	

	
	
	
	sum_adc1 = 0;
	adc_tb = 0;
	while (1) 
	{
		for (int i=0;i<10;i++)
		{
			value_adc1 = ADC_GetConversionValue(ADC1);
			sum_adc1 = sum_adc1 + value_adc1;
			Delay_Ms(1);
		}
		adc_tb = sum_adc1/10;
		sum_adc1 =0;
		//printf("Gia tri ADC là: %4.3f\n",adc_tb);
		USART_SendData(USART2,adc_tb);
		Delay_Ms(1000);

		
		
		
	

	}

}
void Delay1Ms(void)
{
	
	TIM_SetCounter(TIM2, 0);								//Set TIM2 tai 0
	while (TIM_GetCounter(TIM2) < 1000) {		//Khi gia tri TIM2 < 1ms
	}
}

void Delay_Ms(uint32_t u32DelayInMs)
{
	
	while (u32DelayInMs) {
		Delay1Ms();
		--u32DelayInMs;
	}
}
