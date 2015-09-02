#include <SoftwareSerial.h>

#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

TinyGPS gps;
SoftwareSerial ss(4, 3);
char buf[32];

void setup()
{
  Serial.begin(115200);
  ss.begin(4800);
  
  Serial.print("Simple TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  Serial.println("by Mikal Hart");
  Serial.println();
}

void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  unsigned long age;
  if (newData)
  {
    float flat, flon;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    //GPS mode
    Serial.print(" Constellations=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0 : gps.constellations());
  }

  //satellites in view
  uint32_t* satz = gps.trackedSatellites();
  uint8_t sat_count = 0;
  for(int i=0;i<24;i++)
  {
    if(satz[i] != 0)    //exclude zero SNR sats
    {
      sat_count++;
      byte strength = (satz[i]&0xFF)>>1;
      byte prn = satz[i]>>8;
      sprintf(buf, "PRN %d: ", prn);
      Serial.print(buf);
      Serial.print(strength);
      Serial.println("dB");
    }
  }
  
  //date time
  int year;
  uint8_t month, day, hour, minutes, second, hundredths;
  gps.crack_datetime(&year, &month, &day, &hour, &minutes, &second, &hundredths, &age);
  sprintf(buf,"GPS time: %d/%02d/%02d %02d:%02d:%02d", year, month, day, hour, minutes, second);
  Serial.println(buf);

  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
}