# TinyGPS upgrade for NMEA Data Protocol v3.x and GLONASS

This update adds support for newer NMEA-capable GPS devices that implement the [v3.x GNSS spec](http://geostar-navi.com/files/docs/geos3/geos_nmea_protocol_v3_0_eng.pdf) as well as devices that support [GLONASS](https://en.wikipedia.org/wiki/GLONASS).

<center><table><tr valign='center'><td><img src='http://blog.newsplore.com/wp-content/uploads/2015/09/gps-notrack.jpg' width='200px'/></td><td><img src="http://blog.newsplore.com/wp-content/uploads/2015/09/gps-track.jpg" width='200px'/></tr><tr align='center'><td>Acquiring position</td><td>Tracking</td></tr></table></center>

#### The following new sentences are now supported:

1. NMEA GPS sentence:
  * GPS Satellites in view [GPGSV](http://aprs.gids.nl/nmea/#gsv)
2. GNSS sentences:
  * GNRMC (same with GPRMC)
  * GNGNS
3. GLONASS sentences:
  * GLGSV

#### Tracking satellites in view for both GPS and GLONASS constellations.

This allows for building e.g. an advanced GUI that shows the satellites in view and their respective strength both when searching and when tracking.
The data is accessible via a new method: `uint32_t[] trackedSattelites()` that returns an array of uint32 that (to keep the additional memory footprint at a minimum) encodes the useful data as follows (check [GPGSV](http://aprs.gids.nl/nmea/#gsv) for a comprehensive explanation ):

  * bit 0-7: sattelite ID
  * bit 8-14: SNR (dB), max 99dB
  * bit 15: this sattelite is used in solution (not implemented yet)

The uint32 array size is 24, that is, it tracks up to 12 satellites for each constellation, GPS and GLONASS. 12 is the maximum number of satellites in view per constellation at any given point on Earth.
```c
...
  char buf[32];
  uint32_t* satz = tinygps.trackedSatellites();
  uint8_t sat_count = 0;
  for(int i=0;i<24;i++)
  {
    if(satz[i] != 0)	//exclude zero SNR sats
    {
      sat_count++;
      sprintf(buf, "PRN %d: %ddB ", satz[i]>>8, (satz[i]&0xFF)>>1);
      Serial.println(buf);
    }
  }

```

This code produces an output of this form: ```PRN 21: 31dB PRN 11: 25dB PRN 71: 24dB ...``` where satellite ID and the strength are displayed for each satellite in view.
Additional notes from NMEA protocol v3.0:

> GPS satellites are identified by their PRN numbers, which range from 1 to 32.

> The numbers 65-96 are reserved for GLONASS satellites. GLONASS satellites are identified by 64+satellite slot number. The slot numbers are 1 through 24 for the full constellation of 24 satellites, this gives a range of 65 through 88. The numbers 89 through 96 are available if slot numbers above 24 are allocated to on-orbit spares.

The array is grouped by constellations with GPS first.

#### GNS mode indicator

Fix data now includes a field that shows which constellations are used when tracking, Accessible via a new ```char[] constellations()``` method that returns a char array on the following spec:
> Mode Indicator:
A variable length valid character field type with the first two characters currently defined. The first character indicates the use of GPS satellites, the second character indicates the use of GLONASS satellites. If another satellite system is added to the standard, the mode indicator will be extended to three characters, new satellite systems shall always be added on the right, so the order of characters in the Mode Indicator is: GPS, GLONASS, other satellite systems in the future.
The characters shall take one of the following values:

> * N = No fix
> * A = Autonomous mode
> * D = Differential mode
> * P = Precise mode is used to compute position fix
> * R = Real Time Kinematic
> * F = Float RTK
> * E = Estimated (dead reckoning) mode
> * M = Manual input mode
> * S = Simulator mode.

For example, a return of ```AA``` when calling ```constellations()``` means that both GPS and GLONASS are used when the device is tracking, ```AN``` means only GPS is used, etc.

#### Time and date available when the GPS is not tracking

Now the time and date are also updated even when the device is not tracking since a valid date and time is computed when enough satellites are in view. Use with caution as it may yield false date and time.

Blogged [here](http://blog.newsplore.com/2015/09/03/a-tinygps-upgrade-adding-nmea-v3-0-and-glonass-support).
