
// ecs36b first program

#include <iostream>
#include "ecs36b_Exception.h"

// JSON RPC part
#include "hw6client.h"
#include <jsonrpccpp/client/connectors/httpclient.h>

using namespace jsonrpc;
using namespace std;

int
main()
{
  // distributed wordle demo for 11/23/2022
  HttpClient httpclient("http://127.0.0.1:8384");
  hw6Client myClient(httpclient, JSONRPC_CLIENT_V2);
  Json::Value myv;

  // obtain and new
  try {
    myv = myClient.obtain("obtain", "Wordle", "00000000");
  } catch (JsonRpcException &e) {
    cerr << e.what() << endl;
  }
  std::cout << myv.toStyledString() << std::endl;

  // guess
  if ((myv["game_id"].isNull() == false) &&
      (myv["game_id"].isString() == true))
    {
      try {
	// earth
	myv = myClient.guess("guess", "Wordle", myv["game_id"].asString(),
			     "earth");
      } catch (JsonRpcException &e) {
	cerr << e.what() << endl;
      }

      try {
	// mound
	myv = myClient.guess("guess", "Wordle", myv["game_id"].asString(),
			     "mound");
      } catch (JsonRpcException &e) {
	cerr << e.what() << endl;
      }

      try {
	// sicky
	myv = myClient.guess("guess", "Wordle", myv["game_id"].asString(),
			     "sicky");
      } catch (JsonRpcException &e) {
	cerr << e.what() << endl;
      }

      std::cout << myv.toStyledString() << std::endl;
    }
  return 0;
}
