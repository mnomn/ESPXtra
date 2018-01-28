#ifndef ESPXTRA_H
#define ESPXTRA_H

/*
 * Varous useful helper functions for ESP 8266.
 * THis lib uses the RTC memory, so application cannot use it.
 * To use SleepSet/Check: Connect GPIO16 to RST. Otherwize device may not wake up.
 */

#include <Arduino.h>

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
       CheckSleepInSetup in setup() function.
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

    /* Post Json data to a url (without http://),
       and if needed, add extra header.
       PostJson("www.example.com", 80, "secret-key:1234", "value1", 34.56, "value2", 98.65);
       Return 0 on success.
    */
    int PostJson(char * url, int port, char *header = NULL,
                 char *name1 = NULL, float val1 = 0,
                 char *name2 = NULL, float val2 = 0,
                 char *name3 = NULL, float val3 = 0,
                 char *name4 = NULL, float val4 = 0,
                 char *name5 = NULL, float val5 = 0);
  private:
};
#endif // ESPXTRA_H
