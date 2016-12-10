

#include <MIDI.h>

#if (PLAIN_LED == 0) //Set outputs for Smart LED 
#include <Adafruit_NeoPixel.h>
#endif

#include "configuration.h"
#include <avr/interrupt.h>
 

//Debounce variables
volatile unsigned long Tick_10ms = 0;
unsigned long last_interrupt_time = 0;

const int input_Pins[] = {2,3,4,6,7,8,A0,A1,A2,10,11,12};
const int output_Pins[] = {5,9,A3,13};
 
//const int input_Pins[] = {13,12,11,9,8,7,5,4,3,A3,A2,A1};
//const int output_Pins[] = {10,6,2,A0};

MIDI_CREATE_DEFAULT_INSTANCE();

#if (PLAIN_LED == 0) //Set outputs for Smart LED 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(6, A5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixel0 = Adafruit_NeoPixel(1, output_Pins[0], NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixel1 = Adafruit_NeoPixel(1, output_Pins[3], NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixel2 = Adafruit_NeoPixel(1, output_Pins[1], NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixel3 = Adafruit_NeoPixel(1, output_Pins[2], NEO_RGB + NEO_KHZ800);
#endif

volatile byte pins[MAX_INPUT_PINS]= { 1 };
volatile byte last_pins[MAX_INPUT_PINS]= { 1 };

struct channel {
      int mute=0;
      unsigned long release_delay_time=0;
      int marker0=0;
      int marker1=0;
      char mute_led = 0;
    };
channel channels[CHANNELS];

// Handles incoming CC 
void handleControlChange(byte channel, byte number, byte value){
if(channel == 1){
    if((number>=MUTE_FEEDBACK_CONTROL) && (number <=(MUTE_FEEDBACK_CONTROL+CHANNELS))){ //Mute status 
      if(value==127){
        channels[number-MUTE_FEEDBACK_CONTROL].mute_led = 1;
      }
      else if(value == 0){
        channels[number-MUTE_FEEDBACK_CONTROL].mute_led = 0;
      }
    ReadPins(); 
    }
    
//CLEAN THIS UP

#if (PLAIN_LED == 0) //Set outputs for Smart LED  
  if(number==127){ 
    switch(value){
        case 0:StripSet(4,5,pixels.Color(0,0,0)); break;
        case 1:StripSet(4,5,pixels.Color(0,255,0)); break;
        case 2:StripSet(4,5,pixels.Color(0,255,255)); break;
        case 4:StripSet(4,5,pixels.Color(255,0,0)); break;
        default: StripSet(4,5,pixels.Color(0,0,0)); break;
    }
  }
#endif      
 
}  
}

// -----------------------------------------------------------------------------
void setup(){
  
  // Connect the handleNoteOn function to the library,
  // so it is called upon reception of a NoteOn.
  MIDI.setHandleControlChange(handleControlChange);  // Put only the name of the function
  
  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);
  // Turn off midi thru (creates loop) 
  MIDI.turnThruOff();
  init_timers();
  InitPins();
}

void loop(){
  
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();
  UpdateChannels();
}


void UpdateChannels(){
  for(int i=0; i<CHANNELS;i++){  
    if(channels[i].mute > 0){
      MIDI.sendControlChange(i+MUTE_CONTROL,PUSH_TO_TALK?0:127,1);
    }
    else if ((millis()<=channels[i].release_delay_time) && (channels[i].mute<0)){
      MIDI.sendControlChange(i+MUTE_CONTROL,PUSH_TO_TALK?127:0,1); 
    }
    channels[i].mute=0; 
    
    if (channels[i].marker0 < 0){
      MIDI.sendControlChange(i+9,0,1); 
    }
    channels[i].marker0=0;
    
    if (channels[i].marker1 < 0){
      MIDI.sendControlChange(i+17,0,1); 
    }
    channels[i].marker1=0;


#if PLAIN_LED //Set outputs for Plain LED 
     
     digitalWrite(output_Pins[i*LEDS_PER_CHANNEL],INVERT_MUTE_LED?!channels[i].mute_led:channels[i].mute_led);
     
#else

    uint32_t mutecolor = INVERT_MUTE_LED?pixels.Color(0,0,0):pixels.Color(255,0,0);
    uint32_t unmutecolor = INVERT_MUTE_LED?pixels.Color(255,0,0):pixels.Color(0,0,0);
    
    if(channels[0].mute_led)
      pixel0.setPixelColor(0, mutecolor);
    else
      pixel0.setPixelColor(0, unmutecolor);
    pixel0.show();
    
    if(channels[1].mute_led)
      pixel1.setPixelColor(0, mutecolor);
    else
      pixel1.setPixelColor(0, unmutecolor);
    pixel1.show();
    
    if(channels[2].mute_led)
      pixel2.setPixelColor(0, mutecolor);
    else
      pixel2.setPixelColor(0, unmutecolor);
    pixel2.show();
        
    if(channels[0].mute_led)
      pixel3.setPixelColor(0, mutecolor);
    else
      pixel3.setPixelColor(0, unmutecolor);
    pixel3.show();
    
#endif
  }
}


