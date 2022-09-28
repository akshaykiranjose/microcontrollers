#include<AT89C513xA.h>
#include "stdio.h"
#define LCD_data  P2
#define LED P0_6
#define sp P0_7	//speaker output from port0.7

unsigned long i=0;
void SPI_Init();
void LCD_Init();
void Timer_Init();
void LCD_DataWrite(char dat);
void LCD_CmdWrite(char cmd);
void LCD_StringWrite(char * str, unsigned char len);
void LCD_Ready();
void sdelay(int delay);
void delay_ms(int delay);
void LED_blink();

#define CS_BAR P1_4
#define LCD_rs  P0_0
#define LCD_rw  P0_1
#define LCD_en  P0_2
#define LCD_busy  P2_7

__bit transmit_completed= 0;
__bit take_samples=0;


int adcVal=0, avgVal=0, initVal=0;
unsigned char serial_data;
unsigned char data_save_high;
unsigned char data_save_low;

unsigned char voltage[4];
unsigned int count=0;
unsigned int flag=0;


void Timer_Init1(){	//timer 1 used to make the speaker sound off at a specified frequency
	TH1 = 0xDF;
	TL1 = 0x71;
	TMOD = (TMOD & 0x0F) | 0x10;
	ET1 = 1;
	TR1 = 1;
}
void timer1_ISR (void) __interrupt 3 // new interrupt that deals with the speaker
{
	TH1 = 0xDF;
	TL1 = 0x71;
}

// Function to take samples for every one second and display value on the LCD
// we need to sample the temperature at constant rate so that it acknowledges the temperature and takes action based off of the case.

void calculation(){
			for(i = 0; i < 10; i++)
			{
				CS_BAR = 0;                 
				SPDAT= 0x01;		    
				while(!transmit_completed);	
				transmit_completed = 0;    	
				
				SPDAT= 0x80;			
				while(!transmit_completed);	
				data_save_high = serial_data & 0x03 ;  
				transmit_completed = 0;    	
						
				SPDAT= 0x00;			
				while(!transmit_completed);	
				data_save_low = serial_data;
				transmit_completed = 0;    	
				CS_BAR = 1;                	
				
				adcVal = (data_save_high <<8) + (data_save_low);
				avgVal += adcVal;	
			}
			avgVal = avgVal/10.0;			
			avgVal = avgVal*(3300.0/10240.0); 
			LCD_CmdWrite(0x88);		

		
			voltage[1] = avgVal%10 + 0x30; //finding the hexadecimal equivalent for the corresponding decimal
			avgVal = avgVal/10;
			voltage[0] = avgVal%10 + 0x30;
			LCD_StringWrite(voltage, 2);

			
}

// Function to keep the LED glowing 
//if the alarm is acknowledged and temperature is greater than 30

void LED_blink()
{
if(voltage[0] >= 0x33 && voltage[1]>=0x30)
{

			LED = 0;
}
else{
		flag=1;
		LED = 1;
		}
}

// Function to play a sound on the speaker when the alarm is not acknowledged within 5
//seconds and the temperature is greater than 30

void speaker(){
	LED = 0;
	Timer_Init1();
	// Initializing the timer for the speaker
	
	while(P1_0==0){
		sp = 1;
		while(TF1==0);
   calculation();
   //Taking samples continuously to check the temperature
   //only then can the speaker turn off as the temperaturehas fallen below the treshold value
		if(!flag)  // To check if the temperature is greater than 30 and to keep the LED on continuously if alarm is acknowledged
		LED=0;
		sp = 0;
		while(TF1==0);
		calculation();
		LED=1;
		LED_blink();
	}
	sp = 1;
}



// Main function

void main(void)
{
	P3 = 0X00;
	P2 = 0x00;
	P1 &= 0xEF;
	P0 &= 0xF0;

	SPI_Init();
	LCD_Init();

	LCD_StringWrite("Temp(C): x  ", 12);

	while(1)
	{
		while(P1_0==0)
		{
			calculation();
			if(voltage[0] >= 0x33 && voltage[1]>=0x30){
			LED = 1;
			delay_ms(125);
			LED = 0;
			delay_ms(75);
			calculation();
			count++;
			// Blink the LED for 5 seconds if the temp>30
				if(count>=10){
				speaker();
				}
			}

		}
		
		calculation();
		// Keep LED on if temp>30 else blink the LED
		if(voltage[0] >= 0x33 && voltage[1]>=0x30){
			LED = 0;
			count = 0;
		}
		else{
		LED = 1;
		count = 0;
		}
	}
}


void it_SPI(void) __interrupt 9 
{
	switch	( SPSTA )         
	{
		case 0x80:	
			serial_data=SPDAT;   
                        transmit_completed=1;
 		break;

		case 0x10:
			serial_data=SPDAT;   
                        transmit_completed=1;
                        
		break;
	
		case 0x40:
                          
		break;
	}
}

void timer0_ISR (void) __interrupt 1
{
	TH0 = 0x3C;	
	TL0 = 0xB0;
	take_samples = 1;
}


 
void SPI_Init()
{
	CS_BAR = 1;	                 
	SPCON |= 0x20;               	 
	SPCON |= 0x01;                	
	SPCON |= 0x10;               	  
	SPCON &= ~0x08;               	
	SPCON |= 0x04;                	
	IEN1 |= 0x04;                	 
	EA=1;                         	 
	SPCON |= 0x40;                	
}
	

void Timer_Init()
{
	
	TH0 = 0x3C;			
	TL0 = 0xB0;
	TMOD = (TMOD & 0xF0) | 0x01;  	
	ET0 = 1;                      	
	TR0 = 1;                      	
}
	
void LCD_Init()
{
  sdelay(100);
  LCD_CmdWrite(0x38);   	        
  LCD_CmdWrite(0x0E);			
  LCD_CmdWrite(0x01);			
  LCD_CmdWrite(0x80);			
}


void LCD_CmdWrite(char cmd)
{
	LCD_Ready();
	LCD_data=cmd;     			
	LCD_rs=0;         	 		
        LCD_rw=0;          			
        LCD_en=1;          			
        sdelay(5);
        LCD_en=0;
	sdelay(5);
}


void LCD_DataWrite(char dat)
{
  LCD_Ready();
  LCD_data=dat;	   			
  LCD_rs=1;	   			
  LCD_rw=0;    	     			
  LCD_en=1;	   			
  sdelay(5);
  LCD_en=0;
  sdelay(5);
}

void LCD_StringWrite( char * str, unsigned char length)
{
    while(length>0)
    {
        LCD_DataWrite(*str);
        str++;
        length--;
    }
}


void LCD_Ready()
{
	LCD_data = 0xFF;
	LCD_rs = 0;
	LCD_rw = 1;
	LCD_en = 0;
	sdelay(5);
	LCD_en = 1;
	while(LCD_busy == 1)
	{
		LCD_en = 0;
		LCD_en = 1;
	}
	LCD_en = 0;
}


void sdelay(int delay)
{
	char d=0;
	while(delay>0)
	{
		for(d=0;d<5;d++);
		delay--;
	}
}


void delay_ms(int delay)
{
	int d=0;
	while(delay>0)
	{
		for(d=0;d<382;d++);
		delay--;
	}
}

