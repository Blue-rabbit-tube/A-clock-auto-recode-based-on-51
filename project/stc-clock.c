#include "reg51.h"
#include "intrins.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;
sbit P17=P1^7;
sbit P15=P1^5;
sbit P32=P3^2;
sbit P10=P1^0;

#define FOSC 11059200L      //System frequency
#define BAUD 9600           //UART baudrate

/*Define UART parity mode*/
#define NONE_PARITY     0   //None parity
#define ODD_PARITY      1   //Odd parity
#define EVEN_PARITY     2   //Even parity
#define MARK_PARITY     3   //Mark parity
#define SPACE_PARITY    4   //Space parity

#define PARITYBIT EVEN_PARITY   //Testing even parity

bit busy;
unsigned long int count;
unsigned long int ss;
unsigned char sspdd[10]="0123456789";
unsigned int ms_1=0x66;                     //initial timer0 low byte
unsigned int ma_1=0xfc;                //initial timer0 high byte

void uart_init()
{
#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;            //8-bit variable UART
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;            //9-bit variable UART, parity bit initial to 1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;            //9-bit variable UART, parity bit initial to 0
#endif

    TMOD = 0x20;            //Set Timer1 as 8-bit auto reload mode
    TH1 = TL1 = -(FOSC/12/32/BAUD); //Set auto-reload vaule
    TR1 = 1;                //Timer1 start run
    ES = 1;                 //Enable UART interrupt
    EA = 1;                 //Open master interrupt switch 
}

void zhongduan()
{
    IT0 = 1;                        //set INT0 int type (1:Falling 0:Low level)
    EX0 = 1;                        //enable INT0 interrupt
    EA = 1;                         //open global interrupt switch
}

void time0_init()
{
    TMOD |= 0x01;                    //set timer0 as mode1 (16-bit)
	  TL0 = ms_1;                     //initial timer0 low byte
    TH0 = ma_1;                //initial timer0 high byte
    TR0 = 1;                        //timer0 start running
    ET0 = 1;                        //enable timer0 interrupt
}

void SendData(BYTE dat)
{
    while (busy);           //Wait for the completion of the previous data is sent
    ACC = dat;              //Calculate the even parity bit P (PSW.0)
    if (P)                  //Set the parity bit according to P
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;            //Set parity bit to 0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;            //Set parity bit to 1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;            //Set parity bit to 1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;            //Set parity bit to 0
#endif
    }
    busy = 1;
    SBUF = ACC;             //Send data to UART buffer
}

void SendString(char *s)
{
    while (*s)              //Check the end of the string
    {
        SendData(*s++);     //Send current char and increment string ptr
    }
}

void SendoneString(char *s)
{
        SendData(*s);     //Send current char and increment string ptr
}




void main()
{
P10=1;
uart_init();
zhongduan();
time0_init();
	
while(1)
{

while(ss)
{
	SendoneString(&sspdd[(ss%10)]);
	ss=ss/10;
	if(ss==0)
	{
		SendString("\r\n");
	}
}
}
}


void clock() interrupt 1
{
	P15=!P15;
	TL0 = ms_1;                     //initial timer0 low byte
  TH0 = ma_1;                //initial timer0 high byte
  count++;                //1ms
}

void exint0() interrupt 0           //(location at 0003H)
{

P17=!P17;
ss=count;
count=0;

}



void Uart_Isr() interrupt 4
{
    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        busy = 0;           //Clear transmit busy flag
    }
}

