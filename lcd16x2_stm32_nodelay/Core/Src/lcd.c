/*
 * this software is for running Character LCD 16x2 with stm32f103c8 without delay
 * by using timer TIM1 , DMA1_CH3
 * PA0 to PA7 : LCD data pins
 * PA8 : LCD enable pin
 * PA9 : LCD RS Pin
 *
 * this file needs a millisecond delay function for LCD initialization ,
 * here i used HAL_Delay()
 *
 *
 * launch :
 * 1.include lcd.h in main.c
 * 2.call lcd_init() in main function
 * 3.use lcd_Set_cursor() and lcd_print() to print to lcd
 *
 *
 * Licenses : CC BY-NC
 * elcen.ir
 *
 */


#include "lcd.h"

#define __SEND_DATA 	GPIOA->BSRR = GPIO_BSRR_BS9
#define __SEND_COMMAND 	GPIOA->BSRR = GPIO_BSRR_BR9

static uint32_t byte2write[32];

void lcd_command(uint8_t command);

void lcd_peripheral_init(void){

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	//8 data pins PA0 to PA7 all configured output PP
	GPIOA->CRL = 0x22222222;

	//PA9 is GPIO for RS pin
	GPIOA->CRH &= ~( GPIO_CRH_CNF9 | GPIO_CRH_MODE9 );
	GPIOA->CRH |= GPIO_CRH_MODE9_1;

	//PA8 for E pin : TIM1_CH1
	GPIOA->CRH &= ~(GPIO_CRH_MODE8|GPIO_CRH_CNF8);
	GPIOA->CRH |= GPIO_CRH_MODE8_1|GPIO_CRH_CNF8_1;


	//TIM1 Configuration
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	TIM1->CR1 &= ~(TIM_CR1_CKD|TIM_CR1_CMS|TIM_CR1_DIR);
	TIM1->CR1 |= TIM_CR1_OPM;

	TIM1->ARR = (uint16_t)59;
	TIM1->PSC = (uint16_t)71;

	TIM1->CCR1 = (uint16_t)15;
	TIM1->CCR2 = (uint16_t)3;

	TIM1->CCMR1 |= TIM_CCMR1_CC1S;
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
	TIM1->CCMR1 &= ~TIM_CCMR1_CC1S;

	TIM1->CCMR1 &= ~TIM_CCMR1_CC2S;
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM1->CCMR1 |= ( TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 );
	TIM1->CCMR1 &= ~TIM_CCMR1_OC2M;

	TIM1->CCER &= ~TIM_CCER_CC1P;
	TIM1->CCER |= ( TIM_CCER_CC1E | TIM_CCER_CC2E);

	TIM1->BDTR |= TIM_BDTR_MOE;

	TIM1->DIER |= TIM_DIER_CC2DE;

	//DMA1_CH3 to put a byte on PA0 to PA7 each time Compare happens on TIM1_CH2
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	DMA1_Channel3->CPAR = (uint32_t)&GPIOA->BSRR;
	DMA1_Channel3->CMAR = (uint32_t)byte2write;
	DMA1_Channel3->CCR = 0;
	DMA1_Channel3->CCR |= (DMA_CCR_PSIZE_1|DMA_CCR_MSIZE_1|DMA_CCR_DIR|DMA_CCR_MINC);


}

void lcd_init(void){
	lcd_peripheral_init();
	HAL_Delay(20);
	lcd_command(0x30);
	HAL_Delay(5);
	lcd_command(0x30);
	HAL_Delay(5);
	lcd_command(0x30);
	HAL_Delay(1);

	lcd_command(LCD_FUNCTIONSET|LCD_FUNCTION_N|LCD_FUNCTION_DL);
	lcd_command(LCD_DISPLAYCONTROL|LCD_DISPLAY_D);
	lcd_command(LCD_CLEARDISPLAY);
	HAL_Delay(5);

}


void data_prepare(uint8_t * byte,uint8_t size){

	for(uint8_t i=0;i<size;i++)
		byte2write[i]= (uint32_t)byte[i] | (~((uint32_t)byte[i]<<16) & 0xFF0000);


}

void lcd_print(uint8_t * string,uint8_t size){

	while(TIM1->CR1 & TIM_CR1_CEN){}

	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1_Channel3->CNDTR = size;

	__SEND_DATA;

	data_prepare(string, size);

	TIM1->RCR = size-1;
	TIM1->DIER &= ~TIM_DIER_CC2IE;
	TIM1->EGR = TIM_EGR_UG;
	TIM1->SR = ~TIM_SR_CC2IF;
	TIM1->DIER |= TIM_DIER_CC2IE;

	DMA1_Channel3->CCR |= DMA_CCR_EN;
	TIM1->CR1 |= TIM_CR1_CEN;
}


void lcd_command(uint8_t command){

	while(TIM1->CR1 & TIM_CR1_CEN);

	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1_Channel3->CNDTR = 1;

	__SEND_COMMAND;

	byte2write[0]= (uint32_t)command | (~((uint32_t)command<<16) & 0xFF0000);

	TIM1->RCR = 0;

	TIM1->DIER &= ~TIM_DIER_CC2IE;
	TIM1->EGR = TIM_EGR_UG;
	TIM1->SR = ~TIM_SR_CC2IF;
	TIM1->DIER |= TIM_DIER_CC2IE;

	DMA1_Channel3->CCR |= DMA_CCR_EN;
	TIM1->CR1 |= TIM_CR1_CEN;
}




void lcd_Set_cursor(uint8_t row,uint8_t col){

	switch(row){
	case 1:
		lcd_command((col-1)|0x80);
		break;

	case 2:
		lcd_command((col-1)|0x80|0x40);
		break;
	}
}






