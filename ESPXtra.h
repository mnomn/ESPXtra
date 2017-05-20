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
       Returns 0: Not pressed
               1: Pressed
               2: Long pressed (10 sec)
    */
    int ButtonPressed(int pin, int off_state = HIGH);

  private:
};
#endif // ESPXTRA_H
