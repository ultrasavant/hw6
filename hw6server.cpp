
// for Json::value
#include <json/json.h>
#include <json/reader.h>
#include <json/writer.h>
#include <json/value.h>
#include <string>

// for JsonRPCCPP
#include <iostream>
#include "hw6server.h"
#include <jsonrpccpp/server/connectors/httpserver.h>
#include "hw6client.h"
#include <jsonrpccpp/client/connectors/httpclient.h>

// ecs36b
#include "ecs36b_Common.h"
#include "JvTime.h"

using namespace jsonrpc;
using namespace std;

#define WORD_LENGTH 5
#define ALPHA_SIZE 27
FILE *fpAll,  *fpSol;

long wordCount = 0;
char word[WORD_LENGTH + 1] = {0};
char alpha[ALPHA_SIZE] = "abcdefghijklmnopqrstuvwxyz";

// from wordle.c, some of them might be unused

int pickWord(char * word, int index);
bool hasWord(const char * word);
int toLower(char * str);
bool checkWord(const char * guess);
bool isWord(const char* word);
int strpos(const char * str, char search);

char check_result[WORD_LENGTH + 1];

bool
checkWord
(const char *arg_guess)
{
  if (strlen(arg_guess) == strlen(word))
    {
      int i = 0;
      int pos = -1;
      //Copy is used to blank found characters
      //This avoids wrong reports for double letters, for example "l" in "balls"
      char lv_copy[WORD_LENGTH + 1];
      char lv_result[WORD_LENGTH + 1];
      lv_result[WORD_LENGTH] = 0;
      strcpy(lv_copy, word);
      //Do all correct positions first
      while (lv_copy[i])
	{
	  if (lv_copy[i] == arg_guess[i])
	    {
	      //Character found and position correct
	      lv_result[i] = '#';
	      lv_copy[i] = '_';
	    }
	  else
	    {
	      //Fills remaining slots with blanks
	      //We could do this before the loop as well
	      lv_result[i] = '_';
	    }
	  ++i;
	}
      i = 0;

      while (lv_copy[i])
	{
	  pos = strpos(lv_copy, arg_guess[i]);
	  //Char must exist but do not overwrite a good guess
	  if (pos >= 0 && lv_result[i] != '#')
	    {
	      //Character found but position wrong
	      lv_result[i] = 'o';
	      lv_copy[pos] = '_';
	    }
	  ++i;
	}
      snprintf(check_result, WORD_LENGTH + 1, "%s", lv_result);
      return true;
    }

  return false;
}

int
toLower(char *str)
{
  int i = 0;
  while (str[i]) {
    if (str[i] >= 'A' && str[i] <= 'Z')
      {
	str[i] |= 0x20; //Make lowercase
      }
    ++i;
  }
  return i;
}

int
strpos
(const char * str, char search)
{
  int i = 0;
  if (str != NULL)
    {
      while (str[i])
	{
	  if (str[i] == search)
	    {
	      return i;
	    }
	  ++i;
	}
    }
  return -1;
}

bool
isWord(const char* arg_word)
{
  int i = -1;
  // std::cout << "isWord = " << arg_word << std::endl;
  
  if(strlen(arg_word) == WORD_LENGTH)
    {
      while(arg_word[++i])
	{
	  if(arg_word[i]<'a' || arg_word[i]>'z')
	    {
	      return false;
	    }
	}
      return true;
    }
  return false;
}

bool
hasWord
(const char * arg_word)
{
  //A bit longer to also contain the line terminator
  char buffer[WORD_LENGTH + 4];

  // std::cout << "hasWord = " << arg_word << std::endl;

  //Don't bother if the argument is invalid
  if (arg_word == NULL || strlen(arg_word) != WORD_LENGTH || !isWord(arg_word))
    {
      return false;
    }
  
  fseek(fpAll, 0, SEEK_SET);
  //Stop comparing once we are beyond the current letter
  while (fgets(buffer, WORD_LENGTH + 4, fpAll) != NULL)
    {
      buffer[WORD_LENGTH]=0;
      // std::cout << "buffer = " << buffer << std::endl;

      if (strcmp(arg_word, buffer) == 0)
	{
	  return true;
	}
    }
  return false;
}

int
pickWord
(char * word, int index)
{
  int i = 0;
  fseek(fpSol, 0, SEEK_SET);
  while (i <= index && fgets(word, WORD_LENGTH + 1, fpSol) != NULL)
    {
      if (strlen(word) == WORD_LENGTH)
	{
	  ++i;
	}
    }
  return index;
}

