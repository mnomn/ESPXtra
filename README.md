# ESPXtra
Various useful functions for ESP8266 microcontroller 

Create an object to use `ESPXtra espx;`

## Sleep
ESP built in ESP.deepSleep(us) max out at a little over an hour. This function lets you sleep much longer (uint32_t minutes)

Call `espx.SleepSetMinutes(sleepMin);` to initialize sleep.

In the setup() function (for example), call `espx.SleepCheck();` and the device may go back to sleep.

## ButtonPressed

Call `p = ButtonPressed(pin)` to detect a button press (p == 1) or a button long press (p == 2).
