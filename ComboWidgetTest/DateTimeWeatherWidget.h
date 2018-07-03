#ifndef DATETIMEWEATHERWIDGET_H
#define DATETIMEWEATHERWIDGET_H

#include "./WidgetLoader.h"

struct DateTimeWeatherWidget
{

  //
  // ─── CONTENT POSITIONING ────────────────────────────────────────────────────────
  //
  // Time/Date
  #define TIMEDATE_START_V 80
  #define TIMEDATE_START_H 180
  // Weather
  #define WEATHER_START_V 80
  #define WEATHER_START_H 36

  //
  // ─── GLOBAL VARIABLES ───────────────────────────────────────────────────────────
  //

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

  //
  // ─── FUNCTION DECLARATIONS ──────────────────────────────────────────────────────
  //

  void weatherHandler(const char *event, const char *data);
  void screenInit();
  void displayWeather();
  String weekdayLookup(int val);
  void widget_setup();
  void widget_loop();
};

#endif