class Myhw6Server : public hw6Server
{
public:
  Myhw6Server(AbstractServerConnector &connector, serverVersion_t type);
  virtual Json::Value set_name(const std::string& action, const Json::Value& student_list, const std::string& team_name);
  virtual Json::Value obtain(const std::string& action, const std::string& class_id, const std::string& game_id);
  virtual Json::Value guess(const std::string& action, const std::string& class_id, const std::string& game_id, const std::string& my_guess);
  virtual Json::Value submit(const std::string& action, const std::string& game_id, const std::string& team_name);
};

Myhw6Server::Myhw6Server(AbstractServerConnector &connector, serverVersion_t type)
  : hw6Server(connector, type)
{
  std::cout << "Myhw6Server Object created" << std::endl;
}

// member functions

Json::Value
Myhw6Server::set_name
(const std::string& action, const Json::Value& student_list,
 const std::string& team_name)
{
  int i;
  int error_code = 0;
  
  int rc1 = 0;
  int rc2 = 0;
  Json::Value result;
  std::string strJson;

  char buf_fname1[256];
  bzero(buf_fname1, 256);
  snprintf(buf_fname1, 255, "./config/ecs36b_teams.json");

  char buf_fname2[256];
  bzero(buf_fname2, 256);
  snprintf(buf_fname2, 255, "./config/ecs36b_students.json");

  Json::Value lv_teams;
  Json::Value lv_students;
  
  rc1 = myFile2JSON(buf_fname1, &lv_teams);
  rc2 = myFile2JSON(buf_fname2, &lv_students);

  // std::cout << "RC1 = " << rc1 << "RC2 = " << rc2 << "\n";

  if ((rc1 == 0) && (rc2 == 0) && (team_name.empty() == false) &&
      ((lv_teams.isNull() == true) ||
       ((lv_teams.isNull() == false) && (lv_teams.isObject() == true))) &&
      (lv_students.isNull() == false) && (lv_students.isObject() == true) &&
      (student_list.isNull() == false) && (student_list.isArray() == true) &&
      (student_list.size() > 0))
    {
      if ((lv_teams[team_name]).isNull() == true)
	{
	  // set up a new team
	  // check if the team members have been registered with other team
	  int lv_list_check = 0;
	  Json::Value lv_jv;
	  for (i = 0; i < student_list.size(); i++)
	    {
	      if ((student_list[i]).isString() == false)
		{
		  lv_list_check = 1;
		}
	      else
		{
		  lv_jv = lv_students[(student_list[i]).asString()];
		  if ((lv_jv.isNull() == true) ||
		      (lv_jv.isObject() == false) ||
		      (lv_jv["team name"].isNull() == true) ||
		      (lv_jv["team name"].isString() == false) ||
		      (lv_jv["team name"]).asString() != "unassigned")
		    {
		      lv_list_check = 1;
		    }
		}
	    }
	  
	  if (lv_list_check == 0)
	    {
	      for (i = 0; i < student_list.size(); i++)
		{
		  lv_students[(student_list[i]).asString()]["team name"]
		    = team_name; 
		  lv_students[(student_list[i]).asString()]["score"]
		    = 0; 
		}
	      lv_teams[team_name] = student_list;

	      // save to both files
	      rc1 = myJSON2File(buf_fname1, &lv_teams);
	      rc2 = myJSON2File(buf_fname2, &lv_students);
	      if ((rc1 == 0) && (rc2 == 0))
		{
		  error_code = 0;
		  result["status"] = "successful";
		}
	      else
		{
		  error_code = -4;
		  result["status"] = "failed";
		}
	    }
	  else
	    {
	      error_code = -1;
	      result["status"] = "failed";
	    }
	}
      else
	{
	  error_code = -2;
	  result["status"] = "failed";
	}
    }
  else
    {
      error_code = -3;
      result["status"] = "failed";
    }

  result["word"] = "?????";
  result["error code"] = error_code;
  return result;
}

