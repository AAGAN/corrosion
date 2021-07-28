#pragma once
#ifndef id77A91382_344C_4B08_917B09C346FE6A49
#define id77A91382_344C_4B08_917B09C346FE6A49
#include "Arduino.h"
class staple
{
private:
	int positive_relay_pin;
	int sensor_relay_pin;
  int positive_sensor_pin; 
	int sensor_pin;
	int raw_value;
	int adjusted_value;
  int indicator_pin;
  int break_limit;// below this value, the indicator led will go on.
  bool alert;
public:
	//staple(){};
	//~staple(){};
	//staple(int pr_pin,int sr_pin, int s_pin, int ind_pin, int br_limit);//for staples
  staple(int pr_pin,int sr_pin, int s_pin, int ps_pin, int ind_pin, int br_limit); 
  staple(int pr_pin,int sr_pin, int s_pin);
	void set_raw_value(int val){raw_value = val;}
  int get_raw_value(){return raw_value;}
	int measureOn();
  int measureOff();
  void blinki();
  void blinki(bool);
  void turn(bool);
};
#endif 
