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


#define A    1420   // 880 Hz
#define C    1194   // 1046.5 Hz
#define G    1594   // 784 Hz
#define B1   633   // 1975.5 Hz
#define B    2531   // 987.8 Hz


uint32_t sysTickDelay[9] = {0, A, C, B, G, 0, 0, 0, 0};


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void timerInterrupt(void);

int main(void){
	
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);	// bus clock at 80 MHz
	DisableInterrupts();
  Piano_Init();
  Sound_Init();
	
  // other initialization
  EnableInterrupts();
  while(1){ 
		uint32_t pianoData = Piano_In();
		
	  Timer0A_Init(&timerInterrupt, 0x00FFFFFF);
		
		Sound_Play((sysTickDelay[pianoData]));
		
		
		
  }    
}

void timerInterrupt(void) {
	GPIO_PORTE_DATA_R ^= 0x10;
}





