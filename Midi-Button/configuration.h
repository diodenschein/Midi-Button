#define CHANNELS 4
#define BUTTONS_PER_CHANNEL 3
#define LED_PER_CHANNEL 1

#define GLOBAL_PULLUP 1
#define PLAIN_LED 1 


#define PINS_PER_CHANNEL (BUTTONS_PER_CHANNEL+LED_PER_CHANNEL)
#define MAX_INPUT_PINS  (CHANNELS*BUTTONS_PER_CHANNEL)
#define MAX_OUTPUT_PINS  (CHANNELS*LED_PER_CHANNEL)

#define MUTE_CONTROL 1
#define MUTE_FEEDBACK_CONTROL 1
