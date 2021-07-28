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

void staple::turn(bool turn)
{
  if (turn)
  {
    digitalWrite(positive_relay_pin, HIGH);
    digitalWrite(sensor_relay_pin, HIGH);
  }
  else
  {
    digitalWrite(positive_relay_pin, LOW);
    digitalWrite(sensor_relay_pin, LOW);
  }
  
}

void staple::blinki( bool switchi)
{
  if (alert)
  {
    digitalWrite(indicator_pin, LOW);
    delay(200);
    digitalWrite(indicator_pin, HIGH);
    delay(100);
  }
  else
  {
    digitalWrite(indicator_pin, HIGH);
    delay(200);
    digitalWrite(indicator_pin, LOW);
    delay(100);
  }    
}

staple::staple(int pr_pin,int sr_pin, int s_pin, int ps_pin, int ind_pin, int br_limit)
{
  positive_relay_pin = pr_pin;
  sensor_relay_pin = sr_pin;
  sensor_pin = s_pin;
  indicator_pin = ind_pin;
  break_limit = br_limit;
  positive_sensor_pin = ps_pin;
  pinMode(positive_sensor_pin, OUTPUT);
  pinMode(positive_relay_pin, OUTPUT);
  pinMode(sensor_relay_pin, OUTPUT);
  pinMode(indicator_pin, OUTPUT);
  pinMode(sensor_pin, INPUT);
  pinMode(positive_sensor_pin, LOW);
  digitalWrite(positive_relay_pin,LOW);
  digitalWrite(sensor_relay_pin, LOW);
  digitalWrite(indicator_pin, LOW);
  raw_value = 0;
  alert = false;
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
  alert = false;
}



int staple::measureOn()
{
  delay(50);
  digitalWrite(positive_sensor_pin, HIGH);
	digitalWrite(positive_relay_pin, HIGH);
	digitalWrite(sensor_relay_pin, HIGH);
  blinki(true);
	
	raw_value = analogRead(sensor_pin);

  blinki(true);
	digitalWrite(sensor_relay_pin, LOW);
	digitalWrite(positive_relay_pin, LOW);
  digitalWrite(positive_sensor_pin, LOW);
  
  delay(50);
  if (raw_value <= break_limit)
  {
    digitalWrite(indicator_pin,HIGH);
    alert = true;
  }else
  {
    digitalWrite(indicator_pin,LOW);
    alert = false;
  }
	return raw_value;
}

int staple::measureOff()
{
  delay(50);
  blinki(true);
  raw_value = analogRead(sensor_pin);
  blinki(true);
  delay(50);
  
  return raw_value;
}

