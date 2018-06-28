
/***************************************************
This is an example sketch for the Adafruit 2.2" SPI tft.
This library works with the Adafruit 2.2" TFT Breakout w/SD card
----> http://www.adafruit.com/products/1480

Check out the links above for our tutorials and wiring diagrams
These tfts use SPI to communicate, 4 or 5 pins are required to
interface (RST is optional)
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution
****************************************************/

// #include "./Adafruit_mfGFX/Adafruit_mfGFX.h"
#include "./Adafruit_ILI9341/Adafruit_ILI9341.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(A2, A1, A0);

String current_time;
bool toggle;
String format_string;
String weekday;
String date;
unsigned long uptime;
unsigned long ms = 1000;

//
// ─── CONTENT POSITIONING ────────────────────────────────────────────────────────
//
// Time/Date
#define TIMEDATE_START_V 90
#define TIMEDATE_START_H 160
#define TIMEDATE_LINE_HEIGHT 20
// Weather
#define WEATHER_START_V 90
#define WEATHER_START_H 36

const unsigned char weather[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0,
		0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,
		0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x06, 0x00,
		0x00, 0xf0, 0x00, 0x00, 0x0f, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x78, 0x00, 0x00,
		0x1e, 0x00, 0x00, 0x30, 0x07, 0xe0, 0x0c, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00,
		0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x1f, 0x80, 0x00,
		0x00, 0x03, 0xe0, 0x07, 0xc0, 0x00, 0x00, 0x07, 0xc0, 0x03, 0xe0, 0x00, 0x00, 0x07, 0x80, 0x01,
		0xe0, 0x00, 0x00, 0x07, 0x80, 0x01, 0xe0, 0x00, 0x00, 0x0f, 0x00, 0x00, 0xf0, 0x00, 0x3f, 0x0f,
		0x00, 0x00, 0xf0, 0xfc, 0x3f, 0x0f, 0x00, 0x00, 0xf0, 0xfc, 0x3f, 0x0f, 0x00, 0x00, 0xf0, 0xfc,
		0x3f, 0x0f, 0x00, 0x00, 0xf0, 0xfc, 0x00, 0x0f, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x07, 0x80, 0x01,
		0xe0, 0x00, 0x00, 0x07, 0x80, 0x01, 0xe0, 0x00, 0x00, 0x07, 0xc0, 0x03, 0xe0, 0x00, 0x00, 0x03,
		0xe0, 0x07, 0xc0, 0x00, 0x00, 0x01, 0xf8, 0x1f, 0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
		0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x30, 0x07, 0xe0,
		0x0c, 0x00, 0x00, 0x78, 0x00, 0x00, 0x1e, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x1f, 0x00, 0x00, 0xf0,
		0x00, 0x00, 0x0f, 0x00, 0x00, 0x60, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00,
		0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0,
		0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup(){// Request time synchronization from the Particle Device Cloud
	Particle.syncTime();
	// Wait until Photon receives time from Particle Device Cloud (or connection to Particle Device Cloud is lost)
	waitUntil(Particle.syncTimeDone);

	tft.begin();
	tft.setRotation(1);
	titleText();
	tft.drawBitmap(6, 0, weather, 48, 48, ILI9341_YELLOW);
	displayWeather();
	tft.drawFastHLine(0, 50, 320, ILI9341_BLACK);
	tft.drawFastVLine(60, 0, 50, ILI9341_BLACK);

}

void loop(void) {
	format_string = "%I:%M %p";

	Time.zone(-5);
	if (Time.isDST())
	{
		Time.beginDST();
	}
	else
	{
		Time.endDST();
	}
	current_time = Time.format(Time.now(), format_string);
	uptime = millis() / ms;

	// render just what changed (time)
	tft.setFont(CALIBRI_18);
	tft.setTextSize(1);
	tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
	tft.setCursor(TIMEDATE_START_H, TIMEDATE_START_V);
	tft.println(current_time);
	tft.setCursor(TIMEDATE_START_H, (TIMEDATE_START_V + TIMEDATE_LINE_HEIGHT * 1));
	weekday = weekdayLookup(Time.weekday());
	tft.println(weekday);
	tft.setCursor(TIMEDATE_START_H, (TIMEDATE_START_V + TIMEDATE_LINE_HEIGHT * 2));
	tft.println(String(Time.month()) + "/" + String(Time.day()) + "/" + String(Time.year()));
	tft.setCursor(TIMEDATE_START_H, (TIMEDATE_START_V + TIMEDATE_LINE_HEIGHT * 3));
	tft.println("Uptime: " + String(uptime) + "s");
	tft.println("");

	delay(500);

}

void titleText()
{
	tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(96, 12);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(1);
	tft.setFont(CALIBRI_24);
	tft.println("Time & Weather");
}

void displayWeather()
{
	tft.setCursor(WEATHER_START_H, WEATHER_START_V);
	tft.setTextSize(1);
	tft.setFont(CALIBRI_48);
	tft.println("92");
	tft.setFont(CALIBRI_24);
	tft.setCursor(WEATHER_START_H, WEATHER_START_V + 50);
	tft.println("Sunny");
}

String weekdayLookup(int val) {
	switch (val)
	{
		case 1:
			weekday = "Sunday";
			break;
		case 2:
			weekday = "Monday";
			break;
		case 3:
			weekday = "Tuesday";
			break;
		case 4:
			weekday = "Wednesday";
			break;
		case 5:
			weekday = "Thursday";
			break;
		case 6:
			weekday = "Friday";
			break;
		case 7:
			weekday = "Saturday";
			break;
		default: 
			weekday = "Null";
			break;
	}
	return weekday;
}
