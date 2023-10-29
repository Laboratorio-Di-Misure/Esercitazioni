/*Le interruzioni su questo dispositivo hanno dei nomi standard a seconda della linea su vui arrivano, per poter realizzare una ISR personalizzata si deve creare una funzione void, senza parametri
e con lo stesso nome, questo nome si trova nella tabella interrupt vectors pag 183 RefMan. Si dovra poi abilitare la periferica a richiedere l' interrupt(mediante la modifica di un apposito bit
in un registro della periferica (Il DIER per i Timer) e il processore ad accettarla e gestirla settando l'apposito valore a 1 (il valore da settare a 1 è quello corrispondente al numero
dell'interruzione nella tabella citata prima) nei registri NVIC->ISER*/


#include <stm32f303xc.h>
#define ledoff 8

unsigned short int cont;

int main(){

	
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;
	
	GPIOA->MODER &= ~(unsigned int)3;
	GPIOE->MODER |= (1<<16) | (1<<18) | (1<<20) | (1<<22) | (1<<24) | (1<<26) | (1<<28) | (1<<30);
	
	TIM7->CR1 |= TIM_CR1_URS | TIM_CR1_OPM | TIM_CR1_ARPE;
	TIM7->CR1 &= ~TIM_CR1_CEN;
	
	TIM7->CR2 &= ~(unsigned int)((1<<6)|(1<<4));
	TIM7->CR2 |= (1<<5);
	
	/*Effettuiamo una riduzione di 8000 volte, ovvero un incremento ogni ms*/
	TIM7->PSC= 7999;
	
	/*Vogliamo interrompere ogni secondo*/
	TIM7->ARR= 2000;
	
	TIM7->EGR |= TIM_EGR_UG;
	
	TIM7->SR &= ~TIM_SR_UIF;
	
	/*Abilitiamo il timer 7 a poter fare richiesta di interruzioni quando si verifica l'overflow*/
	/*Per poterlo fare bisogna abilitare il bit UIE (Update interrupt enable) sul registro DIER (pag 596 RefMan)*/
	TIM7->DIER |= (1<<0);
	
	/*Abilitiamo anche il core a ricevere e gestire l' eccezione dal TIM7*/
	/*I registri ISER sono 4 (ogniuno da 32 bit, in totale deve avere 88 bit per regolare tutte le possibili interrupt). Tali registri si possono accedere come degli array*/
	/*ad esempio, se,come in questo caso, ho bisogno di abilitare l'interrupt n°62 farò 55-32=23 */
	NVIC->ISER[1] |= (1<<23);
	
	cont=0;
	TIM7->CR1 |= TIM_CR1_CEN;
	
	while(1){
		
	}
	
	return 0;
}

void TIM7_IRQHandler(){
	/*La gestione delle interruzioni va sempre fatta con una ISR quanto più breve possibile*/
	TIM7->SR &= ~TIM_SR_UIF;
	GPIOE->ODR &= ~(unsigned int)(cont<<ledoff);
	cont++;
	GPIOE->ODR |= (unsigned int)(cont<<ledoff);
	TIM7->CNT=0;
	TIM7->CR1 |= TIM_CR1_CEN;
	/*In questo caso semplicemente, qui è prensente una serie di istruzioni che all'arrivo del'interruzione incrementano il contatore*/	
	return;
}


	