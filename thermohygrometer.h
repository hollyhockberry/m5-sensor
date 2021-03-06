// thermohygrometer.h
// Copyright (c) 2021 Inaba

#ifndef THERMOHYGROMETER_H_
#define THERMOHYGROMETER_H_

class IThermoHygrometer {
 public:
  virtual float Temperature() const = 0;
  virtual float Humidity() const = 0;
  virtual float HeatIndex() const = 0;

  virtual bool Fahrenheit() const = 0;
  virtual void Fahrenheit(bool fahrenheit) = 0;

  virtual void begin() = 0;
  virtual bool update() = 0;
};

#endif  // THERMOHYGROMETER_H_
