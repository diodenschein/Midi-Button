

#include <MIDI.h>
#include "configuration.h"
#include "macros.h"
#include <avr/interrupt.h>
 

//Debounce variables
//volatile unsigned long Tick_10ms = 0;
volatile unsigned long Tick_10ms = 0;
static unsigned long last_interrupt_time = 0;

/*--------------------------------------------------------------------------
  Prototypes
--------------------------------------------------------------------------*/
unsigned long get_key_press( unsigned long key_mask );
void init_timers(void);
void init_io(void);
 
static int input_Pins[] = {2,3,6,7,10,11,16,17};
static int output_Pins[] = {4,5,8,9,13,12,18,19};

MIDI_CREATE_DEFAULT_INSTANCE();


 volatile byte pins[MAX_INPUT_PINS]= { 1 };
 volatile byte last_pins[MAX_INPUT_PINS]= { 1 };

    struct channel {
      int mute=0;
      int marker=0;
      char mute_led = 0;
    };

  channel channels[CHANNELS];
    
void handleControlChange(byte channel, byte number, byte value){
if(channel==1){
  if((number>16) && (number <=(16+CHANNELS))){ //Mute status 
    if(value>=127){
      channels[number-1].mute_led = 1;
    }
    else if(value < 127){
      channels[number-1].mute_led = 0;
    }
  }
}  
}

// -----------------------------------------------------------------------------
void setup()
{
    
    // Connect the handleNoteOn function to the library,
    // so it is called upon reception of a NoteOn.
    MIDI.setHandleControlChange(handleControlChange);  // Put only the name of the function

    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);
    init_timers();
    InitPins();
}

void loop()
{
    // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();
  UpdateChannels();
      //digitalWrite(13, digitalRead(13));
}


void UpdateChannels(){

//    sendControlChange(DataByte inControlNumber,DataByte inControlValue,Channel inChannel);

  for(int i=0; i<CHANNELS;i++){  
    if(channels[i].mute > 0){
      MIDI.sendControlChange(i+1,127,1);
    }
    else if (channels[i].mute<0){
      MIDI.sendControlChange(i+1,0,1); 
        }
    channels[i].mute=0; 
      
    if(channels[i].marker > 0){
      MIDI.sendControlChange(i+17,127,1);
    }
    else if (channels[i].marker<0){
      MIDI.sendControlChange(i+17,0,1); 
        }
    channels[i].marker=0;
  
  for(int j=0; j<LED_PER_CHANNEL;j++){
    digitalWrite(output_Pins[i*LED_PER_CHANNEL],channels[i].marker);
  }
}
}


//add statemachine idle,down,up,hold ?

void ReadPins(){
  digitalWrite(13, digitalRead(13));
 unsigned long interrupt_time = Tick_10ms;
 // If interrupts come faster than 200ms, assume it's a bounce and ignore
// if (interrupt_time - last_interrupt_time > 20) 
 {
    for(int i=0; i<CHANNELS; i++){
    channels[i].mute = detect_transition(i*2);
    channels[i].marker = detect_transition((i*2)+1);
    }
 }
 last_interrupt_time = interrupt_time;
}


int detect_transition(int i){
    pins[i]=digitalRead(input_Pins[i]);  
    int returnval=0;
    if(pins[i]>last_pins[i]){
      returnval=-1;
    }
    else if (pins[i]<last_pins[i]){
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
  pciSetup(input_Pins[i]);
  }

for(int i=0; i<(MAX_OUTPUT_PINS); i++){ 
    pinMode(output_Pins[i], OUTPUT);
    digitalWrite(output_Pins[i],LOW);
  }

}



void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}



/*--------------------------------------------------------------------------
  FUNC: 10/13/10 - Sets and starts a system timer
  PARAMS: NONE
  RETURNS: NONE
--------------------------------------------------------------------------*/
void init_timers(void)
{
  cli();            // read and clear atomic !
  //Timer0 for 10ms
  TCCR0B |= 1<<CS02 | 1<<CS00;  //Divide by 1024
  TIMSK0 |= 1<<TOIE0;     //enable timer overflow interrupt
  sei();            // enable interrupts
}
 

//--------------------------------------------------------------------------
ISR(TIM0_OVF_vect)           // interrupt every 10ms 
{
  //TCNT0 is where TIMER0 starts counting. This calculates a value based on
  //the system clock speed that will cause the timer to reach an overflow
  //after exactly 10ms
  TCNT0 = 100; //Preload
  Tick_10ms++; 
}

ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
{   
  ReadPins();  
}
 
ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
  ReadPins();
}  
 
ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
{
  ReadPins();
}  
 
