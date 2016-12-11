#define CHANNELS 4
#define BUTTONS_PER_CHANNEL 3
#define LEDS_PER_CHANNEL 1

#define GLOBAL_PULLUP 1

#define PLAIN_LED 0   //0:WS2811 style LED, 1:Plain LED to VCC. 
#define INVERT_MUTE_LED 0  //0:Mute -> LED On, 1: Mute-> LED OFF

#define PUSH_TO_TALK 0
#define MUTE_CONTROL 1 //CC Midi note so send
#define MUTE_FEEDBACK_CONTROL 1 //CC Midi note to listen to 
#define RELEASE_DELAY_MS 150


// Don't change anything here. 
#define PINS_PER_CHANNEL (BUTTONS_PER_CHANNEL+LEDS_PER_CHANNEL)
#define MAX_INPUT_PINS  (CHANNELS*BUTTONS_PER_CHANNEL)
#define MAX_OUTPUT_PINS  (CHANNELS*LEDS_PER_CHANNEL)
