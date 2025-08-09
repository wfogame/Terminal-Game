#include <iostream>
#include <string>
#include <fstream>
using namespace std;

int main(){
  
  ofstream file("test.txt");
  std::cout << "File successfuly created" << std::endl;
  file << "Hello from my C++ program!";
  file << "\n This is line two of my C++ program.";
  file.close();
  ifstream Hello("test.txt");
  string testTxt;
  getline(Hello,testTxt);
  std::cout << testTxt

  return 0;
}
