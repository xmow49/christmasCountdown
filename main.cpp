//------------------------------------------------------
//                https://gammatroniques.fr/
//                   Christmas Countdown
//                   12/2020 - by Xmow49
//------------------------------------------------------
#include <ESP8266WiFi.h>
#include <time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

#define OLED_RESET LED_BUILTIN
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char *ssid = "Wifi";
const char *password = "motdepasse";

#define UTC 1
#define goalH 0
#define goalM 0
#define goalS 1

#define goalD 25
#define goalMo 12

#define textBottom "Avant Noel"

#define pinLED1 D3
#define pinLED2 D4

int timer, timerD, timerH, timerM, timerS;

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connexion au Wi-Fi: ");
  display.println(ssid);

  WiFi.begin(ssid, password);
  display.println("\nConnexion");
  display.display();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    display.print(".");
    display.display();
  }
  display.clearDisplay();
  display.display();
  display.setCursor(0, 0);
  display.println("Wifi Connecte!");
  display.print("IP:");
  display.println(WiFi.localIP());
  display.display();

  configTime(UTC * 3600, 0, "fr.pool.ntp.org", "time.nist.gov");
  display.println("\nWaiting for NTP...");
  delay(2000);
  while (!time(nullptr))
  {
    delay(1000);
  }
  display.println("\nConnexion NTP....OK");
  display.display();
  delay(2000);

  display.clearDisplay();
  display.display();
  Serial.begin(115200);

  time_t now = time(nullptr);
  while (year(now) == 1970)
  {
    now = time(nullptr);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Recuperation de \nl'heure...");
    display.display();
  }
}






void displayTimer(int x, int y)
{
  display.setCursor(x, y);
  display.setFont(&FreeSans12pt7b);
  if (timerH < 10)
    display.print("0");
  display.print(timerH);
  display.setFont(&FreeSans9pt7b);
  display.print("h");
  display.setFont(&FreeSans12pt7b);
  if (timerM < 10)
    display.print("0");
  display.print(timerM);
  display.setFont(&FreeSans9pt7b);
  display.print("m");
  display.setFont(&FreeSans12pt7b);
  if (timerS < 10)
    display.print("0");
  display.print(timerS);
  display.setFont(&FreeSans9pt7b);
  display.print("s");
}

time_t get_time_t(char *date_string)
{
  tmElements_t tm;
  char ds[20];

  strncpy(ds, date_string, 20);

  tm.Second = atoi(&ds[17]);
  tm.Minute = atoi(&ds[14]);
  tm.Hour = atoi(&ds[11]);
  tm.Day = atoi(&ds[8]);
  tm.Month = atoi(&ds[5]);
  tm.Year = atoi(&ds[0]) - 1970;
  return (makeTime(tm));
}




void printText(int i)
{
  if (i == 0)
  {
    display.setFont();
    display.setCursor(30, 55);
  }
  else if (i == 1)
  {
    display.setFont(&FreeSans9pt7b);
    display.setCursor(15, 60);
  }
  display.print(textBottom);
}





void loop()
{
  time_t now = time(nullptr);

  int timerY;
  if (goalMo < month(now))
    timerY = year(now) + 1;
  else
    timerY = year(now);

  char buff[128];
  snprintf(buff, 128, "%d.%02d.%02d %02d:%02d:%02d", timerY, goalMo, goalD, goalH, goalM, goalS);

  time_t goal = get_time_t(buff);

  timer = goal - now;

  timerD = timer / 86400;
  timerH = ((timer - (timerD * 86400)) / 3600);
  timerM = (timer - ((timerD * 86400) + (timerH * 3600))) / 60;
  timerS = (timer - ((timerD * 86400) + (timerH * 3600) + (timerM * 60)));

  timerH = timerH - UTC;

  Serial.println("-------Now:------");
  Serial.println(day(now));
  Serial.println(month(now));
  Serial.println(hour(now));
  Serial.println(minute(now));
  Serial.println(second(now));
  Serial.println("--------------------");

  Serial.print("Jours Restants: ");
  Serial.println(timerD);
  Serial.print("Heures Restantes: ");
  Serial.println(timerH);
  Serial.print("Minutes Restantes: ");
  Serial.println(timerM);
  Serial.print("Secondes Restantes: ");
  Serial.println(timerS);
  Serial.println("--------------------");

  display.clearDisplay();
  display.setFont();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  if (timerD > 0)
  {
    display.setCursor(35, 15);
    display.setFont(&FreeSans12pt7b);
    display.print(timerD);
    display.setFont();
    display.print(" Jour");
    if (timerD > 1)
    {
      display.print("s");
    }
    displayTimer(0, 40);
    printText(0);
  }
  else if (timerD <= 0 && timerH > 0)
  {
    displayTimer(0, 30);
    printText(1);
  }
  else if (timerD <= 0 && timerH <= 0)
  {
    display.setCursor(20, 30);
    display.setFont(&FreeSans18pt7b);
    display.print(timerM);
    display.setFont(&FreeSans12pt7b);
    display.print("m");
    display.setFont(&FreeSans18pt7b);
    display.print(timerS);
    display.setFont(&FreeSans12pt7b);
    display.print("s");
    printText(1);
  }
  else
  {
  }

  if (timerD <= 0 && timerH < 0)
  {
    display.clearDisplay();
    display.setFont(&FreeSans12pt7b);
    display.setCursor(25, 25);
    display.print("Joyeux");
    display.setCursor(35, 55);
    display.print("Noel !");
  }
  display.display();

  analogWrite(pinLED1, random(120, 255));
  analogWrite(pinLED2, random(120, 255));

  delay(random(10, 100));
} 
