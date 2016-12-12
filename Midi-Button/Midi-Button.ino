#include <MIDI.h>
#include "configuration.h"
#include <avr/interrupt.h>

#if (PLAIN_LED == 0) //Set outputs for Smart LED 
#include <Adafruit_NeoPixel.h>
#endif


const int input_Pins[] = {2,3,4,6,7,8,A0,A1,A2,10,11,12};
const int output_Pins[] = {5,9,A3,13};

volatile byte pins[MAX_INPUT_PINS]= { 1 };
volatile byte last_pins[MAX_INPUT_PINS]= { 1 };

typedef struct{
      int mute=0;
      unsigned long release_delay_time=0;
      int marker0=0;
      int marker1=0;
      char mute_led = 0;
      char armed = 0;
    }channel;
channel channels[CHANNELS];


bool RecFeedback = 0;
bool PlayFeedback = 0;
bool PauseFeedback = 0;
bool GlobalChange = 0;

bool active = 0; //update leds or not


MIDI_CREATE_DEFAULT_INSTANCE();

#if (PLAIN_LED == 0) //Set outputs for Smart LED 
Adafruit_NeoPixel pixel0 = Adafruit_NeoPixel(6, GLOBAL_STATUS_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixel1 = Adafruit_NeoPixel(1, output_Pins[0], NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixel2 = Adafruit_NeoPixel(1, output_Pins[3], NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixel3 = Adafruit_NeoPixel(1, output_Pins[1], NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixel4 = Adafruit_NeoPixel(1, output_Pins[2], NEO_RGB + NEO_KHZ800);
#endif

// -----------------------------------------------------------------------------
void setup(){
  // Set funtion to handle Control change
  MIDI.setHandleControlChange(handleControlChange);
  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);
  // Turn off midi thru (otherwise creates loop) 
  MIDI.turnThruOff();
  //Setup all the pins
  InitPins();
}

//"Main" loop 
void loop(){
  MIDI.read(); //handle incoming midi
  if(active){
    UpdateStrip(); //Update Global LEDs
    UpdateChannelState(); //Update/handle channel buttons and leds
  }
}


// Handle incoming CC 
void handleControlChange(byte channel, byte number, byte value){
  if(channel == 1){
    active = 1; //update LEDs
    GlobalChange = 1; //Something happend
    //Mute state 
    if((number>=MUTE_FEEDBACK_CONTROL) && (number <=(MUTE_FEEDBACK_CONTROL+CHANNELS))){
      if(value==127){
        channels[number-MUTE_FEEDBACK_CONTROL].mute_led = 1;
      }
      else if(value == 0){
        channels[number-MUTE_FEEDBACK_CONTROL].mute_led = 0;
      }
    }
    // Rec arm state
    else if((number>=REC_ARM_FEEDBACK_CONTROL) && (number <=(REC_ARM_FEEDBACK_CONTROL+CHANNELS))){
      if(value == 127){
        channels[number-REC_ARM_FEEDBACK_CONTROL].armed = 1;
      }
      else if(value == 0){
        channels[number-REC_ARM_FEEDBACK_CONTROL].armed = 0;
      }
    }
    //play state
    else if(number == PLAY_FEEDBACK_CONTROL){
      if(value == 127){
        RecFeedback = 1;
      }
      else if(value == 0){
        RecFeedback = 0;
      }
    }
    //pause state
    else if(number == PAUSE_FEEDBACK_CONTROL){
      if(value == 127){
        PauseFeedback = 1;
      }
      else if(value == 0){
        PauseFeedback = 0;
      }      
    }
    //rec arm state
    else if(number == REC_FEEDBACK_CONTROL){
      if(value == 127){
        RecFeedback = 1;
      }
      else if(value == 0){
        RecFeedback = 0;
      }     
    }
    //Reaper is closing
    else if(number == 0x7F){
      LightsOut();
    } 
    else{
      GlobalChange = 0; //Nothing happend
    }
  }  
}


void UpdateStrip(){
  if(GlobalChange){
#if GLOBAL_STATUS
#if GLOBAL_CHANNEL_STATUS    
    for(int i=0; i<CHANNELS;i++){  
      pixel0.setPixelColor(i,ChannelStateToColor(i));     
    }
    StripSet(CHANNELS, pixel0.numpixel0(), GlobalStateToColor());
#else
    StripSet(0, pixel0.numPixels(), GlobalStateToColor());
#endif  
  pixel0.show(); 
#endif
  }
}


//Update/handle channel buttons and leds
void UpdateChannelState(){
  for(int i=0; i<CHANNELS;i++){  
    if(channels[i].mute > 0){
      MIDI.sendControlChange(i+MUTE_CONTROL,PUSH_TO_TALK?0:127,1);
      channels[i].mute=0;
    }
    else if ((millis()>=channels[i].release_delay_time) && (channels[i].mute<0)){
      MIDI.sendControlChange(i+MUTE_CONTROL,PUSH_TO_TALK?127:0,1);
      channels[i].mute=0;
    }
    if (channels[i].marker0 < 0){
      MIDI.sendControlChange(i+9,0,1);
      channels[i].marker0=0;
    }
    if (channels[i].marker1 < 0){
      MIDI.sendControlChange(i+17,0,1);
      channels[i].marker1=0; 
    }
    if(GlobalChange){
      //Update LEDS
#if PLAIN_LED //Set outputs for Plain LED 
       digitalWrite(output_Pins[i*LEDS_PER_CHANNEL],INVERT_MUTE_LED?!channels[i].mute_led:channels[i].mute_led);
#else
      if(channels[0].mute_led)
        pixel1.setPixelColor(0, MUTE_LED_COLOR);
      else if(channels[0].armed)
        pixel1.setPixelColor(0, ARMED_LED_COLOR);
      else
        pixel1.setPixelColor(0, CHANNEL_DEFAULT_COLOR);
      pixel1.show();
      
      if(channels[1].mute_led)
        pixel2.setPixelColor(0, MUTE_LED_COLOR);
      else if(channels[1].armed)
        pixel2.setPixelColor(0, ARMED_LED_COLOR);
      else
        pixel2.setPixelColor(0, CHANNEL_DEFAULT_COLOR);
      pixel2.show();
      
      if(channels[2].mute_led)
        pixel3.setPixelColor(0, MUTE_LED_COLOR);
      else if(channels[2].armed)
        pixel3.setPixelColor(0, ARMED_LED_COLOR);
      else
        pixel3.setPixelColor(0, CHANNEL_DEFAULT_COLOR);
      pixel3.show();
          
      if(channels[3].mute_led)
        pixel4.setPixelColor(0, MUTE_LED_COLOR);
      else if(channels[3].armed)
        pixel4.setPixelColor(0, ARMED_LED_COLOR);
      else
        pixel4.setPixelColor(0, CHANNEL_DEFAULT_COLOR);
      pixel3.show(); 
    }
#endif
  }
}


//Read all input pins, called from interrupt
void ReadPins(){
    active = 1;
    for(int i=0; i<CHANNELS; i++){
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
  pixel0.begin();
  pixel2.begin();
  pixel3.begin();
  pixel4.begin();
  pixel1.begin();
#endif
}


#if (PLAIN_LED == 0)
//Set Part of strip from st to ed 
void StripSet(uint8_t st, uint8_t ed, uint32_t c){
for(int i=st; i<=ed; i++){
   pixel0.setPixelColor(i, c);
}
    pixel0.show();
}
#endif


//Make up a Channel color
uint32_t ChannelStateToColor(int chan){
  uint32_t returnval = pixel0.Color(CHANNEL_DEFAULT_COLOR);
  
  if(channels[chan].mute_led || channels[chan].armed){
    if(channels[chan].mute_led){
      returnval = pixel0.Color(MUTE_LED_COLOR);
    }
    else if(channels[chan].armed){
      returnval = pixel0.Color(ARMED_LED_COLOR);
    }
  }
  else{
    returnval = pixel0.Color(CHANNEL_DEFAULT_COLOR);
  }
  return returnval;
}


uint32_t GlobalStateToColor(){
  uint32_t returnval = pixel0.Color(GLOBAL_DEFAULT_COLOR);
  
  if(RecFeedback || PlayFeedback || PauseFeedback){
    if(RecFeedback){
      returnval = pixel0.Color(GLOBAL_REC_COLOR);
    }
    if(PauseFeedback && RecFeedback){
      returnval = pixel0.Color(GLOBAL_PAUSE_COLOR);
    }
  }
  else {
    returnval = pixel0.Color(GLOBAL_DEFAULT_COLOR);
  }
  return returnval;
}


void LightsOut(){
  active = 0;
  for(int i=0; i<CHANNELS; i++){
    channels[i].mute_led=0;
  }
#if (PLAIN_LED == 1)
  for(int i=0; i<(MAX_OUTPUT_PINS); i++){ 
    digitalWrite(output_Pins[i],HIGH);
  } 
#else
  uint32_t offcolor = pixel0.Color(0,0,0);
  StripSet(0,pixel0.numPixels(),offcolor);
  pixel1.setPixelColor(0, offcolor);
  pixel1.show();
  pixel2.setPixelColor(0, offcolor);
  pixel2.show();
  pixel3.setPixelColor(0, offcolor);
  pixel3.show();
  pixel4.setPixelColor(0, offcolor);
  pixel4.show();
#endif
}


// setup Pin change interrupt for single PIN
void pciSetup(byte pin){
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}


// handle pin change interrupt for D8 to D13 here 
ISR (PCINT0_vect){
  delay(30); 
  ReadPins(); 
}


// handle pin change interrupt for A0 to A5 here
ISR (PCINT1_vect){
   delay(30); 
   ReadPins();  
}  


// handle pin change interrupt for D0 to D7 here 
ISR (PCINT2_vect){
  delay(30); 
  ReadPins();  
}  
 
