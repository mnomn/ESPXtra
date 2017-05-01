#include <ESPCoolStuff.h>

#define COOL_KEY_BEGIN 0x42454749
#define COOL_KEY_END 0x454E4421

#define min_per_h 60
typedef struct RtcData_t
{
  uint32_t keyBegin;
  uint32_t hours;
  uint32_t keyEnd;
};

ESPCoolStuff::ESPCoolStuff()
{
}

int ESPCoolStuff::SleepCheck()
{
  RtcData_t rtcData;
  if (ESP.rtcUserMemoryRead(0, (uint32_t *)&rtcData, sizeof(RtcData_t)))
  {
    Serial.println();
    Serial.print("SleepCheck Read RTC: ");
    Serial.println(rtcData.hours);
    if (rtcData.keyBegin == COOL_KEY_BEGIN && rtcData.keyEnd == COOL_KEY_END)
    {
      if (rtcData.hours == 0) {
        Serial.println("Done sleeping");
        return 0;
      }
      rtcData.hours--;
      ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(RtcData_t));
      
      Serial.println("Go back to sleep");
      ESP.deepSleep(min_per_h * 60 * 1000000,
                    rtcData.hours ? WAKE_RF_DISABLED : WAKE_RF_DEFAULT);
    }
  }
  else
  {
    Serial.print("SleepCheck Read RTC FAILED!");
  }
  return 1;
}

void ESPCoolStuff::SleepSetMinutes(uint32_t sleepMinutes)
{
  RtcData_t rtcData;
  rtcData.hours = sleepMinutes / min_per_h;
  sleepMinutes = sleepMinutes % min_per_h;

  /* Sleep 0 minutes will not wake up, so prevent that here when
     even number of hours */
  if (sleepMinutes == 0 && rtcData.hours > 0)
  {
    sleepMinutes = min_per_h;
    rtcData.hours--;
  }

  rtcData.keyBegin = COOL_KEY_BEGIN;
  rtcData.keyEnd = COOL_KEY_END;

  /* Sleep minutes now and when device wakes up,
   * sleep one our at the time untill done.
   */
  if (ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(RtcData_t)))
  {
    Serial.println("Wrote to RTC");
  }
  /* If last sleep, wake with radio (default), if not last
   * wake without radio to save power.
   */
  Serial.print("Sleep for ");
  Serial.print(sleepMinutes);
  Serial.print("m, ");
  Serial.print(rtcData.hours);
  Serial.println(" h");
  ESP.deepSleep(sleepMinutes * 60 * 1000000,
                rtcData.hours ? WAKE_RF_DISABLED : WAKE_RF_DEFAULT);
}

///////// Private functions ///////////////////
