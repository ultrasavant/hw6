
#include <iostream>
#include "ecs36b_Exception.h"

// JSON RPC part
#include "hw6client.h"
#include <jsonrpccpp/client/connectors/httpclient.h>

using namespace jsonrpc;
using namespace std;

void narrowChoicesFromFile(std::string solutionFormat, std::string containsLetters);
// Files for lists that contain all words and solutions
FILE * fpSol;
std::string possibleWords[10] = {};
int possibleIndex = 0;


int
main()
{
  // distributed wordle demo for 11/23/2022
  // HttpClient httpclient("http://127.0.0.1:8384");
  HttpClient httpclient("https://fa470c1cc98f.ngrok.io");
  hw6Client myClient(httpclient, JSONRPC_CLIENT_V2);
  Json::Value myv;

  const char* wordsForFilter[11] = {"earth", "mound", "sicky", "mafia", "agape", "djins", "above", "ablow", "abuzz", "abbey", "aquas"};
  int wordsForFilterSize = 11;

  int containCount = 0;
  bool alreadyContained = false;
  std::string emptyFormat = "_____";
  std::string containsLetters;
  std::string solutionFormat;

  fpSol = fopen("lists/SOLUTION.TXT", "r");
  if (fpSol == NULL) {
    std::cout << "error opening wordle list" << std::endl;
		exit(-1);
	}

  // Loop through games
  int gamesToLoopThrough = 10;
  for (int i = 0; i < gamesToLoopThrough; i += 1) {

    // Resets for each game
    possibleIndex = 0;
    // FIXME
    for (int i = 0; i < 10; i += 1) {
      possibleWords[i].clear();
    }
    containCount = 0;
    // For containsLetters
    alreadyContained = false;

    containsLetters.assign(emptyFormat);
    solutionFormat.assign(emptyFormat);
    // containsLetters[6] = "_____";
    // solutionFormat[6] = "_____";


    // obtain and new
    try {
      myv = myClient.obtain("obtain", "Wordle", "00000000");
    } catch (JsonRpcException &e) {
      cerr << e.what() << endl;
    }
    // prints out full game content
    // std::cout << myv.toStyledString() << std::endl;

    // Loop through all key words before scanning through SOLUTION.txt
    for (int j = 0; j < wordsForFilterSize; j += 1) {
      try {
        // Guess word
        myv = myClient.guess("guess", "Wordle", myv["game_id"].asString(),
        wordsForFilter[j]);
      } catch (JsonRpcException &e) {
        cerr << e.what() << endl;
      }

      // Prints result after guess
      std::string guess = wordsForFilter[j];
      std::string guess_hint = myv["check_word"].toStyledString();
      // Testing
      // std::cout << "\nguess is " << guess << std::endl;
      // std::cout << "check_word is\n" << guess_hint << "\n" << std::endl;


      // Loop through check_word string index k
      // Index at 1  for guess_hint to not check quotations
      for (int k = 0; k < 5; k += 1) {
          // For precise, record letter into solutionFormat
          // Placement matters
          if (guess_hint[k + 1] == '#' && solutionFormat[k] != guess[k]) {
              solutionFormat[k] = guess[k];
          }
          
          // For accurate, record letter into containsLetters
          // Correct placement letter included too
          if (guess_hint[k + 1] == 'o' || guess_hint[k + 1] == '#') {

              // Check if letter is already in containsLetters
              for (int l = 0; l < 5; l += 1) {
                  if (guess[k] == containsLetters[l]) {
                      // letter is a repeat, do nothing
                      alreadyContained = true;
                      break;
                  }
              }

              // Done checking for repeat in containsLetter, done
              if (alreadyContained) {
                  // Reset for next letter
                  alreadyContained = false;
              }
              // Assign non repeat to containsLetters //BUT check for available space
              else {
                  containsLetters[containCount] = guess[k];
                  containCount += 1;
              }

          }

      }

    } // End of wordForFilter testing


    // FIXME: Scan SOLUTION.txt using solutionFormat and containsLetters
    // Stores narrowed choices into possibleWords after scanning SOLUTION.TXT
    narrowChoicesFromFile(solutionFormat, containsLetters);

    // Guess each of the narrowed words stored in possibleWords
    std::cout << "possible index before guess narrowed words: " << possibleIndex << std::endl;
    for (int x = 0; x < possibleIndex; x += 1) {

      try {
        // Guess word
        myv = myClient.guess("guess", "Wordle", myv["game_id"].asString(),
        possibleWords[x]);
      } catch (JsonRpcException &e) {
        cerr << e.what() << endl;
      }
   
      // Submit criteria met
      if (myv["check_word"] == "#####") {
          std::cout << "the solution is: " << possibleWords[x] << std::endl;
          try {
            myv = myClient.submit("submit", myv["game_id"].asString(), "Alan and Terrance");
          } catch (JsonRpcException &e) {
            cerr << e.what() << endl;
          }
          std::cout << myv.toStyledString() << std::endl;
          break;
      }
      
    }


  } // End of games looping
 
  fclose(fpSol);
 
  // Set Teamname (one time use)
  // Json::Value jv_list;
  // jv_list[0] = "918129138";
  // jv_list[1] = "918886002";

  // try {
  //   myv = myClient.set_name("set_name", jv_list, "Alan and Terrance");
  // } catch (JsonRpcException &e) {
  //   cerr << e.what() << endl;
  // }
  // std::cout << myv.toStyledString() << std::endl;

  
  return 0;

}

void narrowChoicesFromFile(std::string solutionFormat, std::string containsLetters) {

    // printf("Narrowed choices:\n");

    // Scan through words one by one in ALL.TXT
    bool dismissWord = false;
    int matchedLetters = 0;
    char buffer[6];

    while ((fscanf(fpSol, "%[^\n]", buffer)) != EOF) {
        // Dismiss word if not in containsLetters
        matchedLetters = 0;
        for (int i = 0; i < 5; i += 1) {
            for (int j = 0; j < 5; j += 1) {
                // Scan next char of buffer if previous char contains containsLetters element
                if (buffer[i] == containsLetters[j]) {
                  matchedLetters += 1;
                  break;
                }
              
   
            }
            // Check if each character matches a containsLetter element
            if (matchedLetters != (i + 1)) {
                dismissWord = true;
                break;
            }
 
        }

        if (dismissWord) {
            // Reset for next word, then continue in while loop
            fgetc(fpSol);
            dismissWord = false;
            continue;
        }

        // Finishing touch (dismiss word if not following solutionFormat)

        for (int i = 0; i < 5; i += 1) {

            if (solutionFormat[i] != '_') {
                if (buffer[i] != solutionFormat[i]) {
                    dismissWord = true;
                    break;
                }
            }

        }

        if (dismissWord) {
            // Reset for next word, then continue in while loop
            fgetc(fpSol);
            dismissWord = false;
            continue;
        }
    
        // Narrowed choice!
        possibleWords[possibleIndex] = buffer;
        possibleIndex += 1;
        fgetc(fpSol);

    } // end while loop

    // Scan SOLUTION.txt from beginning for next game
    fseek(fpSol, 0, SEEK_SET);


}


