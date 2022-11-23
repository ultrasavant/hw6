
#ifndef _CORE_H_
#define _CORE_H_

#include "ecs36b_Common.h"

#include "GPS.h"
#include "JvTime.h"

// Core.h

using namespace std;

class Check_Point
{
private:
protected:
public:
  GPS_DD location;
  JvTime time;
  Check_Point(void);
  virtual Json::Value dump2JSON();
  virtual bool JSON2Object(Json::Value);
};

class Core
{
 private:
 public:
  std::vector <Check_Point> checkpoints;
  Core(void);
  virtual Json::Value dump2JSON();
  virtual bool JSON2Object(Json::Value);
};

#endif  /* _CORE_H_ */
