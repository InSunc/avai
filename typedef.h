typedef unsigned char  u8;

typedef struct Motor
{
	u8 iPinRight;
	u8 iPinLeft;
} Motor;

typedef enum element
{
	PLATFORM,
	RAMP

} element;

typedef enum state
{
	PLATFORM_CLOSED,

	PLATFORM_OPEN,
	RAMP_CLOSED,

	RAMP_OPEN
} state;

typedef enum event
{
	OPEN,
	PAUSE,
	CLOSE
} event;
