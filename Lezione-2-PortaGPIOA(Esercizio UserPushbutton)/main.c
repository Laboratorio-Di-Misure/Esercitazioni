#define RCC_AHBENR 0x40021014 	/*Indirizzo base dei registri RCC pag.52 Datasheet + Offset del registro AHBENR a pag.116 del RefMan*/

#define GPIOA_MODER 0x48000000 	/*Indirizzo base dei registri GPIOA pag.52 Datasheet + Offset del registro AHBENR a pag.143 del RefMan*/

#define GPIOA_IDR 0x48000010		/*Indirizzo base dei registri della porta GPIOE pag.52 Datasheet + Offset del registro IDR a pag.145 del RefMan*/
									/*L'IDR (Input Data Register) serve a memorizzare i dati provenineti da periferiche esterne*/
																
#define GPIOE_MODER 0x48001000 	/*Indirizzo base dei registri della porta GPIOE pag.52 Datasheet + Offset del registro MODER a pag.143 del RefMan*/	
															
#define GPIOE_ODR 0x48001014		/*Indirizzo base dei registri della porta GPIOE pag.52 Datasheet + Offset del registro ODR a pag.145 del RefMan*/

/***ATTENZIONE***/
/* Per poter comprendere questo codice e' opportuno aver letto e compreso gli esercizi precedenti*/


int main(){
	unsigned int* p;
	unsigned int buffer=0;
	
	/*abilitiamo il clock alla GPIOA e alla GPIOE (bit 17 e 21)(RefMan pag. 116)*/
	p=(unsigned int*)RCC_AHBENR;
	buffer=(1<<17)|(1<<21);
	*p |= buffer;
	
	/*Mi assicuro che la linea 0 della GPIOA sia in modalita' input(00 nei primi due bit del registro)(Di default dovrebbe essere settata cosi', ma io non se firmware eseguito in precedenza cosa possa aver lasiato scritto, quindi e' sempre opportuno inserire al momento i valori desiderati*/
	p=(unsigned int*)GPIOA_MODER;
	buffer=0x00000003;  /*Del tutto equivalente a scrivere (1<<1)|1 che a sua volta equivale a (1<<1)|(1<<0)*/
	*p &= ~buffer;
	
	/*Abilitiamo la modalita output sulla linea 9(led*/
	p=(unsigned int*)GPIOE_MODER;
	buffer=(1<<18);
	*p |= buffer;
	
	
	/*Poichè questa è l'unica funzione che il nostro microcontrollore dovrà svolgere creiamo un ciclo in cui si effettua la verifica della pressione o meno del bottone*/
	
	while(1){   /*Poiche' il dispostivo deve fare solo questo, allora lo chiudo in un loop*/
		buffer=1;
		/*La pressione del bottone è vista come un input, di conseguenza i valori che "mi arrivano" vengono memorizzati nell' IDR (Pag 145 RefMan)*/
		p=(unsigned int*)GPIOA_IDR;
		if((*p & buffer)==buffer){  /* Verifico il valore sulla linea 0 della GPIOA, cioe' se il pulsante e' premuto o rilasciato, per farlo ne faccio una and con il valore temporaneo in modo*/
									/*Da avere in tutte le altre posizioni 0 e nella posizione che voglio verificare 1 se c'e' uno, di conseguenza se il pin che mi interessa e' alzato quella and mi restituira proprio cio che ho nel buffer*/
									/*Esempio   01001011 & 00001000 = 00001000  mentre 01001011 & 00100000 = 00000000*/
				/*Se il pulsante e' premuto allora accendo il led(o lo tengo acceso)*/
			p =(unsigned int*)GPIOE_ODR; 
			buffer=(1<<9);
			*p |= buffer;
		}else{
				/*Se il pulsante e' rilasciato allora lo spengo (o lo tengo spento)*/
			p =(unsigned int*)GPIOE_ODR;
			buffer=(1<<9);
			*p &= ~buffer;
		}
		/*c'è da tenere conto che il tempo di reazione di un essere umano è nell'ordine delle centinaia di millisecondi, quindi una pressione del pulsante è almeno 500ms, in questo intervallo
		* il microcontrollore che lavora a 8Mhz di default avrà completato tantissime volte tale ciclo quindi, ad esemepio, sarà entrato nell'if che verifica se il bottone è premuto più volte di quelle che ci aspettiamo*/	
	}
	
	return 0;
}
