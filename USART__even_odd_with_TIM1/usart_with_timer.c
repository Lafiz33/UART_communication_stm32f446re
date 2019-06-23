#include "stm32f446XX.h"
#include<string.h>

int number[255];
int txBuffer[255];
uint16_t txBufferCounter2=0;
uint16_t txBufferCounter3=0;

int rxBuffer[255];
uint16_t rxBufferCounter2=0;
uint16_t rxBufferCounter3=0;

int rxFlag2=0;
int txFlag2=0;
int rxFlag3=0;
int txFlag3=0;

int overflowCounter;

void start_USART1_USART3(void);

void enable_clocks(void);
void init_control_USART3(void);
void init_gpio_USART3(void);
void enable_interrupt_USART3(void);

void init_control_USART1(void);
void init_gpio_USART1(void);
void enable_interrupt_USART1(void);

void initialize_tim1();
void timerDelayInSecond(int time);

void txeEnableUSART1(void);
void txeDisableUSART1(void);
void rxneEnableUSART1(void);
void rxneDisableUSART1(void);

void txeEnableUSART3(void);
void txeDisableUSART3(void);
void rxneEnableUSART3(void);
void rxneDisableUSART3(void);

void handleTxeUSART1();
void handleRxneUSART1();

void handleTxeUSART3();
void handleRxneUSART3();

int getAscii(int a);
void send_odd();
void send_even();

void turnLedOn(void);
void turnLedOff(void);
void blink(int a);
void delay(int delay);

int main(){
	start_USART1_USART3();
	
	//loop
	while(1){
		send_even();
	}
}


