#include <iostream>
#include <string>
using namespace std;
enum GearLevel { DEMON, GOD };
  class Gear {
    string gName;
    string gType;
    GearLevel gLevel;

    public:
      Gear(string name, string type,GearLevel level){
        gName = name;
        gType = type;
        gLevel = level;
        }
  };
  class player {
    private:
      int lvl;
      Gear equipment;
      int health;
      int damage;
      int armor;
      int inventory;
    public:
      player(int level = 1,string name = "wfogame", Gear gear){
        lvl = level;
        equipment = gear;
        health = lvl;
        damage = lvl;
        armor = lvl;
        inventory= lvl;

      }
  };

