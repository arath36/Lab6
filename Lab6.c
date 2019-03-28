// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 3-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "dac.h"
#include "Timer0A.h"


#define A    710   // 1760 Hz  (A1)
#define B    1265   // 987.8 Hz 
#define C    1194  // 1046.5 hz
#define D    1064   // 1174.8 Hz (D1)
#define E    948   // 1318.6 Hz (E1)
#define F    895   // 1396.6 Hz (F1)
#define G    797   // 1568.4 Hz (G1)









uint32_t sysTickDelay[9] = {0, B, C, D, E, 0, 0, 0, 0};

uint32_t songNotes[42] = {C, C, G, G, A, A, G, F, F, E, E, D, D, C, G, G, F, F, E, E, D, G, G, F, F, E, E, D, C, C, G, G, A, A, G,F ,F ,E, E, D, D, C};
uint8_t songCount = 0;


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void timerInterrupt(void);
void Song(uint32_t songArr[], uint8_t length);
void Stop(void);

int main(void){
	
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);	// bus clock at 80 MHz
	DisableInterrupts();
  Piano_Init();
  Sound_Init();
	Timer0A_Init(&timerInterrupt, 40000000);
	TIMER0_CTL_R &= 0x00000000;
	
  // other initialization
  EnableInterrupts();
  while(1){ 
		uint32_t pianoData = Piano_In();
		
		if (pianoData < 5 && pianoData > 0){
			 Sound_Play((sysTickDelay[pianoData]));
			 Stop();
		} else if (pianoData == 8) {
			EnableInterrupts();
			TIMER0_CTL_R |= 0x00000001;
		} else {
			DisableInterrupts();
			Stop();
		} 
		
	//  
		
		
		

		
  }    
}

void timerInterrupt(void) {
	GPIO_PORTE_DATA_R ^= 0x10;
	Song(songNotes, 42);
}


void Song(uint32_t songArr[], uint8_t length) {
	NVIC_ST_RELOAD_R = songArr[songCount];
	songCount = (songCount + 1)%length;
	uint32_t delay = 500000;
	NVIC_ST_CTRL_R = 5;
	while (delay > 0) {
		delay -= 1;
	}
	NVIC_ST_CTRL_R = 7;
	
	
}


void Stop(void) {
	TIMER0_CTL_R &= 0x00000000;
	songCount = 0;
	GPIO_PORTE_DATA_R &= 0xEF;
}




