#define RCC_AHBENR 0x40021014  
#define GPIOA_MODER 0x48000000 
#define GPIOA_IDR 0x48000010
#define GPIOE_MODER 0x48001000
#define GPIOE_ODR 0x48001014

#define ledoff 8  			/*Poiche' i led sono tutti in sequenza, dalla posizione 8 alla posizione 15 per accendere quelli corretti mi basta shiftare di 8 il calore del contatore (che e' a 8 bit)*/

int main(){
	unsigned int* p;
	unsigned int buffer;
	unsigned short int cont;
	unsigned int on;
	
	/*Ablitare clock linea A e linea E*/
	p=(unsigned int*) RCC_AHBENR;
	buffer=(1<<17)|(1<<21);
	*p |=buffer;
	
	/*Ablitare modalita input linea A porta 0*/
	p=(unsigned int*)GPIOA_MODER;
	buffer=0x3;
	*p &= ~buffer;
	
	/*Abilitare la modalita output linea E, tutte le porte dei led*/
	p=(unsigned int*)GPIOE_MODER;
	buffer=0;
	buffer=(1<<16)|(1<<18)|(1<<20)|(1<<22)|(1<<24)|(1<<26)|(1<<28)|(1<<30);
	*p |= buffer;

	on=0;        		/* azzero il valore che mi ricorda se i led sono stati accesi o meno*/
	cont=0;					/* azzero il contatore */
	while(1){
		p=(unsigned int*) GPIOA_IDR;
		buffer=1;
		if((*p&=buffer)==buffer){  /* Se il bottone e' premuto */
			p=(unsigned int*)GPIOE_ODR;
			if(on==0){								/* Verifico che i led non siano gia stati accesi per quella pressione */ 
									/*È possibile creare il buffer dei bit da alzare semplicemente shiftando di 8 il valore del contatore in quanto i led, nel GPIOE_ODR, sono tutti in sequenza(vedi pag 145 RefMan tenendo conto che i led vanno dal pin 8 al 15)*/
				buffer=(unsigned int)(cont<<ledoff);  /* Se non lo sono allora spengo la combinazione precedente */
				*p &= ~buffer;
				cont++;                 	/* Incremento il contatore */ 
				buffer =(unsigned int)(cont<<ledoff);  /* E accendo la nuova combinazione */
				*p |= buffer;
				on=1;
			}
		}else{
			on=0;
		}
	}
	return 0;
}
