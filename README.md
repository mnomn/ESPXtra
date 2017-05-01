# ESPCoolStuff
Various useful functions for ESP8266 microcontroller 

Create an object to use `ESPCoolStuff espStuff;`

## Sleep
ESP built in ESP.deepSleep(us) max out at a little over an hour. This function lets you sleep much longer (uint32_t minutes)

Call `espStuff.SleepSetMinutes(sleepMin);` to initialize sleep.

In the setup() function (for example), call `espStuff.SleepCheck();` and the device may go back to sleep.
