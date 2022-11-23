
#ifndef _PERSON_H_
#define _PERSON_H_

// Person.h

#include "ecs36b_Common.h"
#include "Core.h"
#include "GPS.h"
#include "JvTime.h"

class Person : public Core
{
private:
public:

  std::string SSN;
  std::string name;
  GPS_DD home;
  GPS_DD location;
  JvTime since_when;
  
  Person(std::string, std::string, GPS_DD);
  Person();
  void setHome(GPS_DD);
  void setLocation(GPS_DD, JvTime);

  bool operator==(Person& aPerson);
  std::string getSSN();
  std::string getName();
  GPS_DD getHome();
  GPS_DD getLocation();
  JvTime getLocationTime();
  virtual Json::Value dump2JSON();
  virtual bool JSON2Object(Json::Value);
};

#endif  /* _PERSON_H_ */
