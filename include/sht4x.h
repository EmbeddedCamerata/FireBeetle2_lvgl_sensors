#ifndef __SHT4X_H__
#define __SHT4X_H__

#include "Adafruit_SHT4x.h"

void sht40_user_setup();
void sht40_get_measured(sensors_event_t *humidity, sensors_event_t *temp);

#endif /* __SHT4X_H__ */
