#include <ESPXtra.h>
#include <ESP8266WiFi.h>

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

int ESPXtra::PostJson(char * url, int port, char *header,
                      char *name1, float val1,
                      char *name2, float val2,
                      char *name3, float val3,
                      char *name4, float val4,
                      char *name5, float val5)
{
  String jdata;
  WiFiClient client;
  // Remove schema:// from url

  char *path = strchr(url, '/');
  if (path) {
    // Separate host and path
    *path = '\0';
    path++;
  }

  jdata = "{";
  if (name1) {
    jdata += "\"";
    jdata += name1;
    jdata += "\":";
    jdata += val1;
  }
  if (name2) {
    jdata += ",\"";
    jdata += name2;
    jdata += "\":";
    jdata += val2;
  }
  if (name3) {
    jdata += ",\"";
    jdata += name3;
    jdata += "\":";
    jdata += val3;
  }
  if (name4) {
    jdata += ",\"";
    jdata += name4;
    jdata += "\":";
    jdata += val4;
  }
  if (name5) {
    jdata += ",\"";
    jdata += name5;
    jdata += "\":";
    jdata += val5;
  }
  jdata += "}";

  Serial.println(F("Connect to host"));
  if (client.connect(url, port))
  {

    String headerData = "POST /";
    headerData += path;
    headerData += " HTTP/1.1\r\nHost: ";
    headerData += url;
    headerData += "\r\nContent-Type: application/json\r\n";
    if (header)
    {
      headerData += header;
      headerData += "\r\n";
    }
    headerData += "Content-Length:";
    headerData += jdata.length();
    headerData += "\r\nConnection: close\r\n\r\n"; // End of header

    Serial.println(F("Sending a request"));
    Serial.println(headerData);
    client.print(headerData);
    client.println(jdata);

    Serial.println(F("Read response"));
    // Read response
    int max_lines = 15;
    while (client.connected())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        if (max_lines-- > 0) {
          Serial.println(line.substring(0,1024));
        }
      }
    }
    Serial.println(F("Done!"));
    client.stop();
  }
  return 0;
}

int ESPXtra::SleepCheck()
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
      ESP.deepSleep(min_per_h * 60UL * 1000000UL,
                    rtcData.hours ? WAKE_RF_DISABLED : WAKE_RF_DEFAULT);
    }
  }
  else
  {
    Serial.print("SleepCheck Read RTC FAILED!");
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
  ESP.deepSleep(sleepMinutes * 60UL * 1000000UL,
                rtcData.hours ? WAKE_RF_DISABLED : WAKE_RF_DEFAULT);
}

int ESPXtra::ButtonPressed(int pin, int off_state)
{
  static int _pin = -1;
  static unsigned long start_press = 0;
  int pin_state = digitalRead(pin);

  if (pin_state == off_state) {
    _pin = -1;
    return 0;    
  }
  unsigned long now = millis();
  if (pin != _pin) {
    _pin = pin;
    start_press = now;
    return 1;
  }

  int ret = now/1000 - start_press/1000;
  if (ret == 0) ret = 1;

  return ret;
}
