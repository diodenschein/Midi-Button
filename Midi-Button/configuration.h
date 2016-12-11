#define CHANNELS 4
#define BUTTONS_PER_CHANNEL 3
#define LEDS_PER_CHANNEL 1

#define GLOBAL_PULLUP 1
#define PUSH_TO_TALK 0
#define RELEASE_DELAY_MS 150

#define PLAIN_LED 0   //0:WS2811 style LED, 1:Plain LED to VCC. 
#define INVERT_MUTE_LED 0  //0:Mute -> LED On, 1: Mute-> LED OFF

#define MUTE_LED_COLOR 200,0,0
#define ARMED_LED_COLOR 2,0,0
#define CHANNEL_DEFAULT_COLOR 0,0,0
// Global Status LEDS (Rotlicht)*********************************

#define GLOBAL_STATUS 1
#define GLOBAL_CHANNEL_STATUS 0

#define GLOBAL_DEFAULT_COLOR 0,0,0 //RGB transparent
#define GLOBAL_REC_COLOR 200,0,0 //RGB red
#define GLOBAL_PAUSE_COLOR 0,20,0 //RGB light green

//outgoing ******************************************************

#define MUTE_CONTROL 1 //CC Midi note so send

//Incoming CC ***************************************************
#define MUTE_FEEDBACK_CONTROL 0x01 //first CC Midi  to listen to 
#define REC_ARM_FEEDBACK_CONTROL 0x09 //first CC Midi  to listen to 

#define PLAY_FEEDBACK_CONTROL 0x59
#define PAUSE_FEEDBACK_CONTROL 0x60
#define REC_FEEDBACK_CONTROL 0x61


// Don't change anything here. 
#define PINS_PER_CHANNEL (BUTTONS_PER_CHANNEL+LEDS_PER_CHANNEL)
#define MAX_INPUT_PINS  (CHANNELS*BUTTONS_PER_CHANNEL)
#define MAX_OUTPUT_PINS  (CHANNELS*LEDS_PER_CHANNEL)
