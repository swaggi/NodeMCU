// Interrupt needs to be at the top
bool alarm = false;
bool message = false;
void ICACHE_RAM_ATTR motionDetection() {
  if (alarm == true){
    message = true;
  }
}

// Libraries
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <Wire.h>
#include "DHT.h"
#include "SSD1306Wire.h"
#include <UniversalTelegramBot.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <WiFiClientSecure.h>

// Connect motion detector to pin D3
int motion = D3;

// Connect AM2302 to pin D4
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22
uint8_t DHTPin = D4;
DHT dht(DHTPin, DHTTYPE);
float h;
float t;

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, D2, D1);  //D2=SDK  D1=SCK  As per labeling on NodeMCU

// set Wi-Fi SSID and password
const char *ssid     = "INSERT_YOUR_SSID_HERE";
const char *password = "INSERT_YOUR_WIFI_PASSWORD_HERE";
int wifi_duration = 0;

// Telegram settings
#define BOT_TOKEN "111111111:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" // your Bot Token (Get from Botfather) 
#define CHAT_ID "-1111111111111" // Chat ID of where you want the message to go (You can use MyIdBot to get the chat ID)
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// NTP settings
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.nist.gov", 7200, 60000);
char Time[] = "  :  ";
char Date[] = "  .  .20  ";
byte last_second, last_minute, second_, minute_, hour_, wday, day_, month_, year_;

// global delay timer
int _delay = 3000;


// SETUP ROUTINE

void setup() {
  delay(1000);
  // Initialize temperature sensor
  pinMode(DHTPin, INPUT);
  dht.begin();
  Serial.begin(115200);  
  Serial.println("Starting...");

  // Initialize motion sensor
  pinMode(motion, INPUT);
  attachInterrupt(digitalPinToInterrupt(motion), motionDetection, RISING);

  // Initialize OLED display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Connecting");
  display.drawString(0, 20, "to WiFi");
  display.display();

  // Initialize WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);
  
  while ( WiFi.status() != WL_CONNECTED && wifi_duration < 600) // Wait 1 minute before continue without WiFi
  {
    delay(100);
    Serial.print(".");
    wifi_duration = wifi_duration + 1;
  }
  if (wifi_duration > 98)
  {
    display.clear();
    display.drawString(0, 0, "Not connected!");
    display.display();
  }
  else
  {
    display.clear();
    display.drawString(0, 0, "Connected!");
    display.display();
  }
  
  // Initialize NTP client
  timeClient.begin();
  delay(500);
  
  client.setInsecure();
}

// MAIN PROGRAM

void loop() {
  // Temperature & humidity
  h = dht.readHumidity(); // Read out humidity
  t = dht.readTemperature(); // Read out temperature

  // Alarming
  IPAddress ip (192, 168, 178, XXX); // Insert Smartphone IP address here (needs to be static or ip address reservation by your DHCP server / router)
  bool ret = Ping.ping(ip);
  // If smartphone is pingable, alarming is inactive and information will be shown on the OLED
  if (ret)
  {
    alarm = false;
    // Time
    if (WiFi.status() == WL_CONNECTED)
    {
      display_time();
      delay(_delay);
    }
    else 
    {
      wifi_duration = 100;
    }
    
    display_temperature();
    delay(_delay);
    display_humidity();
  }
  else if (!ret && wifi_duration > 98) // Phone not reachable and WiFi not connected -> display temperature & humidity
  {
    display_NoWiFi();
    delay(_delay);
    display_temperature();
    delay(_delay);
    display_humidity();
    delay(_delay);
  }
  // If WiFi is active and smartphone is not connected to WiFi, alarming is active. Telegram message will be fired, if motion is recognized.
  else
  {
    alarm = true;
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(10, 10, "Alarming");
    display.drawString(30, 30, "active");
    display.display();
    if (message == true)
    {
      sendTelegram();
      message = false;
    }
  }
}
//=========================================================================

void display_time() {
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();
  second_ = second(unix_epoch);
  if (last_second != second_)
  {
    minute_ = minute(unix_epoch);      // get minutes (0 - 59)
    hour_   = hour(unix_epoch);        // get hours   (0 - 23)
    wday    = weekday(unix_epoch);     // get minutes (1 - 7 with Sunday is day 1)
    day_    = day(unix_epoch);         // get month day (1 - 31, depends on month)
    month_  = month(unix_epoch);       // get month (1 - 12 with Jan is month 1)
    year_   = year(unix_epoch) - 2000; // get year with 4 digits - 2000 results 2 digits year (ex: 2018 --> 18)
 
    //Time[7] = second_ % 10 + '0';
    //Time[6] = second_ / 10 + '0';
    Time[4] = minute_ % 10 + '0';
    Time[3] = minute_ / 10 + '0';
    Time[1] = hour_   % 10 + '0';
    Time[0] = hour_   / 10 + '0';
    Date[9] = year_   % 10 + '0';
    Date[8] = year_   / 10 + '0';
    Date[4] = month_  % 10 + '0';
    Date[3] = month_  / 10 + '0';
    Date[1] = day_    % 10 + '0';
    Date[0] = day_    / 10 + '0';

    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 0, String(Date));
    display.drawString(30, 30, String(Time));
    display.display();

    last_second = second_;
  }
}
void display_temperature() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 0, "Temperature");
  display.setFont(ArialMT_Plain_24);
  display.drawString(20, 30, String(t) + " C");
  display.display();
}
void display_humidity() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 0, "Humidity");
  display.setFont(ArialMT_Plain_24);
  display.drawString(20, 30, String(h) + " %");
  display.display();
}
void display_NoWiFi() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(40, 0, "No");
  display.setFont(ArialMT_Plain_24);
  display.drawString(30, 30, "WiFi");
  display.display();
}
void sendTelegram() {
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(20, 30, "ALARM!!");
  display.display();
  bot.sendMessage(CHAT_ID, "Alarm", "");
}
