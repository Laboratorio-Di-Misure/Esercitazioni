#include <stm32f303xc.h>
#define ledsoff 9
#define period 2000 /*ms*/

unsigned short int unset,leds[3];

void TIM_Config(TIM_TypeDef*,uint32_t,uint32_t,short int);

int main(){
	
	unsigned short int i,press,valtim;  
	
	leds[0]=(1<<9);
	leds[1]=(1<<10);
	leds[2]=(1<<11);
	
	/*Attiviamo il clock al timer 2,6 e 7*/
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM6EN | RCC_APB1ENR_TIM7EN;
	/*Abilitamo il clock alle linee GPIOA e GPIOE*/
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;
	
	/*Abilitiamo il pin della porta 0 in input*/
	GPIOA->MODER &= ~(unsigned int)3;
	
	/*Abilitiamo i pin 15(che useremo per visualizzare il segnale PWM) e i pin 9-10-11-12 che useremo per programmare il segnale*/
	GPIOE->MODER |= (1<<18) | (1<<20) | (1<<22) | (1<<30);
	
	TIM_Config(TIM2,319,50000,1);
	TIM_Config(TIM6,319,12500,1);
	TIM_Config(TIM7,3999,65000,0);
	TIM7->CR1 |= TIM_CR1_OPM;
	
  /*Abilitiamo il Core a ricevere interruzioni dai TIMER 2, 6*/
	NVIC->ISER[0] |= (1<<28);
	NVIC->ISER[1] |= (1<<22);
	
	i=0;
	press=0;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM6->CR1 |= TIM_CR1_CEN;
	GPIOE->ODR |= (1<<15);
	GPIOE->ODR |= leds[i];
	
	while(1){
		if((GPIOA->IDR & GPIO_IDR_0)==GPIO_IDR_0){
			TIM7->CR1 |= TIM_CR1_CEN;
			press=1;
			valtim = TIM7->CNT;
			if(valtim>4000){
				GPIOE->ODR |= leds[0] | leds[1] | leds[2];
			}
		}else if(press==1){
			TIM7->CR1 &= ~TIM_CR1_CEN;
			valtim = TIM7->CNT;
			TIM7->CNT=0;
			if(valtim < 4000){
				GPIOE->ODR &= ~leds[i];
				i= (i+1)%3;
				GPIOE->ODR |= leds[i];
			}else{
				unset=1;
				TIM6->ARR = (500 *(i+1))*25;
				while(unset==1);
				i=0;
				GPIOE->ODR |= leds[i];
			}
			press=0;
		}
				
	}
	
	return 0;
}

/*Dato che si devono abilitare tre timer ci serviamo di una funzione creata al momento*/
void TIM_Config(TIM_TypeDef* TIM_Address,uint32_t inPSC,uint32_t inARR,short int Abilita_interrupt){

	/*Assicuriamoci che il timer sia fermo*/
	TIM_Address->CR1 &= ~TIM_CR1_CEN;
	/*Settiamo i bit ARPE, in modo da avere il registro ARR bufferizzato e il bit URS in modo da generare eventi solo in presenza di overflow*/
	TIM_Address->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;
	
	/*Impostiamo ad 010 l'MMS sul registro CR2*/	
	TIM_Address->CR2 |= (1<<5);
	TIM_Address->CR2 &= ~(unsigned int)((1<<6) | (1<<4));

	/*Se serve che invii le interrupt ne abilitiamo la richiesta*/
	if(Abilita_interrupt==1){
		TIM_Address->DIER |= TIM_DIER_UIE;
	}

	/*Carichiamo i valori scelti nel prescalar e nell arrival register*/
	TIM_Address->PSC = inPSC;
	TIM_Address->ARR = inARR;
	
	/*Azzeriamo il contatore*/
	TIM_Address->CNT = 0;
	
	/*Generiamo un update event da software in modo da aggiornare i valori dei registri PSC e ARR*/
	TIM_Address->EGR |= TIM_EGR_UG;
	
	/*Ci assicuriamo che l'update interrupt flag sia abbassato in modo da essere pronti a cogliere la prossima interrupt*/
	TIM_Address->SR &= ~TIM_SR_UIF;
	
	return;
}

void TIM2_IRQHandler(){
	/*Avvia il conteggio sul periodo del segnale e accende il led che lo visualizza*/
	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->CNT=0;
	TIM6->CNT=0;
	GPIOE->ODR |= (1<<15);
	return;
}
	
void TIM6_DAC_IRQHandler(){
	/*Ferma il timer 6 e ne azzera l'interrupt*/
	TIM6->SR &= ~TIM_SR_UIF;
	TIM6->CR1 &= ~TIM_CR1_CEN;
	/*Se il led è acceso lo spegne in quanto il periodo di segnale altro è terminato*/
	if((GPIOE->ODR & (1<<15))==(1<<15)){
		GPIOE->ODR &= ~(unsigned int)(1<<15);
	}else{ /*Altriemnti riavviamo il timer(tenere il timer sempre in moto aiuta ad evitare di perdere tempo quando si d3eve ricminciare il cilco sulla interrupt del TIM2)*/
		TIM6->CR1 |= TIM_CR1_CEN;
		return;
	}
	TIM6->CR1 |= TIM_CR1_CEN;
	/*nel caso in cui sia stato cambiato il valore del pwm alto dobbiamo segnalare all'utente che è stato caricato il nuovo valore spegnendo i led che si erano accesi*/
	if(unset==1){
		GPIOE->ODR &= ~(leds[0] | leds[1] | leds[2]);
		unset=0;
	}
	
	return;
}	
