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

    /* Check if a button is not pressed, pressed, longn presssed.
       Only one button at the time can be checked for long press.
       pin: Which rpio pin that shall be pressed
       off_state (optional, default HIGH). Set LOW if pin is normally low.
       Returns Seconds pressed, 0 if not pressed

    */
    int ButtonPressed(int pin, int off_state = HIGH);

    /* Post Json data to a url
       and if needed, add extra header.
       PostJsonString("http://www.example.com:8080/some/path", "secret-key:1234", "{\"val1\":123,\"val2\":321.0}");
       Return 0 on success.
    */
    int PostJsonString(const String& url, const char *header, const char *jsonStr);
};
#endif // ESPXTRA_H
