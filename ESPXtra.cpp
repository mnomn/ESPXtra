#include <ESPXtra.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define COOL_KEY_BEGIN 0x42454749
#define COOL_KEY_END 0x454E4421

#define TIME_BUTTON_MEDIUM 2000UL
#define TIME_BUTTON_LONG 6000UL

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
      ESP.deepSleep(60UL * 60UL * 1000000UL,
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
  rtcData.hours = sleepMinutes / 60;
  sleepMinutes = sleepMinutes % 60;

  /* Sleep 0 minutes will not wake up, so prevent that here when
     even number of hours */
  if (sleepMinutes == 0 && rtcData.hours > 0)
  {
    sleepMinutes = 60;
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
  if (digitalRead(buttonPin) == releasedState) {
    return ButtonNotPressed;
  }

  XTRA_PRINTLN("Button pressed");
  unsigned long start_press = millis();

  // Button pressed, block until release and return how long it was pressed
  while(1) {
    unsigned long now = millis();

    // Calculate how long button has been pressed, and blink
    unsigned long t = now - start_press;
    int buttonTime = ButtonShort;
    if (t > TIME_BUTTON_LONG) {
      if (buttonTime != ButtonLong) {
        buttonTime = ButtonLong;
        XTRA_PRINTLN("Button long");
      }
      // slow blink
      if (ledPin > 0) digitalWrite(ledPin, (now / 500) & 1);
    }
    else if (t > TIME_BUTTON_MEDIUM) {
      if (buttonTime != ButtonMedium) {
        buttonTime = ButtonMedium;
        XTRA_PRINTLN("Button medium");
      }
      // Fast blink
      if (ledPin > 0) digitalWrite(ledPin, (now / 100) & 1);
    }

    // Button released, return.
    int pin_state = digitalRead(buttonPin);
    if (pin_state == releasedState) {
      XTRA_PRINTLN("Button released");
      return buttonTime;
    }

    delay(10); // Buttton not released, wait and check again
  }
}

bool ESPXtra::TimeToWork(unsigned long millisBetweenWork)
{
  static unsigned long prev_work = 0;

  if (millis() - prev_work > millisBetweenWork)
  {
    prev_work += millisBetweenWork;
    return true;
  }

  return false;
}
