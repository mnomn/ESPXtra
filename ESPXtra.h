#ifndef ESPXTRA_H
#define ESPXTRA_H

/*
 * Varous useful helper functions for ESP 8266.
 * THis lib uses the RTC memory, so application cannot use it.
 * To use SleepSet/Check: Connect GPIO16 to RST. Otherwize device may not wake up.
 */

#include <Arduino.h>

#ifndef XTRA_DEBUG
#define XTRA_DEBUG 0
#endif

// Enable by defining "XTRA_DEBUG"
#define XTRA_PRINT(x) do {if(XTRA_DEBUG)Serial.print(x);}while(0)
#define XTRA_PRINTLN(x) do {if(XTRA_DEBUG)Serial.println(x);}while(0)
#define XTRA_PRINTF(x,...) do {if(XTRA_DEBUG)Serial.printf(x,__VA_ARGS__);}while(0)

/* Enable by defining "XTRA_DEBUG 2"
in platform.io:
build_flags =
  -DXTRA_DEBUG=2
*/
#define XTRA_PRINT2(x) do {if(XTRA_DEBUG>=2)Serial.print(x);}while(0)
#define XTRA_PRINTLN2(x) do {if(XTRA_DEBUG>=2)Serial.println(x);}while(0)
#define XTRA_PRINTF2(x,...) do {if(XTRA_DEBUG>=2)Serial.printf(x,__VA_ARGS__);}while(0)

// Enable by defining "XTRA_DEBUG 3"
#define XTRA_PRINT3(x) do {if(XTRA_DEBUG>=3)Serial.print(x);}while(0)
#define XTRA_PRINTLN3(x) do {if(XTRA_DEBUG>=3)Serial.println(x);}while(0)

class ESPXtra
{
  public:
    static const int ButtonNotPressed = 0;
    static const int ButtonShort = 1;
    static const int ButtonMedium = 2;
    static const int ButtonLong = 3;

    /*  Create ESPConfig object. This class uses the RTC memory,
        so do not mess with that.*/
    ESPXtra();

    /* Check if device shall go back to sleep.
       Typically this function shall be called in setup().
       If it is time to sleep, this function will not return.
       return: 1 if no "sleep info"" found (first/cold boot)
               0 if Done sleeping, returning from reset
               no return: device decided to continue sleeping.
    */
    int SleepCheck();

    /* Configure Sleep. This function will put the device in sleep.
       If you sleep for more than 60 min, you must call
       SleepCheck in the setup() function.
       0 minutes meens forever.
    */
    void SleepSetMinutes(uint32_t minutes);

    /* Check if a button is pressed, short, medium (> 2 sec) or long (> 6 sec) pressed.
       Optionally blink a led to show when button presed medium (medium blinks) or long (long blinks).
       Only one button at the time can be checked for longpress.
       buttonPin: Which gpio pin to check
       ledPin: Which pin the indicator led is connected to (or -1).
       releasedState: (optional, default HIGH). Set LOW if pin is normally low.
       Returns ButtonNotPressed (0), ButtonSort, ButtonMedium, ButtonLong.
    */
    int ButtonPressed(int buttonPin, int ledPin = -1, int releasedState = HIGH);

    /* Post Json data to a url
       and if needed, add extra header.
       PostJsonString("http://www.example.com:8080/some/path", "secret-key:1234", "{\"val1\":123,\"val2\":321.0}");
       Return 0 on success.
    */
    int PostJsonString(const String& url, const char *header, const char *jsonStr);

    /* Check if it time to do periodic work. Call from loop function.
       millisBetweenWork: How often this function will return true.
    */
    bool TimeToWork(unsigned long millisBetweenWork);
};
#endif // ESPXTRA_H
