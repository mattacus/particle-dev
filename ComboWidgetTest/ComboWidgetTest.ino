#include "./ArduinoJson-v5.13.2.h"
#include "./Adafruit_ILI9341/Adafruit_ILI9341.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(A2, A1, A0);

//
// ─── CONTENT POSITIONING ────────────────────────────────────────────────────────
//

// Time/Date
#define TIMEDATE_START_V 80
#define TIMEDATE_START_H 180
// Weather
#define WEATHER_START_V 80
#define WEATHER_START_H 36

// Stocks
#define SYMBOL_START_V 72
#define SYMBOL_START_H 112
#define PRICE_START_V 144
#define PRICE_START_H 112

// Widget enums
#define TIMEWEATHERWIDGET 0
#define STOCKSWIDGET 1

//
// ─── GLOBAL VARIABLES ───────────────────────────────────────────────────────────
//

// Widget selector
int widgetView = TIMEWEATHERWIDGET;

// Memory pool for JSON object tree.
StaticJsonBuffer<200> jsonBuffer;

// Time/Weather
String temperature = "...";
String weather_desc = "...";
String humidity = "...";
String wind = "...";
String current_time;
bool toggle;
String format_string;
String weekday;
String date;
int blinkLed = D7;

// Stocks
String symbol = "null";
String price = "0";


