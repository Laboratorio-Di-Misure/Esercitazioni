#include <stm32f303xc.h>
/* Questa libreria contiene puntatori a tutte le periferiche del microcontrollore, e' organizzata come segue:
		ogni perifericha e' una struct che contiene al suo interno tante variabili da 32 bit quanti sono i registri di quella periferica
		ci fornisce poi dei puntatori a quei registri che hanno memorizzato l'indirizzo base della periferica, in modo da poter calcolare "automaticamente" gli indirizzi e poter accedere direttamente ai valori del registro, ad esempio,
		con la sintassi GPIOA->MODER si accede direttamente al moder di GPIO A*/
#define ledoff 8
int main(){
	unsigned int buffer;
	unsigned short int cont;
	unsigned int on;

	/*ablitare clock linea A e linea e*/
	/* La stessa libreria ci mette anche a disposizione una serie di valori predefiniti, nel nostro caso RCC_AHBENR_CPIOAEN e RCC_AHBENR_GPIOEEN
		sono delle stringhe a 32 bit che hanno alto solo il valore 21 (abilita la porta E) e 18 (abilita la porta A), i valori esatti di tali costanti si possono
		trovare leggendo la libreria*/
	RCC->AHBENR |= (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOEEN);
	
	/*ablitare modalita input linea A porta 0*/
	GPIOA->MODER &= ~GPIO_MODER_MODER0;
	
	/*abilitare la modalita output linea E, tutte le porte dei led*/
	buffer=0;
	buffer=(1<<16)|(1<<18)|(1<<20)|(1<<22)|(1<<24)|(1<<26)|(1<<28)|(1<<30);
	GPIOE->MODER |= buffer;

	/*La logica e' la stessa dell' esercizio precedente*/
	on=0;
	cont=0;
	while(1){
		buffer=1;
		if((GPIOA->IDR&=buffer)==buffer){
			if(on==0){
				buffer=(cont<<ledoff);
				GPIOE->ODR &= ~buffer;
				cont++;
				buffer = (cont<<ledoff);
				GPIOE->ODR|= buffer;
				on=1;
			}
		}else{
			on=0;
		}
	}
	return 0;
}
