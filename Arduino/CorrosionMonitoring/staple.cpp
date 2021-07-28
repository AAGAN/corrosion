#include "staple.h"

extern int blinkNum;

void staple::blinki()
{
  
  for (int i = 1 ; i < blinkNum ; i++)
  {
    digitalWrite(indicator_pin, HIGH);
    delay(20/blinkNum);
    digitalWrite(indicator_pin, LOW);
    delay(80/blinkNum);
  }
}

staple::staple(int pr_pin,int sr_pin, int s_pin, int ind_pin, int br_limit)
{
	positive_relay_pin = pr_pin;
	sensor_relay_pin = sr_pin;
	sensor_pin = s_pin;
  indicator_pin = ind_pin;
  break_limit = br_limit;
  pinMode(positive_relay_pin, OUTPUT);
  pinMode(sensor_relay_pin, OUTPUT);
  pinMode(indicator_pin, OUTPUT);
  pinMode(sensor_pin, INPUT);
  digitalWrite(positive_relay_pin,LOW);
  digitalWrite(sensor_relay_pin, LOW);
  digitalWrite(indicator_pin, LOW);
	raw_value = 0;
}

staple::staple(int pr_pin,int sr_pin, int s_pin)//this constructor is only for resistive sensor pins, not staples
{
  positive_relay_pin = pr_pin;
  sensor_relay_pin = sr_pin;
  sensor_pin = s_pin;
  //indicator_pin = ind_pin;
  //break_limit = br_limit;
  pinMode(positive_relay_pin, OUTPUT);
  pinMode(sensor_relay_pin, OUTPUT);
  pinMode(indicator_pin, OUTPUT);
  pinMode(sensor_pin, INPUT);
  digitalWrite(positive_relay_pin,LOW);
  digitalWrite(sensor_relay_pin, LOW);
  digitalWrite(indicator_pin, LOW);
  raw_value = 0;
  break_limit = 0;
  indicator_pin = 50;
}

int staple::measure()
{
  delay(50);
	digitalWrite(positive_relay_pin, HIGH);
	digitalWrite(sensor_relay_pin, HIGH);
  blinki();
	//delay(100);
	raw_value = analogRead(sensor_pin);
	//delay(100);
	digitalWrite(sensor_relay_pin, LOW);
	digitalWrite(positive_relay_pin, LOW);
  blinki();
  delay(50);
  if (raw_value <= break_limit)
  {
    digitalWrite(indicator_pin,HIGH);
  }else
  {
    digitalWrite(indicator_pin,LOW);
  }
  
	return raw_value;
}