Json::Value
Myhw6Server::submit
(const std::string& action, const std::string& game_id,
 const std::string& team_name)
{
  int i;
  int error_code = 0;
  int rc = 0;
  Json::Value result;
  std::string strJson;

  if (game_id.empty())
    {
      result["status"] = "failed";
      strJson = ( "game_id null ");
      result["reason"] = strJson;
      return result;
    }

  char buf_fname[256];
  char buf_gid[256];

  bzero(buf_fname, 256);
  snprintf(buf_fname, 255, "./games/wordle_%s.json", game_id.c_str());
  rc = myFile2JSON(buf_fname, &result);

  if ((rc == 0) &&
      (result["check_word"].isNull() == false) &&
      (result["check_word"].isString() == true) &&
      (result["check_word"].asString() == "#####") &&
      (result["remaining_score"].isNull() == false) &&
      (result["remaining_score"].isInt() == true) &&
      (result["remaining_score"].asInt() > 0) &&
      (result["score_distributed"].isNull() == false) &&
      (result["score_distributed"].isBool() == true) &&
      (result["score_distributed"].asBool() == false))
    {
      int rc1 = 0;
      int rc2 = 0;

      char buf_fname1[256];
      bzero(buf_fname1, 256);
      snprintf(buf_fname1, 255, "./config/ecs36b_teams.json");

      char buf_fname2[256];
      bzero(buf_fname2, 256);
      snprintf(buf_fname2, 255, "./config/ecs36b_students.json");

      Json::Value lv_teams;
      Json::Value lv_students;
  
      rc1 = myFile2JSON(buf_fname1, &lv_teams);
      rc2 = myFile2JSON(buf_fname2, &lv_students);

      // std::cout << "RC1 = " << rc1 << "RC2 = " << rc2 << "\n";
      if ((rc1 == 0) && (rc2 == 0) && (team_name.empty() == false) &&
	  (lv_teams.isNull() == false) && (lv_teams.isObject() == true) &&
	  (lv_students.isNull() == false) && (lv_students.isObject() == true))
	{
	  Json::Value lv_team_list = lv_teams[team_name];
	  if ((lv_team_list.isNull() == false) &&
	      (lv_team_list.isArray() == true))
	    {
	      for (i = 0; i < lv_team_list.size(); i++)
		{
		  int lv_score = lv_students[(lv_team_list[i]).asString()]["score"].asInt();
		  lv_score += result["remaining_score"].asInt();
		  lv_students[(lv_team_list[i]).asString()]["score"] = lv_score;
		}
	      result["score_distributed"] = true;
	      // save to all three files
	      rc1 = myJSON2File(buf_fname1, &lv_teams);
	      rc2 = myJSON2File(buf_fname2, &lv_students);
	      rc  = myJSON2File(buf_fname, &result);
	      if ((rc1 == 0) && (rc2 == 0) && (rc == 0))
		{
		  error_code = 0;
		  result["status"] = "successful";
		}
	      else
		{
		  error_code = -4;
		  result["status"] = "failed";
		}
	    }
	  else
	    {
	      error_code = -1;
	      result["status"] = "failed";
	    }
	}
      else
	{
	  error_code = -2;
	  result["status"] = "failed";
	}
    }
  else
    {
      error_code = -3;
      result["status"] = "failed";
    }
  
  result["word"] = "?????";
  result["error_code"] = error_code;
  return result;
}

Json::Value
Myhw6Server::obtain
(const std::string& action, const std::string& class_id,
 const std::string& game_id)
{
  int rc = 0;
  Json::Value result;
  std::string strJson;

  if (class_id != "Wordle")
    {
      result["status"] = "failed";
      strJson = ( "class " + class_id + " unknown");
      result["reason"] = strJson;
    }

  if (game_id.empty())
    {
      result["status"] = "failed";
      strJson = ( "game_id null ");
      result["reason"] = strJson;
    }

  char buf_fname[256];
  char buf_gid[256];
  FILE *target;

  if (game_id == "00000000")
    {
      // new game
      unsigned int my_rand = rand();
      my_rand = my_rand % 100000;

      JvTime * my_now_ptr = getNowJvTime();
      std::string *my_now_str = my_now_ptr->getTimeString();

      bzero(buf_gid, 256);
      snprintf(buf_gid, 255, "%d_%s",
	       my_rand, my_now_str->c_str());

      bzero(buf_fname, 256);
      snprintf(buf_fname, 255, "./games/wordle_%s.json",
	       buf_gid);
      target = fopen(buf_fname, "a");
      if (target == NULL)
	{
	  std::cout << "new error\n";
	  exit(-1);
	}
      fclose(target);

      result["game_id"] = buf_gid;
      result["remaining_score"] = 100;
      result["score_distributed"] = false;
      
      // try to find the solution word
      my_rand = rand();
      my_rand = my_rand % 2314;

      int pi = pickWord(word, my_rand);
      if (pi != my_rand)
	std::cout << "very strange\n";

      result["word"] = word;
      result["check_word"] = "_____";
      
      rc = myJSON2File(buf_fname, &result);
      if (rc == 0)
	{
	  result["status"] = "successful";
	}
      else
	{
	  result["status"] = "failed";
	}
    }
  else
    {
      // trying to retrieve an existing game
      bzero(buf_fname, 256);
      snprintf(buf_fname, 255, "./games/wordle_%s.json", game_id.c_str());
      rc = myFile2JSON(buf_fname, &result);
      if (rc == 0)
	{
	  result["status"] = "successful";
	}
      else
	{
	  result["status"] = "failed";
	}
    }

  // remember to mask the real solution before returning
  result["word"] = "?????";
  std::cout << result.toStyledString() << std::endl;
  return result;
}

