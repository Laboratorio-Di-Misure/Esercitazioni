#define RCC_AHBENR 0x40021014 	/*Indirizzo base dei registri RCC pag.52 Datasheet + Offset del registro AHBENR a pag.116 del RefMan*/
				/*L'AHBENR serve ad abilitare e disabilitare il clock sulle porte collegate ai Bus AHB3 AHB2 AHB1*/
																
#define GPIOE_MODER 0x48001000 	/*Indirizzo base dei registri della porta GPIOE pag.52 Datasheet + Offset del registro MODER a pag.143 del RefMan*/
				/*Il MODER serve ad abilitare le varie modalita' sulle singole linee della GPIO scelta*/
																
#define GPIOE_ODR 0x48001014	/*Indirizzo base dei registri della porta GPIOE pag.52 Datasheet + Offset del registro ODR a pag.145 del RefMan*/
				/*L'ODR (Output Data Register) serve a scrivere un valore sulle varie linee quando impostate in modalita' output*/
																
int main(){
	unsigned int* p; /* I registri sono a 32 byte, quindi possiamo usare un unsigned int come tipo per identificarli*/
	unsigned int buffer,i; /*Sara la nostra variabile di appoggio per abilitare e disabilitare i bit*/
	
	/*Abilitiamo il clock alla GPIOE */
	p= (unsigned int*) RCC_AHBENR; /*assegnamo a p l'indirizzo del registro AHBENR*/
	buffer= (1<<21); /*Carichiamo sul buffer una stringa di tutti 0 e un uno nella posizione che volgiamo settare a 1 sul registro, in questo caso il bit che abilita il clock alla GPIOE e' il 21 pag.116 RefMan*/
	*p |= buffer; /*Alziamo il pin interessato ponendo in or il valore del registro con il buffer, in modo che in tutte le altre posizioni sara' x OR 0 = x, mentre nella posizione che ci interessa x OR 1 = 1*/
	
	/*Abiliamo la modalita' output sulla linea 8 della GPIOE (Ricorda 00 = input, 01=Output, 10= Alternate, 11=Analog)*/
	p= (unsigned int*) GPIOE_MODER; 
	buffer= (1<<16); /* Nel MODER ogni due bit rappresentano la modalita' su una linea, quindi la modalita' sulla linea 8 e' controllata dai bit 16 e 17 del MODER*/
	*p |= buffer;    /*Poniamo il bit 16 a 1*/
	buffer=(1<<17);		/*Poniamo il bit 17 a 0*/
	*p &= ~buffer;    /*Negando il buffer abbiamo una stringa con tutti 1 e uno 0 solo in posizione di quello che dobbiamo abbassare, in modo che in tutte le altre posizioni ci sarà x AND 1 = x e dove voglio abbassare x AND 0 = 0*/
	
	/*Accendiamo il led sulla linea PE8*/
	p= (unsigned int*) GPIOE_ODR;
	buffer=(1<<8);
	*p |= buffer;
	
	/*Inseriamo un' attesa con un ciclo a 8000*/
	for(i=0;i<8000;i++);
	
	/*Spegnamo il led*/
	*p &= ~buffer; /* p memorizza ancora l'indirizzo dell'ODR, quindi ci bastera' andare a cambiarne i valori*/
	
	/*Per sicurezza impostiamo al valore di default la modalita' sulla linea 8, in modo da permettere al prossimo processo di trovare i valori di reset e aggiungere un layer di sicurezza*/
	p= (unsigned int*) GPIOE_MODER;
	buffer=(1<<16)|(1<<17); /*Ogni operazione di shift crea una stringa con un solo(in questo caso) bit alzato, mettere in OR le due stringhe vuol dire che mi troverò una stringa con entrambi i bit alti*/
				/*Esempio: (1<<4) = 00010000 (1<<2) = 00000100 (1<<4)|(1<<2) = 00010100*/  
	*p &= ~buffer;
	
	/*Abilitiamo la modalita' di input sulla linea 9 della GPIOE con la stessa logica seguita per la linea 8*/
	p= (unsigned int*) GPIOE_MODER; 
	buffer= (1<<18);
	*p |= buffer;
	buffer= (1<<19);
	*p &= ~buffer;
	
	/*Accendiamo il led sulla linea PE9*/
	p= (unsigned int*) GPIOE_ODR;
	buffer=(1<<9);
	*p |= buffer;
	
	/*Inseriamo un' attesa con un ciclo a 8000*/
	for(i=0;i<8000;i++);
	
	/*Spegnamo il led*/
	*p &= ~buffer;
	
	/*Per sicurezza impostiamo al valore di default la modalita' sulla linea 9*/
	p= (unsigned int*) GPIOE_MODER;
	buffer=(1<<18)|(1<<19);
	*p &= ~buffer;
	
	/*Disabilitiamo il clock alla GPIOE*/
	p=(unsigned int*) RCC_AHBENR;
	buffer=(1<<21);
	*p &= ~buffer;
	
	while(1); /*AL termine del programma e' sempre opportuno bloccare il processore con un while che non esegue alcuna operazione non sapendo, in assenza di un sistema operativo cosa viene caricato nel PC(Program Counter del core del microcontrollore) dopo il return*/
	return 0;
	
}
