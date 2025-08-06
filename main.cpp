#include <iostream>
#include <string>
using namespace std;
  class encode{
    string input;

    public:
      encode(string sinput){
        input = sinput;


      }
      string compress(){
        string output = "";
        char cchar = input[0];
        int cnum = 0;
        for (int i = 0; i < input.length(); i++){
          if(cchar == input[i]){
            cnum = cnum + 1;
          }

          
          if(cnum > 1 && cchar != input[i+1]){
            output = output + cnum + cchar;
          }
          else{
            output = output + cchar;
          }

          cchar = input[i];

        }
        return output;
      }
    private:

      

  };

  int main(void){
    string teststring;
    std::cin >> teststring;
    std::cout << encode(teststring);
    
  }
