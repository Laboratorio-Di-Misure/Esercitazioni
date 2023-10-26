#define RCC_AHBENR 0x40021014
#define GPIOA_MODER 0x48000000
#define GPIOA_IDR 0x48000010
#define GPIOE_MODER 0x48001000
#define GPIOE_ODR 0x48001014

int main(){
	unsigned int* p;
	unsigned int buffer;
	unsigned short int on;
	
	/*abilitiamo il clock alla linea A e alla linea E (pin 17 e 21)*/
	buffer=0;
	p=(unsigned int*)RCC_AHBENR;
	buffer=(1<<17)|(1<<21);
	*p |= buffer;
	
	/*mi assicuro che la modalita input del pin 0 della porta A*/
	p=(unsigned int*)GPIOA_MODER;
	buffer=0x00000003;
	*p &= ~buffer;
	
	/*abilitiamo la modalita output sul pin 9*/
	p=(unsigned int*)GPIOE_MODER;
	buffer=(1<<18);
	
	
	/*mandiamo un ciclo che fa la verifica sul bottone e accende o spege il led*/
	on=0;
	while(1){
		buffer=1;
		p=(unsigned int*)GPIOA_IDR;
		if((*p & buffer)== buffer && on==0){  /*Verifico che il pulsante sia premuto e il led non abbia gia' cambiato stato(Se non facessi la seconda verifica rischio che ogni volta che faccio il ciclo se tengo il bottone premuto, il led commuta in continuazione)*/
			buffer=(1<<9);
			p =(unsigned int*)GPIOE_ODR;
			if(((*p & buffer) == buffer)){  /*Verifico se il led è già acceso*/
				*p &= ~buffer;				/*Allora lo spengo*/
			}else{
				*p |= buffer;				/*Altrimenti il contrario*/
			}
			on=1;    /*E setto on a 1 in modo da ricordare che per quell'evento pressione del bottone ho gia' commutato il led*/
		}else{
			on=0;			/*Se il pulsante e' stato rilasciato setto ON a 0 in modo da ricordare che la prossima volta che trova il pulsante premuto deve accendere il led*/
		}
	}
			
	
	return 0;
}
	
