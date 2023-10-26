#include <stm32f303xc.h>
#define ledsoff 8

int main(){
	unsigned short int on,cont,i;
	unsigned int buffer;
	/*abilito il clock per le porte GPIOA e GPIOE*/
	RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN);
	
	/*abilito la modalita lettura della linea 0 sulla GPIOA*/
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);
	
	/*abilitiamo i led sulla linea GPIOE(modalita output dalla linea 8 alla porta 15*/
	GPIOE->MODER |= ((1<<16)|(1<<18)|(1<<20)|(1<<22)|(1<<24)|(1<<26)|(1<<28)|(1<<30));
	
	/*Facciamo un loop che esegua queste istruzioni*/
	/*Faremo il programma in modo che il contatore parta con la pressione del tasto e incrementi ogni 8000 cicli di for*/
	on=0;
	cont=0;
	while(1){         
		buffer=1;
		if((GPIOA->IDR & buffer)==buffer && on!=2){ /* verifico che il pulsante sia premuto e che il contatore non sia gia' stato avviato(la logica e' che il contatore puo' ripartire se ha finito il giro e il pulsante e' rilasciato*/ 
			on=1;			
		}
		if(on==1){                        	/*se c'e' il segnale di far partire il contatore allora vado*/
			GPIOE->ODR &= ~((unsigned int)cont<<ledsoff);
			cont++;
			GPIOE->ODR |= ((unsigned int)cont<<ledsoff);
			i=0;
			for(;i<8000;i++);									/*poche' deve andare in automatico per rendere un'accensione distinguibile dall'altra mettiamo un ciclo di attesa*/
			if(cont+1==0){                    /*se il prossimo valore da accendere e' 0, quindi il contatore ha finito il ciclo*/
				cont=0;													/*Lo reimposto a 0*/
				GPIOE->ODR &= ~((unsigned int)cont<<ledsoff); /*spengo tutto*/
				on=2;														/* e segnalo che il contatore ha finito*/
			}
		}
		if(!((GPIOA->IDR & buffer)==buffer) && on==2){  /* se il contatore ha finito e il pulsante risulta rilasciato*/
			on=0;																			  /*allora si imposta il flag in modo tale da permetterre al contatore di ripartire in caso di nuova pressione*/
		}
	}
	return 0;
		
}
