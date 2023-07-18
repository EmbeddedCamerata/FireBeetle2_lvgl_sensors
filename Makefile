comp:
	arduino-cli compile -b DFRobot:esp32:esp32-e FireBeetle2_lvgl_sensors.ino \
	--build-property build.extra_flags=-DLV_CONF_INCLUDE_SIMPLE \
	--build-property build.extra_flags=-I${PWD}/include \
	--build-path ./build \
	--clean \
	--warnings default

upload:
	arduino-cli upload -p /dev/ttyUSB0 -b DFRobot:esp32:esp32-e FireBeetle2_lvgl_sensors.ino 

run:
	arduino-cli compile -p /dev/ttyUSB0 -b DFRobot:esp32:esp32-e FireBeetle2_lvgl_sensors.ino -u --warnings all
