#include "include/lcd_lvgl.h"
#include "include/sht4x.h"
#include "include/vl53l0x.h"
#include <TFT_eSPI.h>
#include <Wire.h>

#define BTN_PIN 27

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(BTN_PIN, INPUT_PULLUP);
	
	Wire.begin();
	Serial.begin(115200);

    sht40_user_setup();
	vl53l0x_user_setup();

    lvgl_user_setup();

	lcd_basic_layout();
}

// the loop function runs over and over again forever
void loop() {
	lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}