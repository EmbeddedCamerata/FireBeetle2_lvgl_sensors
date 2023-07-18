#include "../include/sht4x.h"
#include <Arduino.h>

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

void sht40_user_setup()
{

	if (!sht4.begin())
	{
		Serial.println("Couldn't find SHT4x");
		while (1)
			delay(1);
	}

	Serial.println("Found SHT4x sensor");
	Serial.print("Serial number 0x");
	Serial.println(sht4.readSerial(), HEX);

	// You can have 3 different precisions, higher precision takes longer
	sht4.setPrecision(SHT4X_HIGH_PRECISION);
	switch (sht4.getPrecision())
	{
	case SHT4X_HIGH_PRECISION:
		Serial.println("High precision");
		break;
	case SHT4X_MED_PRECISION:
		Serial.println("Med precision");
		break;
	case SHT4X_LOW_PRECISION:
		Serial.println("Low precision");
		break;
	}

	// You can have 6 different heater settings
	// higher heat and longer times uses more power
	// and reads will take longer too!
	sht4.setHeater(SHT4X_NO_HEATER);
	switch (sht4.getHeater())
	{
	case SHT4X_NO_HEATER:
		Serial.println("No heater");
		break;
	case SHT4X_HIGH_HEATER_1S:
		Serial.println("High heat for 1 second");
		break;
	case SHT4X_HIGH_HEATER_100MS:
		Serial.println("High heat for 0.1 second");
		break;
	case SHT4X_MED_HEATER_1S:
		Serial.println("Medium heat for 1 second");
		break;
	case SHT4X_MED_HEATER_100MS:
		Serial.println("Medium heat for 0.1 second");
		break;
	case SHT4X_LOW_HEATER_1S:
		Serial.println("Low heat for 1 second");
		break;
	case SHT4X_LOW_HEATER_100MS:
		Serial.println("Low heat for 0.1 second");
		break;
	}
}

void sht40_get_measured(sensors_event_t *humidity, sensors_event_t *temp)
{
	sht4.getEvent(humidity, temp); // populate temp and humidity objects with fresh data

	Serial.print("Temperature: ");
	Serial.print(temp->temperature);
	Serial.println(" degrees C");
	Serial.print("Humidity: ");
	Serial.print(humidity->relative_humidity);
	Serial.println("% rH");
}