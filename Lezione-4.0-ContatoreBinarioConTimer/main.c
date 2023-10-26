#include <stm32f302xc.h>

#define ledoff 8

/*I timer sono delle periferiche che permettono di avere una base dei tempi oppure un cronometro, in modo da poter inserire delle attese più precise di un for generico e avere
*modo di temporizzare le operazioni*/

int main(){
	unsigned short int start,cont;
	/*Abilitiamo il clock alle due porte GPIO*/
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;

	/*Abilitamo il clock al timer TIM6 che si trova sul bus APB1 (pag 53 datasheet), per farlo abilitmao il pin 4 dell' APB1ENR (pag 119 RefMan) */
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	
	/*Settiamo la modalita di funzionamento del pin a cui e' collegato il bottone e di quelli a cui sono collegati i led*/
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);
	GPIOE->MODER |= (1<<16) | (1<<18) | (1<<20) | (1<<22) | (1<<24) | (1<<26) | (1<<28) | (1<<30);
	GPIOE->MODER &= ~(unsigned int)((1<<17) | (1<<19) | (1<<21) | (1<<23) | (1<<25) | (1<<27) | (1<<29) | (1<<31));
	
	/*Useremo diuunque il timer TIM6 (pag 588 RefMan)*/
	/*Configuriamo il timer tramite il registro CR1 (pag. 594 RefMan)*/
	/*Ripristinimao la configurazione del registro cosi' da rimettere i valori attesi modificati da un eventuale altro firmware*/
	TIM6->CR1 = (unsigned short int)0; /*Il valore di reset e' 0, qunindi basta fare una and con una stringa di 0(short int in quanto e' a 16 bit il registro*/
	
	/*Settiamo il bit 2 (URS o Update Request Servive) a 1 in modo che l'unico evento che possa generare un evento e' l'overflow*/ 
	TIM6->CR1 |= TIM_CR1_URS;
	/*Settiamo il bit 3 (OPM o One pulse mode) a 1 in modo da avere il contatore che si ferma quando va in overflow, ovvero quando supera il valore desiderato*/
	TIM6->CR1 |= TIM_CR1_OPM;
	/*Settiamo il bit 7 (ARPE o Auto-reloaded preload enable) a 1 per far in modo che se il valore del registro ARR viene cambiato mentre il contatore e' abilitato, tale modifica viene caricata nel registro effettivo solo al successivo update event(cioe' per come lo abbiamo configurato, quando arriva al valore desiderato)*/
	TIM6->CR1 |= TIM_CR1_ARPE;
	/*Le 4 istruzioni precedenti potevano essere fatte in un unica come segue*/
	/* TIM6->CR1 &= TIM_CR1_URS | TIM_CR1_OPM | TIM_CR1_ARPE;*/
	
	/*Configuriamo il registro CR2 (pag. 596 RefMan) in modalita Update, ovvero genereremo un trigger in uscita dal contatore(su una linea fisica) ad un evento update, mettendo 010 all'MMS (bit 6,5,4)*/
	TIM6->CR2 &= ~(unsigned int)((1<<6)|(1<<4));
	TIM6->CR2 |= (1<<6);
	
	/*Assicuriamoci che lo SR abbia il bit UIF(Update interrupt flag)(tale bit ci segnala che e' avvenuto un evento, dopo un evento deve essere riportato a 0 manualmente dal software, tant'e' che e' abilitato in lettura e inscrittura solo se vi ci si scrive 0) a 0*/
	TIM6->SR &= ~TIM_SR_UIF;
	
	/*Settiamo il PSC(Prescelar Counter)(pag. 598 RefMan) in modo da ottenere un clock, che poi andra in ingresso al contatore vero e proprio*/
	/*Vogliamo ottenere un segnale di overflow ogni 1 sec, e mi basta una risoluzione di 1ms quindi posso settare il prescaler a 7999 (voglio ottenre un clock diviso 8000, poiche il divisore e' prescler +1 metto 7999)*/
	TIM6->PSC = 7999;
	
	/*Settiamo il valore che vogliamo far raggiungere al contatore, nel nostro caso 1s -> 1000ms */
	TIM6->ARR = 1000;
	
	/* Settimao a 1 il bit UG che riporta a 0 il contatore e genera un update event (in modo da fare un update dei registri PSC e ARR che sono bufferizzati)*/
	TIM6->EGR |= TIM_EGR_UG;
	/*Questo bit va alzato ogni qualvolta si vogliano aggiornare i valori dei registri a contatore fermo*/
	
	start=0;
	cont=0;
	GPIOE->ODR &= ~(unsigned int)(cont<<ledoff);
	while(1){ 												/*Entriamo nel while del nostro contatore*/
		/*Verifichiamo che il contatore sia fermo o che il timer abbia completato il conteggio*/
		if((TIM6->SR & TIM_SR_UIF) == TIM_SR_UIF || start==0){		
			if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0){
				start=1;
			}
			if(start==1){
				/*Se non avessimo settato la OPM a 1 avremmo dovuto fermare il contatore tramite il bit 0 del registro CR1*/
			  /*TIM6->CR1 &= ~TIM_CR1_CEN;*/
				/*Assicuriamoci che l'Update interrupt flag sia abbassata in modo da poter cogliere quando il contatore raggiunge il valore desiderato la prossima volta che avvieremo il timer*/
				TIM6->SR &= ~TIM_SR_UIF;
				GPIOE->ODR &= ~(unsigned int)(cont<<ledoff);
				if(cont==255){
					cont=0;
					start=0;
					TIM6->CNT=0;
				}else{
					cont++;
					GPIOE->ODR |= (unsigned int)(cont<<ledoff);
					/*Riportiamo a 0 il contatore(Registro CNT pag 597 REFMAN)*/
					TIM6->CNT=0;
					/*E avviamo il contatore tramite il bit 0 del registro CR1 */
					TIM6->CR1 |= TIM_CR1_CEN;
				}
			}
		}
	}
	
	
	
	return 0;
}