// Icons
const unsigned char weatherIcon[] = {
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
const unsigned char stocksIcon[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0xe0, 0x00, 0x00, 0x00,
		0x3f, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0xe0, 0x00,
		0x00, 0x00, 0x0f, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x3f, 0xf8,
		0xe0, 0x00, 0x00, 0x00, 0x7f, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0xff, 0xf8, 0xe0, 0x00, 0x10, 0x01,
		0xff, 0x38, 0xe0, 0x00, 0x38, 0x03, 0xfe, 0x18, 0xe0, 0x00, 0x7c, 0x07, 0xfc, 0x00, 0xe0, 0x00,
		0xfe, 0x0f, 0xf8, 0x00, 0xe0, 0x01, 0xff, 0x1f, 0xf0, 0x00, 0xe0, 0x03, 0xff, 0xbf, 0xe0, 0x00,
		0xe0, 0x07, 0xff, 0xff, 0xc0, 0x00, 0xe0, 0x0f, 0xff, 0xff, 0x80, 0x00, 0xe0, 0x1f, 0xff, 0xff,
		0x00, 0x00, 0xe0, 0x3f, 0xef, 0xfe, 0x00, 0x00, 0xe0, 0x7f, 0xc7, 0xfc, 0x00, 0x00, 0xe0, 0xff,
		0x83, 0xf8, 0x00, 0x00, 0xe1, 0xff, 0x01, 0xf0, 0x00, 0x00, 0xe3, 0xfe, 0x00, 0xe0, 0x00, 0x00,
		0xe3, 0xfc, 0x00, 0x40, 0x00, 0x00, 0xe1, 0xf8, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf0, 0x00, 0x00,
		0x00, 0x00, 0xe0, 0x60, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//
// ─── DEVICE SETUP ──────────────────────────────────────────────────────────────────────
//

void setup() {
	Serial.begin(9600);
	while (!Serial)
	{
		// wait serial port initialization
	}
	Serial.println("Hello World!");

	// Event listeners
	Particle.subscribe("openWeather", weatherHandler);
	Particle.subscribe("stocks", stocksHandler);

	// Sync time with particle cloud
	Particle.syncTime();
	waitUntil(Particle.syncTimeDone);

	pinMode(blinkLed, OUTPUT);

	// Initial screen setup
	tft.begin();
	tft.setRotation(1);
	tft.fillScreen(ILI9341_WHITE);

	// Draw top header bar
	drawHeader();
}

//
// ─── MAIN LOOP ──────────────────────────────────────────────────────────────────
//

void loop(void) {

	if (widgetView = TIMEWEATHERWIDGET) {
		format_string = "%I:%M   %p";

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

		displayWeather();
		tft.setTextSize(1);
		tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
		tft.setCursor(TIMEDATE_START_H, TIMEDATE_START_V);
		tft.setFont(CALIBRI_24);
		tft.println(current_time);

		tft.setFont(CALIBRI_18);
		tft.setCursor(TIMEDATE_START_H, (TIMEDATE_START_V + 26));
		weekday = weekdayLookup(Time.weekday());
		tft.println(weekday);
		tft.setCursor(TIMEDATE_START_H, (TIMEDATE_START_V + 46));
		tft.println(String(Time.month()) + "/" + String(Time.day()) + "/" + String(Time.year()));

		delay(500);
	}

	else if (widgetView = STOCKSWIDGET) {
		// render just what changed (time)
		tft.setTextSize(1);
		tft.setFont(CALIBRI_48);
		tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);

		// Stock symbol
		tft.setCursor(SYMBOL_START_H, SYMBOL_START_V);
		tft.println(symbol);
		// Stock price
		tft.setCursor(PRICE_START_H, PRICE_START_V);
		tft.println(price);

		delay(500);
	}
}

//
// ─── UTIL FUNCTIONS ─────────────────────────────────────────────────────────────
//

void weatherHandler(const char *event, const char *data)
{
	Serial.print("Got an event!");
	Serial.print(event);
	if (data) {
		int length = strlen(data) + 1;
		char dataCopy[length];
		strcpy(dataCopy, data);
		Serial.println(dataCopy);
		JsonObject &root = jsonBuffer.parseObject(dataCopy);
		if (!root.success())
		{
			Particle.publish("parseObject() failed");
			return;
		}

		// Update JSON data into our display variables
		temperature = root["temp"].asString();
		weather_desc = root["main"].asString();
		humidity = root["humidity"].asString();
		wind = root["wind"].asString();
		Serial.println("Current temp " + temperature);

		// Clear JSON buffer for reuse
		jsonBuffer.clear();
	}
	else {
		Serial.println("NULL");
	}
	digitalWrite(blinkLed, HIGH);
	delay(500);
	digitalWrite(blinkLed, LOW);
}

void stocksHandler(const char *event, const char *data)
{
	Serial.print("Recieved event: ");
	Serial.println(event);
	if (data)
	{
		int length = strlen(data) + 1;
		char dataCopy[length];
		strcpy(dataCopy, data);
		Serial.println(dataCopy);
		JsonObject &root = jsonBuffer.parseObject(dataCopy);
		if (!root.success())
		{
			Particle.publish("parseObject() failed");
			Serial.println("parseObject() failed");
			return;
		}

		// Update JSON data into our display variables
		symbol = root["Symbol"].asString();
		price = root["Price"].asString();
		Serial.println("Current price " + price);

		// Clear JSON buffer for reuse
		jsonBuffer.clear();
	}
	else
	{
		Serial.println("NULL");
	}
	digitalWrite(blinkLed, HIGH);
	delay(500);
	digitalWrite(blinkLed, LOW);
}

void drawHeader()
{
	// Don't edit these
  tft.setCursor(96, 12);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(1);
	tft.setFont(CALIBRI_24);
	tft.drawFastHLine(0, 50, 320, ILI9341_BLACK);
	tft.drawFastVLine(60, 0, 50, ILI9341_BLACK);

	// Edit here
	if (widgetView = TIMEWEATHERWIDGET) {
		tft.println("Time & Weather");
		tft.drawBitmap(6, 0, weatherIcon, 48, 48, ILI9341_YELLOW);
	} else if (widgetView = STOCKSWIDGET) {
		tft.println("   Stock    Updates");
		tft.drawBitmap(6, 0, stocksIcon, 48, 48, ILI9341_GREEN);
	}
}

void displayWeather()
{
	tft.setCursor(WEATHER_START_H, WEATHER_START_V);
	tft.setTextSize(1);
	tft.setFont(CALIBRI_48);
	tft.print(temperature);
	tft.setFont(CALIBRI_18);
	tft.println(" F");
	// tft.setCursor(WEATHER_START_H + 58, WEATHER_START_V);

	tft.setFont(CALIBRI_24);
	tft.setCursor(WEATHER_START_H, WEATHER_START_V + 50);
	tft.println(weather_desc);

	tft.setFont(CALIBRI_18);
	tft.setCursor(WEATHER_START_H, WEATHER_START_V + 76);
	tft.println("Humidity " + humidity + "%");
	tft.setCursor(WEATHER_START_H, WEATHER_START_V + 96);
	tft.println("Wind " + wind + "mph");
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
