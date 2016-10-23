#include <MIDI.h>
#include "configuration.h"
#include "macros.h"


MIDI_CREATE_DEFAULT_INSTANCE();


volatile byte pins[MAX_INPUT_PINS]= { 1 };
volatile byte last_pins[MAX_INPUT_PINS]= { 1 };

    struct channel {
      int mute;
    };

  channel channels[CHANNELS];
    
void handleNoteOn(byte channel, byte pitch, byte velocity)
{

    // Do whatever you want when a note is pressed.
    // Try to keep your callbacks short (no delays ect)
    // otherwise it would slow down the loop() and have a bad impact
    // on real-time performance.
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  
    // Do something when the note is released.
    // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
}

// -----------------------------------------------------------------------------
void setup()
{
    
    // Connect the handleNoteOn function to the library,
    // so it is called upon reception of a NoteOn.
    MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function

    // Do the same for NoteOffs
    MIDI.setHandleNoteOff(handleNoteOff);

    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);
    InitPins();
  
}

void loop()
{
    // Call MIDI.read the fastest you can for real-time performance.
   MIDI.read();
   ReadPins();
   StuffButtons();

}


void StuffButtons(){

//    sendControlChange(DataByte inControlNumber,DataByte inControlValue,Channel inChannel);
 //digitalWrite(13,1);
for(int i=0; i<CHANNELS;i++){
  
  if(channels[i].mute > 0){
    MIDI.sendControlChange(i+1,127,1);
   
  }
  else if (channels[i].mute<0){
    MIDI.sendControlChange(i+1,0,1);
    
      }
  channels[i].mute=0;   
  }
  //digitalWrite(13,0);
}


//add statemachine idle,down,up,hold ?

void ReadPins(){
for(int i=0; i<MAX_INPUT_PINS; i++){
    pins[i]=digitalRead(i+2);
    if(pins[i]>last_pins[i]){
      channels[i].mute=-1;
    }
    else if (pins[i]<last_pins[i]){
      channels[i].mute=1;
    }
    last_pins[i]=pins[i];
  }
  
}


void InitPins(){

  for(int i=0; i<(MAX_INPUT_PINS); i++){ 
#if GLOBAL_PULLUP 
    pinMode(i+2, INPUT_PULLUP);
#else
    pinMode(i+2, INPUT);
#endif
  pins[i]=digitalRead(i+2);
  last_pins[i]=digitalRead(i+2);
  }

//DEBUG

   pinMode(13, OUTPUT);
}

 