/**********************************************
 * FUNCTION: USART3_IRQHandler
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               checks status register to see which interrupt occured
 *
 * AUTHOR:    shamrat apu gazi
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void USART3_IRQHandler(void){
	NVIC_ClearPendingIRQ(USART3_IRQn);
	//if TXE occurs then it will set TXE flag 1
	if((USART3->SR & 0x80)!=0){
		if(txFlag3==1){
			handleTxeUSART3();
		}
	}
	
	//if TC occurs
	if((USART3->SR & 0x40)!=0){
		txeDisableUSART3();
		//disable TC interrupt of USART3
		USART3->CR1 &= ~(USART_CR1_TCIE);
	}
	
	
	//if RXNE occurs it will set rxflag 1
	if((USART3->SR & 0x20)!=0){
		handleRxneUSART3();
		}
}

/**********************************************
 * FUNCTION: USART1_IRQHandler
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               checks status register to see which interrupt occured
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void USART1_IRQHandler(void){
	
	//NVIC_DisableIRQ(USART1_IRQn);
	NVIC_ClearPendingIRQ(USART1_IRQn);
	
	
	//if TXE occurs 
	if((USART1->SR & 0x80)!=0){
		if(txFlag2==1){
		handleTxeUSART1();
		}
	}
	
	//if TC occurs
	if((USART1->SR & 0x40)!=0){
		txeDisableUSART1();
		//disable TC interrupt in USART1
		USART1->CR1 &= ~(USART_CR1_TCIE);
	}
	
	
	//if RXNE occurs 
	if((USART1->SR & 0x20)!=0){
		//blink(1);
		handleRxneUSART1();
	}
	
}

/**********************************************
 * FUNCTION: handleTxeUSART1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               if txe interrupt occurs in USART1 then it will transmit the data
 *	after tansmission it will disable the interrupt
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void handleTxeUSART1(){
	txBufferCounter2=0;
	//USART1->DR=(txBuffer[txBufferCounter2++] & 0x00FF);
	USART1->DR=(txBuffer[txBufferCounter2] & 0x00FF);
	txFlag2=0;
	txeDisableUSART1();
	USART1->CR1 |= USART_CR1_TCIE;
	if(txBufferCounter2==14){
		txBufferCounter2=0;
		txFlag2=0;
		txeDisableUSART1();
		USART1->CR1 |= USART_CR1_TCIE;
	}
}

/**********************************************
 * FUNCTION: handleRxneUSART1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               receives the data from USART1 then enables TXE for transmission
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void handleRxneUSART1(){
	rxBufferCounter2=0;
	rxBuffer[rxBufferCounter2++]=(USART1->DR & 0x00FF);
	txFlag3=1;
	txeEnableUSART3();
	if(rxBufferCounter2==14){
		rxBufferCounter2=0;
		txFlag3=1;
		txeEnableUSART3();
	}
}

/**********************************************
 * FUNCTION: handleTxeUSART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *              if txe interrupt occurs in USART1 then it will transmit the data
 *	after tansmission it will disable the interrupt
 *
 *
 * AUTHOR:    shamrat apu gazi
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void handleTxeUSART3(){
	txBufferCounter3=0;
	USART3->DR=(rxBuffer[txBufferCounter3++] & 0x00FF);
	txFlag3=0;
	txeDisableUSART3();
	USART3->CR1 |= USART_CR1_TCIE;
	if(txBufferCounter3==3){
		txBufferCounter3=0;
		txeDisableUSART3();
		USART3->CR1 |= USART_CR1_TCIE;
	}
}
/**********************************************
 * FUNCTION: handleRxneUSART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *              receives the data from USART1 then enables TXE for transmission
 *
 *
 * AUTHOR:    shamrat apu gazi
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void handleRxneUSART3(){
	rxBufferCounter3=0;
	txBuffer[rxBufferCounter3++]=(USART3->DR & 0x00FF);
	txFlag2=1;
	txeEnableUSART1();
	if(rxBufferCounter3==3){
		rxBufferCounter3=0;
		txFlag2=1;
		txeEnableUSART1();
		
	}
}


/**********************************************
 * FUNCTION: USART1_USART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               calls all the initialization funtions
 *
 *
 * AUTHOR:    Abdur Rhaman Pappu
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void start_USART1_USART3(void){
	enable_clocks();
	initialize_tim1();
	init_control_USART3();
	init_gpio_USART3();
	enable_interrupt_USART3();
	
	init_control_USART1();
	init_gpio_USART1();
	enable_interrupt_USART1();
}


/**********************************************
 * FUNCTION: enable_clocks
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               enables necessery functions
 *
 *
 * AUTHOR:    Abdur Rhaman Pappu
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void enable_clocks(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
}



/**********************************************
 * FUNCTION: init_control_USART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               configures control registers and baud rate for USART3
 *
 *
 * AUTHOR:    Abdur Rhaman Pappu
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void init_control_USART3(void){
	//enable USART3 with interrupts
	USART3->SR = 0x0;
	USART3->CR1 = 0x00;
	USART3->BRR = 0x8A;
	USART3->CR2 = 0x00;
	USART3->CR3 = 0x00;
	USART3->CR1 = 0x200C; //0x20EC;
}

/**********************************************
 * FUNCTION: init_gpio_USART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               sets up gpio pins for usart3 and PA6 for LED
 *
 *
 * AUTHOR:    Abdur Rhaman Pappu
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void init_gpio_USART3(void){
	//LED PA6
	GPIOA->MODER &= ~(3<<12);
	GPIOA->MODER |= (1<<12);
	GPIOA->OTYPER &=~(1<<6);
	GPIOA->OSPEEDR &=~ (3<<12);
	GPIOA->OSPEEDR |= (3<<12);
	GPIOA->PUPDR &=~ (3<<12);
	
	
	//PB10 for TX USART3
	GPIOB->MODER &=~ (3<<20);
	GPIOB->MODER |= (2<<20);
	GPIOB->OTYPER &=~(1<<10);
	GPIOB->OSPEEDR &=~ (3<<20);
	GPIOB->OSPEEDR |= (3<<20);
	GPIOB->PUPDR &=~ (3<<20);
	GPIOB->AFR[1] |= (7<<8);
	
	
	
	//PC5 for RX USART3
	GPIOC->MODER &=~ (3<<10);
	GPIOC->MODER |= (2<<10);
	GPIOC->OTYPER &=~(1<<5);
	GPIOC->OSPEEDR &=~ (3<<10);
	GPIOC->OSPEEDR |= (3<<10);
	GPIOC->PUPDR &=~ (3<<10);
	GPIOC->AFR[0] |= (7<<20);
}

/**********************************************
 * FUNCTION: nable_interrupt_USART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               configure interrupts for USART3
 *
 *
 * AUTHOR:    Abdur Rhaman Pappu
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void enable_interrupt_USART3(void){
	
	NVIC_EnableIRQ(USART3_IRQn);
	NVIC_ClearPendingIRQ(USART3_IRQn);
	
	USART3->CR1 |= USART_CR1_RXNEIE;
	USART3->CR1 &= ~USART_CR1_TXEIE;
}


/**********************************************
 * FUNCTION: init_control_USART1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               configures control registers and baud rate for USART1
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void init_control_USART1(void){
	
	//enable USART1 with interrupts
	USART1->SR = 0x0;
	USART1->CR1 =0x00;
	USART1->BRR = 0x8A;
	USART1->CR2 = 0x00;
	USART1->CR3 = 0x00;
	USART1->CR1 = 0x200C; //0x20EC;
}

/**********************************************
 * FUNCTION: init_gpio_USART1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               sets up gpio pins for usart1 and PA5 for LED
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void init_gpio_USART1(void){
	
	//LED PA5
	GPIOA->MODER &= ~(3<<10);
	GPIOA->MODER |= (1<<10);
	GPIOA->OTYPER &=~(1<<5);
	GPIOA->OSPEEDR &=~ (3<<10);
	GPIOA->OSPEEDR |= (3<<10);
	GPIOA->PUPDR &=~ (3<<10);
	
	
	//PB6 for TX USART1
	GPIOB->MODER &=~ (3<<12);
	GPIOB->MODER |= (2<<12);
	GPIOB->OTYPER &=~(1<<6);
	GPIOB->OSPEEDR &=~ (3<<12);
	GPIOB->OSPEEDR |= (3<<12);
	GPIOB->PUPDR &=~ (3<<12);
	GPIOB->AFR[0] |= (7<<24);
	
	
	
	//PB7 for RX USART1
	GPIOB->MODER &=~ (3<<14);
	GPIOB->MODER |= (2<<14);
	GPIOB->OTYPER &=~(1<<7);
	GPIOB->OSPEEDR &=~ (3<<14);
	GPIOB->OSPEEDR |= (3<<14);
	GPIOB->PUPDR &=~ (3<<14);
	GPIOB->AFR[0] |= (7<<28);

}

/**********************************************
 * FUNCTION: enable_interrupt_USART1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               configure interrupts for USART1
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void enable_interrupt_USART1(void){
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_ClearPendingIRQ(USART1_IRQn);
	
	USART1->CR1 |= USART_CR1_RXNEIE;
	USART1->CR1 &= ~USART_CR1_TXEIE;
}





/**********************************************
 * FUNCTION: txeEnableUSART1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               enables TXE for USART1
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void txeEnableUSART1(){
	USART1->CR1 |= USART_CR1_TXEIE;
}
/**********************************************
 * FUNCTION: txeDisableUSART1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               disables TXE for USART1
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void txeDisableUSART1(){
	USART1->CR1 &= ~USART_CR1_TXEIE;
}
/**********************************************
 * FUNCTION: rxneEnableUSART1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               enables RXNE for USART1
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void rxneEnableUSART1(){
	USART1->CR1 |= USART_CR1_RXNEIE;
}
/**********************************************
 * FUNCTION: rxneDisableUSART1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               disables RXNE for USART1
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void rxneDisableUSART1(){
	USART1->CR1 &= ~USART_CR1_RXNEIE;
}

/**********************************************
 * FUNCTION: txeEnableUSART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               enables TXE for USART3
 *
 *
 * AUTHOR:    shamrat apu gazi
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void txeEnableUSART3(){
	USART3->CR1 |= USART_CR1_TXEIE;
}
/**********************************************
 * FUNCTION: txeDisableUSART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               disbles TXE for USART3
 *
 *
 * AUTHOR:    shamrat apu gazi
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void txeDisableUSART3(){
	USART3->CR1 &= ~USART_CR1_TXEIE;
}
/**********************************************
 * FUNCTION: rxneEnableUSART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               enbles RXNE for USART3
 *
 *
 * AUTHOR:    shamrat apu gazi
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void rxneEnableUSART3(){
	USART3->CR1 |= USART_CR1_RXNEIE;
}
/**********************************************
 * FUNCTION: rxneDisableUSART3
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               disbles RXNE for USART3
 *
 *
 * AUTHOR:    shamrat apu gazi
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void rxneDisableUSART3(){
	USART3->CR1 &= ~USART_CR1_RXNEIE;
}








/**********************************************
 * FUNCTION: initialize_tim1
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               sets up TIM1
 *
 *
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/ 
void initialize_tim1(){
	//ENABLE peripheral clock for TIM1	EN
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; //enable timer for delay
	//disable tim1
	TIM1->CR1 &= ~(TIM_CR1_CEN);
	//clear SR
	TIM1->SR = 0x0;
	//set the prescaler
	TIM1->PSC = 16000-1;
	//set auto reload
	TIM1->ARR = (100-1);
	//enable counter
	TIM1->CR1 |= (TIM_CR1_CEN);
	//enable timer interrupt in DIER
	TIM1->DIER |=(1<<0);
	
	//enable interrupt
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

/**********************************************
 * FUNCTION: TIM1_UP_TIM10_IRQHandler
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               handler for TIM1 up counter
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void TIM1_UP_TIM10_IRQHandler(){
	if((TIM1->SR & TIM_SR_UIF)!=0){
		overflowCounter++;
	}
	TIM1->SR &= ~TIM_SR_UIF;
}

/**********************************************
 * FUNCTION: timerDelayInSecond
 *
 * INPUT PARAMETERS : int
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               counter for TIM1, takes int and gives delays in second
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/ 
void timerDelayInSecond(int time){
		overflowCounter=0;
	while(overflowCounter != time);
	
}


void send_odd(){
	int c[3];
	int counter=1;
	while(counter<255){
	if(counter<=9){
				c[0]=counter;
			int i;
			for(i=0; i<1; i++){
				while(((USART3->SR & 0x80)==0));
				USART3->DR=getAscii(c[i]);
				timerDelayInSecond(1);
			counter=counter+2;
			
		}
	}
			else if(counter<=99){
				int a=counter;
				c[1]=a%10;
				a=a/10;
				c[0]=a;
				int i;
			for(i=0; i<2; i++){
				while(((USART3->SR & 0x80)==0));
				USART3->DR=getAscii(c[i]);
				timerDelayInSecond(1);
			}
			counter=counter+2;
	}
			else if(counter<255){
				int a=counter;
				//int b=a%100;
				c[2]=a%10;
				a=a/10;
				c[1]=a%10;
				a=a/10;
				c[0]=a%10;
				int i;
			for(i=0; i<3; i++){
				while(((USART3->SR & 0x80)==0));
				USART3->DR=getAscii(c[i]);
				timerDelayInSecond(1);
			}
			counter=counter+2;
			
		
	}
			else{
				counter=0;
			}
			while(((USART3->SR & 0x80)==0)){}
				USART3->DR=' ';
				timerDelayInSecond(1);
		}
}
void send_even(){
	int c[3];
	int counter=0;
	while(counter<255){
	if(counter<=9){
				c[0]=counter;
			int i;
			for(i=0; i<1; i++){
				while(((USART3->SR & 0x80)==0));
				USART3->DR=getAscii(c[i]);
				timerDelayInSecond(1);
			counter=counter+2;
			
		}
	}
			else if(counter<=99){
				int a=counter;
				c[1]=a%10;
				a=a/10;
				c[0]=a;
				int i;
			for(i=0; i<2; i++){
				while(((USART3->SR & 0x80)==0));
				USART3->DR=getAscii(c[i]);
				timerDelayInSecond(1);
			}
			counter=counter+2;
	}
			else if(counter<255){
				int a=counter;
				//int b=a%100;
				c[2]=a%10;
				a=a/10;
				c[1]=a%10;
				a=a/10;
				c[0]=a%10;
				int i;
			for(i=0; i<3; i++){
				while(((USART3->SR & 0x80)==0));
				USART3->DR=getAscii(c[i]);
				timerDelayInSecond(1);
			}
			counter=counter+2;
			
		
	}
			else{
				counter=0;
			}
			while(((USART3->SR & 0x80)==0)){}
				USART3->DR=' ';
				timerDelayInSecond(1);
		}
}

int getAscii(int a){
	int ascii=48;
	return ascii+a;
	
}

/**********************************************
 * FUNCTION: blink
 *
 * INPUT PARAMETERS : int
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               blinks LED for desired time
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void blink(int a){
	turnLedOn();
	delay(a);
	turnLedOff();
	delay(a);
}
/**********************************************
 * FUNCTION: turnLedOn
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *              turns on LED
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void turnLedOn(void){
		GPIOA->BSRR |= (1<<5);
}
/**********************************************
 * FUNCTION: turnLedOff
 *
 * INPUT PARAMETERS : none
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               turns off LED
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void turnLedOff(void){
	  GPIOA->BSRR |= (1<<(5+16));
}
/**********************************************
 * FUNCTION: delay
 *
 * INPUT PARAMETERS : int
 *
 * RETURN VALUE: none
 *
 * DESCRIPTION
 *               simple delay with loops
 *
 *
 * AUTHOR:    Lafiz Maruf
 * UPDATED:  06 April 2019
 *
 ***********************************************/
void delay(int delay){
	for(int i=0; i<delay*1000000; i++);
}