Json::Value
Myhw6Server::guess
(const std::string& action, const std::string& class_id,
 const std::string& game_id, const std::string& my_guess)
{
  int rc;
  Json::Value result;
  std::string strJson;

  std::cout << "guess is " << my_guess << std::endl;
  
  if (class_id != "Wordle")
    {
      result["status"] = "failed";
      strJson = ( "class " + class_id + " unknown");
      result["reason"] = strJson;
    }

  if (game_id.empty())
    {
      result["status"] = "failed";
      strJson = ( "game_id null ");
      result["reason"] = strJson;
    }

  char buf_fname[256];
  char buf_gid[256];
  FILE *target;

  bzero(buf_fname, 256);
  snprintf(buf_fname, 255, "./games/wordle_%s.json", game_id.c_str());
  rc = myFile2JSON(buf_fname, &result);

  if ((rc == 0) &&
      (result["check_word"].isNull() == false) &&
      (result["check_word"].isString() == true) &&
      (result["check_word"].asString() != "#####") &&
      (result["word"].isNull() == false) &&
      (result["word"].isString() == true) &&
      ((result["word"].asString()).size() == 5))
    {
      bzero(word, WORD_LENGTH + 1);
      snprintf(word, WORD_LENGTH + 1, "%s", (result["word"].asString()).c_str());
      toLower(word);
      
      char lv_guess[WORD_LENGTH + 1];
      bzero(lv_guess, WORD_LENGTH + 1);
      snprintf(lv_guess, WORD_LENGTH + 1, "%s", my_guess.c_str());
      toLower(lv_guess);
      
      if (strcmp(lv_guess, word) == 0)
	{
	  // guessed right
	  result["check_word"] = "#####";
	  int index = (result["guesses"]["data"]).size();
	  Json::Value lv_jv;
	  lv_jv["guess"] = lv_guess;
	  lv_jv["result"] = "#####";
	  JvTime * my_now_ptr = getNowJvTime();
	  std::string *my_now_str = my_now_ptr->getTimeString();
	  lv_jv["time"] = (*my_now_str);
	  result["guesses"]["data"][index] = lv_jv;
	  rc = myJSON2File(buf_fname, &result);
	  result["status"] = "successful";
	}
      else
	{	  
	  if (isWord(lv_guess) && hasWord(lv_guess))
	    {
	      if (checkWord(lv_guess))
		{
		  std::cout << "checkWord done \n";
		  
		  result["check_word"] = check_result;
		  int index = (result["guesses"]["data"]).size();
		  Json::Value lv_jv;
		  lv_jv["guess"] = lv_guess;
		  lv_jv["result"] = check_result;
		  JvTime * my_now_ptr = getNowJvTime();
		  std::string *my_now_str = my_now_ptr->getTimeString();
		  lv_jv["time"] = (*my_now_str);
		  result["guesses"]["data"][index] = lv_jv;
		  if((result["remaining_score"].isNull() == false) &&
		     (result["remaining_score"].isInt() == true))
		    {
		      int lv_remaining = result["remaining_score"].asInt();
		      result["remaining_score"] = lv_remaining -1;
		    }
		  rc = myJSON2File(buf_fname, &result);
		  if (rc == 0)
		    {
		      result["status"] = "successful";
		    }
		  else
		    {
		      result["status"] = "failed";
		    }
		}
	      else
		{
		  result["check_word"] = "_____";
		  result["status"] = "failed";
		}
	    }
	  else
	    {
	      result["check_word"] = "_____";
	      result["status"] = "failed";
	      puts("Word not in list");
	    }
	}
    }
  else
    {
      // failed
      result["check_word"] = "_____";
      result["status"] = "failed";
    }
  
  result["word"] = "?????";
  return result;
}

int
main(void)
{
  JvTime * my_ptr = getNowJvTime();
  fpAll = fopen("./lists/ALL.TXT", "r");
  fpSol = fopen("./lists/SOLUTION.TXT", "r");
  if (fpAll == NULL || fpSol == NULL)
    {
      std::cout << "error opening wordle lists\n";
      exit(-2);
    }

  srand((my_ptr->second) * (my_ptr->minute) * (my_ptr->year));
  HttpServer httpserver(8384);
  Myhw6Server s(httpserver,
		JSONRPC_SERVER_V1V2); // hybrid server (json-rpc 1.0 & 2.0)
  s.StartListening();
  std::cout << "Hit enter to stop the server" << endl;
  getchar();

  s.StopListening();
  return 0;
}
