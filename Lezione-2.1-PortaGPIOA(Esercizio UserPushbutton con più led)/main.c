/*Sono gli stessi registri usati nell'esercizio Lezione 2.0*/
#define RCC_AHBENR 0x40021014
#define GPIOA_MODER 0x48000000
#define GPIOA_IDR 0x48000010
#define GPIOE_MODER 0x48001000
#define GPIOE_ODR 0x48001014

int main(){
	unsigned int* p;
	unsigned int buffer;
	unsigned int cont;      /*Una variabile per ricordare in quale dei 5 stati ci troviamo(0=Led Verde(linea 15 GPIOE) 1=Led Arancione(linea 10 GPIOE) 2=Led Rosso(Linea 9 GPIOE) 3=Led Arancione(Linea 8 GPIOE) 4=Tutti i led)*/
	unsigned int leds[4];   /*Per andare comodi memorizziamo in un arrey le stringhe necessarie ad accendere ogniuno dei 4 leds*/
	unsigned short int on;  /*Una variabile per memorizzare se i led sono stati già accesi o meno*/
							/*Ci è utile in quanto per tutta la durata della pressione del bottone il ciclo di controllo viene eseguito più volte, rischiando di farci avere sequenze indesiderate*/
	
	
	leds[0]=(1<<15);
	leds[1]=(1<<10);
	leds[2]=(1<<9);
	leds[3]=(1<<8);

	
	
	/*abilitiamo il clock alla GPIOA e alla GPIOE (pin 17 e 21 del registro RCC_AHBENR)*/
	buffer=0;
	p=(unsigned int*)RCC_AHBENR;
	buffer=(1<<17)|(1<<21);
	*p |= buffer;
	
	/*mi assicuro che la modalita input(00) del pin 0 della GPIOE*/
	p=(unsigned int*)GPIOA_MODER;
	buffer=0x00000003;
	*p &= ~buffer; /*Equivalente a *p &= ~((1<<1)|(1<<0))*/
	
	/*abilitiamo la modalita output sul pin 15,10,9,8 per il led rosso*/
	p=(unsigned int*)GPIOE_MODER;   /*Ho memorizzato tutte le stringe necessarie ad abilitare le varie linee in un vettore, quindi metto le linee in modalita input con un ciclo*/
	*p |= (1<<16) | (1<<18) | (1<<20) | (1<<30);
	*p &= ~((1<<17) | (1<<19) | (1<<21) | (1<<31))
	
	/*Carico nel vettore i valori necessari ad accendere i vari led*/
	leds[0]=(1<<9);
	leds[1]=(1<<10);
	leds[2]=(1<<8);
	leds[3]=(1<<15);	
	
	/*E inizializzo le variabili temporanee a 0*/
	cont=0;
	on=0;		/*Mi serve per ricordare se i led sono accesi e nel momento in cui li spengo devo incrementare il contatore*/
	while(1){
		buffer=1;
		p=(unsigned int*)GPIOA_IDR;
		if((*p & buffer)==buffer&&on==0){ /*Entro nel blocco solo se il bottone è premuto e i led erano precedentemente spenti*/
			if(cont==4){			/*Se ho gia' acceso tutti i led uno alla volta allora preparo il buffer per accenderli tutti insieme*/
				buffer= leds[0]|leds[1]|leds[2]|leds[3];
			}else{						/* Altrimenti mi preparo ad accendere il led che mi serve*/
				buffer= leds[cont];
			}
			p =(unsigned int*)GPIOE_ODR;
			*p |= buffer;
			on=1;      /*Mi ricordo che ho acceso il(i) led*/
		}else if((*p & buffer)!=buffer&&on==1){ /*Entro nel blocco solo se il bottone è rilasciato e i led erano precedentemente accesi*/
			p =(unsigned int*)GPIOE_ODR;
			if(cont==4){    	/*Se ne ho accesi 4 ne devo spegnere 4*/
				buffer= leds[0]|leds[1]|leds[2]|leds[3];
			}else{              /*Altrimenti spengo solo quello che era acceso*/
				buffer= leds[cont];
			}
			*p &= ~buffer;
			if(on==1){        /*Se on era acceso vuol dire che ho appena spento le luci*/
				cont=(cont + 1)%5;		/*Quindi incremento il conatatore e passo allo stato successivo (Altirmenti ogni volta che rifa' il ciclo e vede il pulsante rilasciato mi incrementa il contatore, anche se di fatto non e' cambiato lo stato del sistema*/
			}
			on=0;
		}	
	}
	
	return 0;
}
