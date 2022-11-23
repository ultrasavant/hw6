
#include "Person.h"

Person::Person(std::string arg_SSN, std::string arg_name, GPS_DD arg_home)
{
  this->SSN = arg_SSN;
  this->name = arg_name;
  this->home = arg_home;
}

Person::Person()
{
  this->SSN = "";
  this->name = "";
  this->home = GPS_DD {};
}

void
Person::setHome
(GPS_DD arg_home)
{
  this->home = arg_home;
}

void
Person::setLocation
(GPS_DD arg_location, JvTime arg_time)
{
  this->location = arg_location;
  this->since_when = arg_time;
}

GPS_DD
Person::getHome
(void)
{
  return this->home;
}

std::string
Person::getSSN()
{
  return this->SSN;
}

std::string
Person::getName()
{
  return this->name;
}

bool
Person::operator==
(Person& aPerson)
{
  return (this->SSN == aPerson.getSSN());
}

Json::Value
Person::dump2JSON
(void)
{
  Json::Value result { };

  if (this->name != "")
    {
      result["name"] = this->name;
    }

  if (this->SSN != "")
    {
      result["SSN"] = this->SSN;
    }

  Json::Value jv_result;
  
  jv_result = (this->home).dump2JSON();
  result["home"] = jv_result;

  jv_result = (this->location).dump2JSON();
  result["location"] = jv_result;

  jv_result = (this->since_when).dump2JSON();
  result["since_when"] = jv_result;

  // std::cout << jv_result.toStyledString() << std::endl;

  return result;
}

bool
Person::JSON2Object
(Json::Value arg_jv)
{
  if ((arg_jv.isNull() == true) ||
      (arg_jv.isObject() != true))
    {
      if (arg_jv.isNull() != true)
	{
	  std::cout << arg_jv.toStyledString() << std::endl;
	}
      return false;
    }

  if (((arg_jv["SSN"]).isNull() == true) ||
      ((arg_jv["name"]).isNull() == true) ||
      ((arg_jv["SSN"]).isString() != true) ||
      ((arg_jv["name"]).isString() != true))
    {
      return false;
    }

  // we allow GPS_DD to be modified (for mobility)
  this->SSN = (arg_jv["SSN"]).asString();
  this->name = (arg_jv["name"]).asString();

  if (((arg_jv["home"]).isNull() == true) ||
      ((arg_jv["location"]).isNull() == true) ||
      ((arg_jv["home"]).isObject() != true) ||
      ((arg_jv["location"]).isObject() != true))
    {
      return false;
    }
  (this->home).JSON2Object(arg_jv["home"]);
  (this->location).JSON2Object(arg_jv["location"]);

  if (((arg_jv["since_when"]).isNull() == true) ||
      ((arg_jv["since_when"]).isObject() != true))
    {
      return false;
    }
  (this->since_when).JSON2Object(arg_jv["since_when"]);

  return true;
}

