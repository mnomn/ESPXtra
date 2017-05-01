#ifndef ESPCOOLCONF_H
#define ESPCOOLCONF_H

/*
 * Varous useful helper functions for ESP 8266.
 * THis lib uses the RTC memory, so application cannot use it.
 * To use SleepSet/Check: Connect GPIO16 to RST. Otherwize device may not wake up.
 */

#include <Arduino.h>

class ESPCoolStuff
{
  public:
    /*  Create ESPConfig object. This class uses the RTC memory,
        so do not mess with that.*/
    ESPCoolStuff();

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

  private:
};
#endif // ESPCOOLCONF_H
