#include <stm32f303xc.h>
#define UOT 5 /*unit of time*/
int main(){
	unsigned int crono;
	unsigned short int start;
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER &= ~GPIO_MODER_MODER0;
	
	TIM6->CR1 |= TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_URS;
	TIM6->CR1 &= ~TIM_CR1_CEN;	
	
	/*Incrementiamo di 1 ogni 5ms,in totale possiamo contare 327.672 ms (327 minuti)*/
	TIM6->PSC = 40000;
	
	/*Settiamo il valore di overflow a 60000 in modo da contare massimo esattamente  min*/
	TIM6->ARR= 60000;
	
	TIM6->CR2 |= (1<<5);
	TIM6->CNT=0;
	TIM6->SR &= ~TIM_SR_UIF;
	
	TIM6->EGR |= TIM_EGR_UG;
	
	crono=0;
	start=0;
	while(1){
		if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0){ /*Verifichiamo che il pulsante sia stato premuto*/
			if(start==0){  					/*Se non era già stato premuto allora resettiamo e avviamo il contatore*/
				TIM6->CNT =0;
				crono =0;
				TIM6->CR1 |= TIM_CR1_CEN;
				start =1;
			}else if(start==2){    /*Se il pulsante era stato premuto, rilasciato e poi ripremuto allora fermo il timer e carico il valore in una variabile*/
				TIM6->CR1 &= ~TIM_CR1_CEN;
				crono= TIM6->CNT * UOT;
				if((TIM6->SR & TIM_SR_UIF)==TIM_SR_UIF){ /*Se sono andato in Overflow allora lo segnalo caricando nel bit più significativo della stringa un uno*/
					crono |= (unsigned int)(1<<31);
				}
				TIM6->SR &= ~TIM_SR_UIF;
				TIM6->CNT=0;
				start=0;
			}
		}else if(start==1){ /*Se il pulsante e stato premuto e poi rilasciato impostiamo start a 2 in modo che alla prossima pressione si ricordi che è già stato premuto una volta*/
			start=2;
		}
	}
	return 0;
}
