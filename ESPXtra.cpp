#include <ESPXtra.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define COOL_KEY_BEGIN 0x42454749
#define COOL_KEY_END 0x454E4421

#define min_per_h 60
struct RtcData_t
{
  uint32_t keyBegin;
  uint32_t hours;
  uint32_t keyEnd;
};

ESPXtra::ESPXtra()
{
}

int ESPXtra::PostJsonString(const String& url, const char *header, const char *jsonStr)
{
  String jdata;
  WiFiClient client;
  HTTPClient http;

  XTRA_PRINT(F("Post to host: ")); XTRA_PRINTLN(url.c_str());
  XTRA_PRINT2(F("Post data: ")); XTRA_PRINTLN2(jsonStr);

  if (!http.begin(client, url)) {
    XTRA_PRINT(F("Failed to connect to url "));
    XTRA_PRINTLN(url.c_str());
    return 1;
  }

  http.addHeader("Content-Type", "application/json");
  int retCode = http.POST(jsonStr);
  XTRA_PRINTF("Return code: %d\n", retCode);

  #if XTRA_DEBUG>1
    String retStr = http.getString();
    Serial.print(F("Returned str :"));
    Serial.println(retStr);
    Serial.println(F("Done!"));
  #endif

  http.end();
  return 0;
}

int ESPXtra::SleepCheck()
{
  RtcData_t rtcData;
  if (ESP.rtcUserMemoryRead(0, (uint32_t *)&rtcData, sizeof(RtcData_t)))
  {
    XTRA_PRINTF2("SleepCheck Read RTC: %d\n", rtcData.hours);
    if (rtcData.keyBegin == COOL_KEY_BEGIN && rtcData.keyEnd == COOL_KEY_END)
    {
      if (rtcData.hours == 0) {
        XTRA_PRINTLN2("Done sleeping");
        return 0;
      }
      rtcData.hours--;
      ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(RtcData_t));
      
      XTRA_PRINTLN2("Go back to sleep");
      ESP.deepSleep(min_per_h * 60UL * 1000000UL,
                    rtcData.hours ? WAKE_RF_DISABLED : WAKE_RF_DEFAULT);
    }
  }
  else
  {
    XTRA_PRINTLN("SleepCheck Read RTC FAILED!");
  }
  return 1;
}

void ESPXtra::SleepSetMinutes(uint32_t sleepMinutes)
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
    XTRA_PRINTLN2("Wrote to RTC");
  }
  /* If last sleep, wake with radio (default), if not last
   * wake without radio to save power.
   */
  XTRA_PRINTF("Sleep for %d h %d m", rtcData.hours, sleepMinutes);
  ESP.deepSleep(sleepMinutes * 60UL * 1000000UL,
                rtcData.hours ? WAKE_RF_DISABLED : WAKE_RF_DEFAULT);
}

int ESPXtra::ButtonPressed(int buttonPin, int ledPin, int releasedState)
{
  static unsigned long start_press = 0;
  int pin_state = digitalRead(buttonPin);

  if (pin_state == releasedState) {
    if (start_press) {
      XTRA_PRINTLN("Released button");
    }
    start_press = 0;
    return 0;
  }

  unsigned long now = millis();

  if (start_press == 0) {
    XTRA_PRINTF("Pressed button %d\n", buttonPin);
    start_press = now;
  }

  unsigned long t = now - start_press;

  if (t > 6000)
  {
    if (ledPin >= 0)
    {
      // slow blink
      digitalWrite(ledPin, (now / 500) & 1);
    }
    XTRA_PRINTLN2("Pressed button long");
    return ButtonLong;
  }

  if (t > 2000)
  {
    if (ledPin >= 0)
    {
      // Fast blink
      digitalWrite(ledPin, (now / 100) & 1);
    }
    XTRA_PRINTLN2("Pressed button medium");
    return ButtonMedium;
  }

  XTRA_PRINT2("Pressed button short ");
  XTRA_PRINTLN2(t);
  return ButtonShort;
}
