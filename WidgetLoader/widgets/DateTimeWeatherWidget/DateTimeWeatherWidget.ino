#include "./DateTimeWeatherWidget.h"

// Icons
const unsigned char weather_icon[] = {
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

//
// ───  SETUP AND LOOP ─────────────────────────────────────────────────────────────
//

void DateTimeWeatherWidget::widget_setup()
{
	Serial.println("Time-Weather widget loaded");

	// Event listeners
	Particle.subscribe("openWeather", [this](const char *a, const char *b) { streamDataHandler(a, b); });
}

void DateTimeWeatherWidget::widget_loop()
{
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
}

//
// ─── UTIL FUNCTIONS ─────────────────────────────────────────────────────────────
//


void DateTimeWeatherWidget::streamDataHandler(const char *event, const char *data)
{
	// Allocate buffer for handling JSON, automatically destoyed after this handler finishes
	static StaticJsonBuffer<1024> jsonBuffer;

	Serial.print("Recieved event: ");
	Serial.println(event);
	if (data)
	{
		int length = strlen(data) + 1;
		char dataCopy[length];
		strcpy(dataCopy, data);
		Serial.print("Recieved data: ");
		Serial.println(dataCopy);
		JsonObject &root = jsonBuffer.parseObject(dataCopy);
		if (!root.success())
		{
			Particle.publish("parseObject() failed");
			Serial.println("parseObject() failed");
			return;
		}

		// Update JSON data into our display variables
		name = root["name"].asString();
		temperature = root["temp"].asString();
		weather_desc = root["main"].asString();
		humidity = root["humidity"].asString();
		wind = root["wind"].asString();
		Serial.println("Current temp " + temperature);
	}
	else
	{
		Serial.println("NULL");
	}
	digitalWrite(blinkLed, HIGH);
	delay(500);
	digitalWrite(blinkLed, LOW);
}

void DateTimeWeatherWidget::screenInit()
{
	Serial.println("Rendering DateTimeWeatherWidget");

	// Sync time with particle cloud
	Particle.syncTime();
	waitUntil(Particle.syncTimeDone);

	/***** Initial screen setup *****/
	tft.begin();
	tft.setRotation(1);
	tft.fillScreen(ILI9341_WHITE);
	tft.drawFastHLine(0, 50, 320, ILI9341_BLACK);
	tft.drawFastVLine(60, 0, 50, ILI9341_BLACK);

	tft.setCursor(96, 12);
	tft.setTextColor(ILI9341_BLACK);
	tft.setTextWrap(false);
	tft.setTextSize(1);
	tft.setFont(CALIBRI_24);

	tft.println("Time & Weather");
	tft.drawBitmap(6, 0, weather_icon, 48, 48, ILI9341_YELLOW);
}

void DateTimeWeatherWidget::displayWeather()
{
	
	tft.setCursor(WEATHER_START_H, WEATHER_START_V);
	tft.setTextSize(1);
	tft.setFont(CALIBRI_48);
	tft.print(temperature);
	tft.setFont(CALIBRI_18);
	tft.println(" F");

	tft.setFont(CALIBRI_24);
	tft.setCursor(WEATHER_START_H, WEATHER_START_V + 50);
	tft.println(weather_desc);

	tft.setFont(CALIBRI_18);
	tft.setCursor(WEATHER_START_H, WEATHER_START_V + 76);
	tft.println("Humidity " + humidity + "%");
	tft.setCursor(WEATHER_START_H, WEATHER_START_V + 96);
	tft.println("Wind " + wind + "mph");
	tft.setCursor(WEATHER_START_H, WEATHER_START_V - 22);
	tft.println(name);
}

String DateTimeWeatherWidget::weekdayLookup(int val)
{
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


