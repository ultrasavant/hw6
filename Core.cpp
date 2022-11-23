
#include "Core.h"

Check_Point::Check_Point(void)
{
}

Json::Value
Check_Point::dump2JSON
()
{
  Json::Value result;
  Json::Value l_jv;
  l_jv = (this->location).dump2JSON();
  result["location"] = l_jv;

  l_jv = (this->time).dump2JSON();
  result["time"] = l_jv;

#ifdef _ECS36B_DEBUG_
  std::cout << result.toStyledString() << std::endl;
#endif /* _ECS36B_DEBUG_ */

  return result;
}

bool
Check_Point::JSON2Object
(Json::Value arg_jv)
{
  if ((arg_jv.isNull() == true) ||
      (arg_jv.isObject() != true))
    {
      return false;
    }

  if (((arg_jv["location"]).isNull() == true) ||
      ((arg_jv["time"]).isNull() == true) ||
      ((arg_jv["location"]).isObject() != true) ||
      ((arg_jv["time"]).isObject() != true))
    {
      return false;
    }
  (this->location).JSON2Object(arg_jv["location"]);
  (this->time).JSON2Object(arg_jv["time"]);

  return true;
}

Core::Core(void)
{
}

Json::Value
Core::dump2JSON
()
{
  int i;
  Json::Value result;
  Json::Value l_jv;

  if ((this->checkpoints).size() > 0)
    {
      Json::Value json_checkpoints;
      Json::Value json_checkpoints_array;
      for (i = 0; i < (this->checkpoints).size(); i++)
        {
          l_jv = ((this->checkpoints)[i]).dump2JSON();
	  // l_jv ==>
	  // {"location":{<GPS JSON Object>},"time":{<JvTime JSON Object>}}

          json_checkpoints[i] = l_jv;
	  // l_jv ==>
	  // {"location":{<GPS JSON Object>},"time":{<JvTime JSON Object>}}
	  // <abc>

	  // json_checkpoints [<abc>, <abc>,... <abc>...]
	  // json_checkpoints_array
	  // {"data":[<abc>, <abc>,... <abc>...], "count":234}
	  // result
	  // {"checkpoints": {"data":[<abc>, <abc>,... <abc>...], "count":234}}

        }
      json_checkpoints_array["data"] = json_checkpoints;
      json_checkpoints_array["count"] = ((int) (this->checkpoints).size());
      result["checkpoints"] = json_checkpoints_array;
    }

#ifdef _ECS36B_DEBUG_
  std::cout << result.toStyledString() << std::endl;
#endif /* _ECS36B_DEBUG_ */

  return result;
}

bool
Core::JSON2Object
(Json::Value arg_jv)
{
  int i;
  // {"checkpoints": {"data":[<abc>, <abc>,... <abc>...], "count":234}}
  if ((arg_jv.isNull() == true) ||
      (arg_jv.isObject() != true))
    {
      return false;
    }
  
  // {"checkpoints": {"data":[<abc>, <abc>,... <abc>...], "count":234}}
  if (((arg_jv["checkpoints"]).isNull() == true) ||
      ((arg_jv["checkpoints"]).isObject() != true) ||
      ((arg_jv["checkpoints"]["data"]).isNull() == true) ||
      ((arg_jv["checkpoints"]["data"]).isArray() != true))
    {
      return false;
    }

  // {"checkpoints": {"data":[<abc>, <abc>,... <abc>...], "count":234}}
  for (i = 0; i < (arg_jv["checkpoints"]["data"]).size(); i++)
    {
      Json::Value l_jv = arg_jv["checkpoints"]["data"][i]; // <abc>
      Check_Point l_cp {};
      bool rc = l_cp.JSON2Object(l_jv);
      
      vector<Check_Point>::iterator my_it_cp;
      int flag_cp = 0;
      for (my_it_cp = (this->checkpoints).begin();
	   my_it_cp < (this->checkpoints).end(); my_it_cp++)
	{
	  if (((my_it_cp)->location == l_cp.location) &&
	      ((my_it_cp)->time == l_cp.time))
	    {
	      flag_cp = 1;
	    }
	}

      if (flag_cp == 0)
	{
	  // new one
	  (this->checkpoints).push_back(l_cp);
	}
    }

  return true;
}

