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

//Pitch defines
#define A    710   // 1760 Hz  (A1)
#define B    1265   // 987.8 Hz 
#define C    1194  // 1046.5 hz
#define D    1064   // 1174.8 Hz (D1)
#define E    948   // 1318.6 Hz (E1)
#define F    895   // 1396.6 Hz (F1)
#define G    797   // 1568.4 Hz (G1)


//Note Defines
#define quarter 20000000
#define half  40000000
#define whole 80000000




uint32_t sysTickDelay[9] = {0, B, C, D, E, 0, 0, 0, 0}; //array for the 'piano' part

uint32_t songNotes[42] = {C, C, G, G, A, A, G, F, F, E, E, D, D, C, G, G, F, F, E, E, D, G, G, F, F, E, E, D, C, C, G, G, A, A, G,F ,F ,E, E, D, D, C};
uint8_t songCount = 0; // counter for the song

// structure of how the Music is stored in memory
struct Music {
	uint32_t pitch;
	uint32_t note;
};
typedef const struct Music Music;

// this is the array a user passes into the song function
Music MusicArr[42] = {
	   {C, half}, 
		 {C, half},
		 {G, half},
		 {G, half},
		 {A, half},
		 {A, half},
		 {G, whole},
		 {F, half},
     {F, half},
		 {E, half},
		 {E, half},
		 {D, half},
		 {D, half},
		 {C, whole},
		 {G, half},
		 {G, half},
		 {F, half},
		 {F, half},
		 {E, half},
		 {E, half},
		 {D, whole},
		 {G, half},
		 {G, half},
		 {F, half},
		 {F, half},
		 {E, half},
		 {E, half},
		 {D, whole},
		 {C, half},
		 {C, half},
		 {G, half},
		 {G, half},
		 {A, half},
		 {A, half},
		 {G, whole},
		 {F, half},
		 {F, half},
		 {E, half},
		 {E, half},
		 {D, half},
		 {D, half},
		 {C, whole},
		 
};


// basic functions defined at end of startup.s
// other functions that we defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void timerInterrupt(void);
void Song(Music MusicArray[], uint8_t length);
void Stop(void);

int main(void){
	// all Init functions occur before while loop
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);	// bus clock at 80 MHz
	DisableInterrupts();
  Piano_Init();
  Sound_Init();
	Timer0A_Init(&timerInterrupt, 40000000);  // sets timer interrupt to the timerInterrupt function and gives intial value to reload
	TIMER0_CTL_R &= 0x00000000;  // disables timer0A 
	
  // other initialization
  EnableInterrupts();
  while(1){ 
		uint32_t pianoData = Piano_In();  // gets piano data
		
		if (pianoData < 5 && pianoData > 0){
			// this occurs if one of the first 3 buttons are pressed
			//regular part of lab
			 Sound_Play((sysTickDelay[pianoData]));
			 Stop();   
		} else if (pianoData == 8) {
			// Bonus part of lab
			// enables timer so the timer interrupt can run
			EnableInterrupts();
			TIMER0_CTL_R |= 0x00000001;
		} else {
			// invalid input means no sound plays, all interrupts are disabled
			DisableInterrupts();
			Stop();
		} 
		
	//  
		
		
		

		
  }    
}

void timerInterrupt(void) {
	//flips heartbeat everytime it's run
	GPIO_PORTE_DATA_R ^= 0x10;
	//Plays the song
	Song(MusicArr, 42);
}


void Song(Music MusicArray[], uint8_t length) {
	
  TIMER0_TAILR_R = MusicArr[songCount].note;   // sets timer0A interrupt to length depending on how long pitch needs to play
	NVIC_ST_RELOAD_R = MusicArr[songCount].pitch;  // sets the frequency of SYStick to set pitch
	songCount = (songCount + 1)%length;   //interpolates through the array
	uint32_t delay = 500000;               // stops the sound for a bit so transitions between notes are distinct
	NVIC_ST_CTRL_R = 5;                   // no sound
	while (delay > 0) {
		delay -= 1;
	}
	NVIC_ST_CTRL_R = 7;                  // sound resumes
	
	
}


void Stop(void) {
	// disables timer0A, returns songCount to 0, turns off heartbeat LED
	TIMER0_CTL_R &= 0x00000000;  
	songCount = 0;
	GPIO_PORTE_DATA_R &= 0xEF;
}




