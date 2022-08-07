#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define LED_PIN 7
#define BUTTON_PIN 5

/*INTERPOLATION ALGO
(a.x + c * (b.x - a.x));

Interpolate between a and b by c
*/

tmElements_t tm;
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

int pulseLength = 1000;

bool SetTime = false;
bool doDebug = true;
bool alarmShown = false;
bool alarmSilenced = false;
bool resetDone = false;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Sebastian's Cat Feeding Alarm");

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if(SetTime){
    doSetTime();
  }
}

void loop() {
  checkTime();
  getInput();
  pulseCycle();
}

long next = 0;
bool led_lit = false;
void pulseCycle(){
  if(millis() > next){
    next = (long)(millis() + pulseLength);
    if(!alarmSilenced){
      led_lit = !led_lit;
    }
    else{
      led_lit = false;
    }
    if(doDebug)
      debug();
  }

  if(alarmShown){
    if(led_lit)
      digitalWrite(LED_PIN, HIGH);
    else
      digitalWrite(LED_PIN, LOW);
  }
}

void debug(){
  //READ TIME
  if (RTC.read(tm)) {
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.println();
  }
}

void getInput(){
  if(digitalRead(BUTTON_PIN) == 0 && alarmShown && !alarmSilenced){
    alarmSilenced = true;
    Serial.println("ALARM SILENCED");
  }
}

void checkTime()
{
  if(RTC.read(tm))
  {
    if(tm.Hour == 7 && !resetDone){
      alarmShown = false;
      alarmSilenced = false;
      resetDone = true;
      Serial.println("IT IS 7 AM");
    }

    if(tm.Hour == 8 && resetDone){
      alarmShown = true;
      resetDone = false;
      Serial.println("IT IS 8 AM");
    }
  }
}









//BLACKBOXED
void doSetTime(){
  bool parse=false;
  bool config=false;
  
  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }
  Serial.println("TIME SET");
}
bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}
bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