//add statemachine idle,down,up,hold ?

void ReadPins(){
    for(int i=0; i<CHANNELS; i++)
    {
      if(!channels[i].mute) {
        int transition = detect_transition(i*BUTTONS_PER_CHANNEL);
        channels[i].mute = transition;
        if(transition){
          channels[i].release_delay_time = (RELEASE_DELAY_MS+millis());
        }
        
      }
      if(!channels[i].marker0) channels[i].marker0 = detect_transition((i*BUTTONS_PER_CHANNEL)+1);
      if(!channels[i].marker1) channels[i].marker1 = detect_transition((i*BUTTONS_PER_CHANNEL)+2);
    }
}


int detect_transition(int i){
 
  int returnval=0; 
  pins[i] = digitalRead(input_Pins[i]);  
  if(pins[i]>last_pins[i]){   //Button was pressed
    returnval=-1;
  }
  else if (pins[i]<last_pins[i]){  //Button was released
    returnval=1;
  }
  last_pins[i]=pins[i];
  return returnval;
}


void InitPins(){ 
  
  for(int i=0; i<(MAX_INPUT_PINS); i++){ 
  #if GLOBAL_PULLUP 
    pinMode(input_Pins[i], INPUT_PULLUP);
  #else
    pinMode(input_Pins[i], INPUT);
  #endif
  pins[i]=digitalRead(input_Pins[i]);
  last_pins[i]=pins[i];
  pciSetup(input_Pins[i]); // setup Pin change interrupt
  }

#if PLAIN_LED //Set outputs for Smart LED 
  for(int i=0; i<(MAX_OUTPUT_PINS); i++){ 
    pinMode(output_Pins[i], OUTPUT);
    digitalWrite(output_Pins[i],LOW);
  }
#else
  pixels.begin();
  pixel1.begin();
  pixel2.begin();
  pixel3.begin();
  pixel0.begin();
#endif
}

#if (PLAIN_LED == 0) //Set outputs for Smart LED 
void StripSet(uint8_t st, uint8_t ed, uint32_t c){
for(int i=st; i<=ed; i++){
   pixels.setPixelColor(i, c); // Moderately bright green color.
}
    pixels.show();
}
#endif


void pciSetup(byte pin){// setup Pin change interrupt for single PIN
  
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}



/*--------------------------------------------------------------------------
  FUNC: 10/13/10 - Sets and starts a system timer
  PARAMS: NONE
  RETURNS: NONE
--------------------------------------------------------------------------*/
void init_timers(void){
  
  cli();            // read and clear atomic !
  //Timer0 for 10ms
  TCCR2B |= (1 << CS02) | (1<<CS01)| (0<<CS00); //Divide by 1024
  TIMSK2 |= 1;     //enable timer overflow interrupt
  sei();            // enable interrupts
}
 

//--------------------------------------------------------------------------
ISR(TIMER2_OVF_vect){           // interrupt every 10ms 

  //TCNT0 is where TIMER0 starts counting. This calculates a value based on
  //the system clock speed that will cause the timer to reach an overflow
  //after exactly 10ms
  TCNT2= 1; //Preload
  Tick_10ms++; 
}

ISR (PCINT0_vect){ // handle pin change interrupt for D8 to D13 here 
  delay(30); 
  ReadPins(); 
}
 
ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
   delay(30); 
   ReadPins();  
}  
 
ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
{
  delay(30); 
  ReadPins();  
}  
 
