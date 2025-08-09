#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

class self{
public:
    self(int howmany){
        string basic = "";
        
        while(basic.length() < howmany){
            ifstream sel(__FILE__);
            stringstream buffer;
            buffer << sel.rdbuf();
            string fileContent = buffer.str();
            ofstream file(basic + __FILE__);
            file << fileContent;
            basic = basic + "1";
        }
    }
};

int main(){
    int many = 2;
    cout << "How Much times to replicate.";
    cin >> many;
    self s(many);
    return 0;
}
