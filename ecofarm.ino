//OUTPUT PIN
#define pinFanColdSide 13
#define pinFanHotSide 12
#define pinPeltier 11
#define pinPump 10
#define pinLightRed 9
#define pinLightBlue 8

//INPUT PIN
#define pinHotLM35 A2
#define pinColdLM35 A0
#define pinSoilMoisture A1

#define pinButton A3

unsigned long time;
int lightCycle = 0;
float temperature_hot, temperature_cold;
bool hotFan = false;
bool coldFan = false;

//DATA RANGE
int hot_upper = 60;
int hot_lower = 55;
int cold_upper = 24;
int cold_lower = 20;
int moisture_limit = 100;
float hot_average = 25;
float cold_average = 25;

//status
bool button = false;

void setup(){
  Serial.begin(9600);
  time = millis();
  pinMode(pinFanColdSide, OUTPUT);
  pinMode(pinFanHotSide, OUTPUT);
  pinMode(pinPeltier, OUTPUT);
  digitalWrite(pinPeltier, HIGH);
  pinMode(pinPump, OUTPUT);
  pinMode(pinLightRed, OUTPUT);
  pinMode(pinLightBlue, OUTPUT);
  
  pinMode(pinHotLM35, INPUT);
  pinMode(pinColdLM35, INPUT);
  pinMode(pinSoilMoisture, INPUT);
}

int light(){
  int mode = lightCycle % 3;
  switch (mode){
    case 0:
      digitalWrite(pinLightRed, LOW);
      digitalWrite(pinLightBlue, LOW);
      break;
    case 1:
      digitalWrite(pinLightRed, HIGH);
      digitalWrite(pinLightBlue, HIGH);
      break;
    case 2:
      digitalWrite(pinLightRed, LOW);
      digitalWrite(pinLightBlue, HIGH);
      break;
  }
}

int temperature(){
  temperature_hot = analogRead(pinHotLM35)*4.88/15;
  temperature_cold = analogRead(pinColdLM35)*4.88/12.5;
  hot_average = ((hot_average*49)+temperature_hot)/50;
  cold_average = ((cold_average*49)+temperature_cold)/50;
  Serial.print("Hot");
  Serial.println(hot_average);
  Serial.print("Cold");
  Serial.println(cold_average);
  if (hotFan == false && hot_average < hot_lower){
    hotFan = true;
  } else if (hotFan == true && hot_average > hot_upper){
    hotFan = false;
  }
  if (coldFan == false && cold_average > cold_upper){
    coldFan = true;
  } else if (coldFan == true && cold_average < cold_lower){
    coldFan = false;
  }
  if (hotFan == true){
    digitalWrite(pinFanHotSide, HIGH);
  } else {
    digitalWrite(pinFanHotSide, LOW);
  }
  if (coldFan == true){
    digitalWrite(pinFanColdSide, HIGH);
  } else {
    digitalWrite(pinFanColdSide, LOW);
  }
}

int soilMoisture(){
  int moisture = map(analogRead(pinSoilMoisture), 0, 1023, 255, 0);
  Serial.print("Moisture");
  Serial.println(moisture);
  if (moisture < moisture_limit){
    digitalWrite(pinPump, HIGH);
    delay(5000);
    digitalWrite(pinPump, LOW);
  } else {
    digitalWrite(pinPump, HIGH);
  }
}

int buttonStatus(){
  int status = analogRead(pinButton);
  if (status == 0 && button == false){
    button = true;
    lightCycle += 1;
  } else if (status != 0 && button == true){
    button = false;
  }
}

void loop(){
  temperature();
  soilMoisture();
  buttonStatus();
  light();
  delay(50);
}
