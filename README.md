# ESPXtra
Various useful functions for ESP8266 microcontroller 

Create an object to use `ESPXtra espx;`

## Sleep
ESP built in ESP.deepSleep(us) max out at a little over an hour. This function lets you sleep much longer (uint32_t minutes)

Call `espx.SleepSetMinutes(sleepMin);` to initialize sleep.

In the setup() function (for example), call `espx.SleepCheck();` and the device may go back to sleep.

A small resistor (220 ohm) between GPIO 16 and RESET will make both flashing and wakeup work. (don't know why, trial and error + googling)

## ButtonPressed
Call `p = ButtonPressed(pin)` to detect a button press. Returns after the button is released, and also returns for how long the button was pressed: short, medium or long.

## PostJson
Post up to 5 values in json format to a url.
```
PostJson("192.168.0.12", 8080, NULL, "value1", 34.56);
PostJson("www.example.com", 80, "secret-key:1234", "value1", 34.56, "value2", 98.65);
```