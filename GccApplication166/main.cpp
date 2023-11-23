#define F_CPU 8000000UL
#define BAUD 9600
#define UBRR_VAL F_CPU/16/BAUD-1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>

void usart_init(unsigned int speed)
{
    UBRRH=(unsigned char)(speed>>8);
    UBRRL=(unsigned char) speed;
    UCSRA=0x00;
    UCSRB|=(1<<RXEN)|(1<<TXEN);
    UCSRB|=(1<<RXCIE);
    UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

char Data;
bool StateSwOnPs1 = false;
bool NewStateSwOnPs1 = false;
bool State_SW_ON_PS_1 = false;
bool StatePSF1 = true;
bool _0_SW_ON = false;

void StateInp1Psf1(void)
{
        if (StatePSF1 & State_SW_ON_PS_1)
        {
            PORTA &= ~(1<<PA1);
            UDR = 'A';
        }
        else
        {
            PORTA |= (1<<PA1);
            UDR = 'a';
        }    
}
 
ISR(USART_RXC_vect)
{
    UDR = Data;
    switch(Data)
    {
        case 'A':
        StatePSF1 = true;
        StateInp1Psf1();
        break;
        case 'a':
        StatePSF1 = false;
        StateInp1Psf1();
        break;
        case 'O':
        _0_SW_ON = true;
        PORTA &= ~(1<<PA0);
        break;
        case 'o':
        _0_SW_ON = false;
        PORTA |= (1<<PA0);
        break;
    }
}

void ChekSwOnPs1(void)
{
    if (bit_is_set(PINC, PINC0))
    {
        NewStateSwOnPs1 = true;
        State_SW_ON_PS_1 = true;
    }
    if (bit_is_clear(PINC, PINC0))
    {
        NewStateSwOnPs1 = false;
        State_SW_ON_PS_1 = false;
    }
    if (NewStateSwOnPs1 != StateSwOnPs1)
    {
        if (State_SW_ON_PS_1)
        {
            UDR = 'F';
        } 
        else
        {
            UDR = 'f';
        }
        StateInp1Psf1();        
    }
    StateSwOnPs1 = NewStateSwOnPs1;
}

int main(void)
{   
    PORTA |=(1<<PA0);
    PORTA &= ~(1<<PA1); 
    usart_init(BAUD);
    sei();
    while (1) 
    {
        ChekSwOnPs1();
    }
}

