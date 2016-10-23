// Macros to support option testing
#define _CAT(a, ...) a ## __VA_ARGS__
#define SWITCH_ENABLED_false 0
#define SWITCH_ENABLED_true  1
#define SWITCH_ENABLED_0     0
#define SWITCH_ENABLED_1     1
#define SWITCH_ENABLED_      1
#define ENABLED(b) _CAT(SWITCH_ENABLED_, b)
#define DISABLED(b) (!_CAT(SWITCH_ENABLED_, b))

#define NUMERIC(a) ((a) >= '0' && '9' >= (a))
#define NUMERIC_SIGNED(a) (NUMERIC(a) || (a) == '-')
#define COUNT(a) (sizeof(a)/sizeof(*a))

