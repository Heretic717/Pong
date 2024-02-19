
// Button utilities
struct Button_State {
	bool is_down;
	bool changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_ENTER,
	BUTTON_ESCAPE,
	BUTTON_W,
	BUTTON_S,

	BUTTON_COUNT, // Used to initialise the buttons array, should always be last
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
};
