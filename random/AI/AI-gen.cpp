#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <map>
#include <iomanip>

using namespace std;

// Forward declarations
class Character;
class Gear;

// Enums for gear types and levels
enum class GearType {
    SWORD,
    SPEAR,
    ARROW
};

enum class GearLevel {
    NORMAL,
    DEMON,
    GOD
};

// Base Gear class
class Gear {
public:
    string name;
    GearType type;
    GearLevel level;
    vector<string> abilities;
    
    // Stats modifiers
    int healthBonus = 0;
    int armorBonus = 0;
    int damageBonus = 0;
    
    Gear(string n, GearType t, GearLevel l) : name(n), type(t), level(l) {
        initializeGear();
    }
    
    void initializeGear() {
        // Base stats based on type
        switch(type) {
            case GearType::SWORD:
                damageBonus = 15;
                armorBonus = 5;
                healthBonus = 10;
                break;
            case GearType::SPEAR:
                damageBonus = 20;
                armorBonus = 2;
                healthBonus = 5;
                break;
            case GearType::ARROW:
                damageBonus = 25;
                armorBonus = 0;
                healthBonus = 0;
                break;
        }
        
        // Level-specific bonuses and abilities
        switch(level) {
            case GearLevel::DEMON:
                abilities.push_back("Soul Steal");
                abilities.push_back("Poison");
                abilities.push_back("Multi-Attack");
                abilities.push_back("Death Blow");
                damageBonus += 10;
                break;
            case GearLevel::GOD:
                abilities.push_back("Restrain");
                abilities.push_back("Holy Armor");
                abilities.push_back("Holy Takedown");
                abilities.push_back("Divine Protection");
                armorBonus += 20;
                healthBonus += 30;
                break;
            case GearLevel::NORMAL:
                // No special abilities for normal gear
                break;
        }
    }
    
    string getTypeString() const {
        switch(type) {
            case GearType::SWORD: return "Sword";
            case GearType::SPEAR: return "Spear";
            case GearType::ARROW: return "Arrow";
            default: return "Unknown";
        }
    }
    
    string getLevelString() const {
        switch(level) {
            case GearLevel::DEMON: return "\033[31mDEMON\033[0m";  // Red
            case GearLevel::GOD: return "\033[33mGOD\033[0m";      // Yellow
            case GearLevel::NORMAL: return "Normal";
            default: return "Unknown";
        }
    }
    
    void displayInfo() const {
        cout << "\n=== " << name << " ===" << endl;
        cout << "Type: " << getTypeString() << endl;
        cout << "Level: " << getLevelString() << endl;
        cout << "Health Bonus: +" << healthBonus << endl;
        cout << "Armor Bonus: +" << armorBonus << endl;
        cout << "Damage Bonus: +" << damageBonus << endl;
        if (!abilities.empty()) {
            cout << "Abilities: ";
            for (size_t i = 0; i < abilities.size(); i++) {
                cout << abilities[i];
                if (i < abilities.size() - 1) cout << ", ";
            }
            cout << endl;
        }
    }
};

// Character class representing players and enemies
class Character {
public:
    string name;
    int maxHealth;
    int currentHealth;
    int baseDamage;
    int armor;
    unique_ptr<Gear> equippedGear;
    int souls = 0;  // For DEMON gear
    int worshippers = 0;  // For GOD gear
    bool isPoisoned = false;
    int poisonTurns = 0;
    bool isRestrained = false;
    
    Character(string n, int health, int damage, int arm) 
        : name(n), maxHealth(health), currentHealth(health), baseDamage(damage), armor(arm) {}
    
    int getTotalDamage() const {
        int totalDamage = baseDamage;
        if (equippedGear) {
            totalDamage += equippedGear->damageBonus;
            
            // DEMON level bonus: less health = more damage
            if (equippedGear->level == GearLevel::DEMON) {
                float healthPercentage = (float)currentHealth / maxHealth;
                if (healthPercentage < 0.5) {
                    totalDamage *= 1.5;  // 50% more damage when below half health
                }
                if (healthPercentage < 0.25) {
                    totalDamage *= 2;    // Double damage when below 25% health
                }
                // Soul bonus
                totalDamage += souls * 2;
            }
            
            // GOD level bonus: based on worshippers
            if (equippedGear->level == GearLevel::GOD) {
                totalDamage += worshippers * 5;
            }
        }
        return totalDamage;
    }
    
    int getTotalArmor() const {
        int totalArmor = armor;
        if (equippedGear) {
            totalArmor += equippedGear->armorBonus;
            
            // GOD level bonus: higher health = higher armor
            if (equippedGear->level == GearLevel::GOD) {
                float healthPercentage = (float)currentHealth / maxHealth;
                if (healthPercentage > 0.75) {
                    totalArmor *= 1.5;
                }
            }
        }
        return totalArmor;
    }
    
    void equipGear(unique_ptr<Gear> gear) {
        equippedGear = move(gear);
        // Apply health bonus
        if (equippedGear) {
            maxHealth += equippedGear->healthBonus;
            currentHealth += equippedGear->healthBonus;
        }
    }
    
    void takeDamage(int damage, Character* attacker = nullptr) {
        int totalArmor = getTotalArmor();
        
        // GOD level special: sometimes take 0 damage
        if (equippedGear && equippedGear->level == GearLevel::GOD) {
            if (rand() % 100 < 20) {  // 20% chance
                cout << name << "'s Divine Protection activated! No damage taken!" << endl;
                return;
            }
        }
        
        int actualDamage = max(1, damage - totalArmor);
        
        // DEMON vs GOD: 1.5x damage
        if (attacker && attacker->equippedGear && 
            attacker->equippedGear->level == GearLevel::GOD &&
            equippedGear && equippedGear->level == GearLevel::DEMON) {
            actualDamage = actualDamage * 1.5;
            cout << "Holy damage! Extra effective against demons!" << endl;
        }
        
        currentHealth -= actualDamage;
        cout << name << " takes " << actualDamage << " damage! (Health: " << max(0, currentHealth) << "/" << maxHealth << ")" << endl;
        
        if (currentHealth <= 0 && equippedGear && equippedGear->level == GearLevel::DEMON && attacker) {
            // Death Blow ability
            cout << name << " triggers Death Blow!" << endl;
            attacker->takeDamage(baseDamage);
        }
    }
    
    void heal(int amount) {
        currentHealth = min(currentHealth + amount, maxHealth);
        cout << name << " heals for " << amount << " HP! (Health: " << currentHealth << "/" << maxHealth << ")" << endl;
    }
    
    bool isAlive() const {
        return currentHealth > 0;
    }
    
    void displayStatus() const {
        cout << "\n=== " << name << " ===" << endl;
        cout << "Health: " << currentHealth << "/" << maxHealth << endl;
        cout << "Damage: " << getTotalDamage() << endl;
        cout << "Armor: " << getTotalArmor() << endl;
        if (equippedGear) {
            cout << "Equipped: " << equippedGear->name << " (" << equippedGear->getLevelString() << ")" << endl;
        }
        if (souls > 0) {
            cout << "Souls collected: " << souls << endl;
        }
        if (worshippers > 0) {
            cout << "Worshippers: " << worshippers << endl;
        }
        if (isPoisoned) {
            cout << "\033[32mPoisoned (" << poisonTurns << " turns)\033[0m" << endl;
        }
        if (isRestrained) {
            cout << "\033[35mRestrained\033[0m" << endl;
        }
    }
};

// Game class to manage the gameplay
class Game {
private:
    unique_ptr<Character> player;
    vector<unique_ptr<Character>> enemies;
    mt19937 rng;
    int turn = 1;
    
public:
    Game() : rng(random_device{}()) {
        cout << "\033[2J\033[1;1H";  // Clear screen
        cout << "========================================" << endl;
        cout << "     TERMINAL COMBAT: GODS VS DEMONS    " << endl;
        cout << "========================================" << endl;
        initializeGame();
    }
    
    void initializeGame() {
        // Create player
        string playerName;
        cout << "\nEnter your character's name: ";
        getline(cin, playerName);
        player = make_unique<Character>(playerName, 100, 20, 5);
        
        // Choose starting gear
        chooseStartingGear();
        
        // Create enemies
        createEnemies();
        
        // Start game loop
        gameLoop();
    }
    
    void chooseStartingGear() {
        cout << "\nChoose your starting gear:" << endl;
        cout << "1. Demon Sword - Power through destruction" << endl;
        cout << "2. God Spear - Divine might and protection" << endl;
        cout << "3. Normal Arrow - Balanced approach" << endl;
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        unique_ptr<Gear> gear;
        switch(choice) {
            case 1:
                gear = make_unique<Gear>("Bloodthirsty Blade", GearType::SWORD, GearLevel::DEMON);
                break;
            case 2:
                gear = make_unique<Gear>("Divine Lance", GearType::SPEAR, GearLevel::GOD);
                break;
            case 3:
            default:
                gear = make_unique<Gear>("Hunter's Bow", GearType::ARROW, GearLevel::NORMAL);
                break;
        }
        
        gear->displayInfo();
        player->equipGear(move(gear));
    }
    
    void createEnemies() {
        // Create a mix of enemies
        enemies.push_back(make_unique<Character>("Goblin", 50, 10, 2));
        
        auto goblinGear = make_unique<Gear>("Rusty Dagger", GearType::SWORD, GearLevel::NORMAL);
        enemies.back()->equipGear(move(goblinGear));
        
        enemies.push_back(make_unique<Character>("Demon Knight", 80, 15, 8));
        auto demonGear = make_unique<Gear>("Hell Sword", GearType::SWORD, GearLevel::DEMON);
        enemies.back()->equipGear(move(demonGear));
        
        enemies.push_back(make_unique<Character>("Angel Guardian", 120, 12, 15));
        auto angelGear = make_unique<Gear>("Celestial Spear", GearType::SPEAR, GearLevel::GOD);
        enemies.back()->equipGear(move(angelGear));
    }
    
    void gameLoop() {
        while (player->isAlive() && !enemies.empty()) {
            cout << "\n========== TURN " << turn << " ==========" << endl;
            
            // Process poison
            processPoison();
            
            // Player turn
            playerTurn();
            
            // Remove dead enemies
            enemies.erase(
                remove_if(enemies.begin(), enemies.end(),
                    [this](const unique_ptr<Character>& enemy) {
                        if (!enemy->isAlive()) {
                            cout << enemy->name << " has been defeated!" << endl;
                            // DEMON gear soul steal
                            if (player->equippedGear && player->equippedGear->level == GearLevel::DEMON) {
                                player->souls++;
                                cout << "Soul stolen! Total souls: " << player->souls << endl;
                            }
                            return true;
                        }
                        return false;
                    }),
                enemies.end()
            );
            
            if (enemies.empty()) {
                break;
            }
            
            // Enemy turns
            enemyTurns();
            
            turn++;
        }
        
        // Game over
        cout << "\n========================================" << endl;
        if (player->isAlive()) {
            cout << "     VICTORY! YOU ARE THE CHAMPION!     " << endl;
        } else {
            cout << "       DEFEAT! BETTER LUCK NEXT TIME    " << endl;
        }
        cout << "========================================" << endl;
    }
    
    void processPoison() {
        if (player->isPoisoned) {
            player->takeDamage(5);
            player->poisonTurns--;
            if (player->poisonTurns <= 0) {
                player->isPoisoned = false;
                cout << player->name << " is no longer poisoned!" << endl;
            }
        }
        
        for (auto& enemy : enemies) {
            if (enemy->isPoisoned) {
                enemy->takeDamage(5);
                enemy->poisonTurns--;
                if (enemy->poisonTurns <= 0) {
                    enemy->isPoisoned = false;
                    cout << enemy->name << " is no longer poisoned!" << endl;
                }
            }
        }
    }
    
    void playerTurn() {
        player->displayStatus();
        
        if (player->isRestrained) {
            cout << "You are restrained and cannot act this turn!" << endl;
            player->isRestrained = false;
            return;
        }
        
        cout << "\nChoose your action:" << endl;
        cout << "1. Attack" << endl;
        cout << "2. Use Special Ability" << endl;
        cout << "3. Heal (20 HP)" << endl;
        cout << "4. View Enemy Status" << endl;
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        switch(choice) {
            case 1:
                performAttack();
                break;
            case 2:
                useSpecialAbility();
                break;
            case 3:
                player->heal(20);
                break;
            case 4:
                viewEnemyStatus();
                playerTurn();  // Let player choose again
                break;
            default:
                cout << "Invalid choice! Skipping turn..." << endl;
        }
    }
    
    void performAttack() {
        if (enemies.empty()) return;
        
        cout << "Choose target:" << endl;
        for (size_t i = 0; i < enemies.size(); i++) {
            cout << i + 1 << ". " << enemies[i]->name 
                 << " (HP: " << enemies[i]->currentHealth << "/" << enemies[i]->maxHealth << ")" << endl;
        }
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        if (choice > 0 && choice <= enemies.size()) {
            auto& target = enemies[choice - 1];
            int damage = player->getTotalDamage();
            
            // DEMON ability: more damage to low health enemies
            if (player->equippedGear && player->equippedGear->level == GearLevel::DEMON) {
                float targetHealthPercent = (float)target->currentHealth / target->maxHealth;
                if (targetHealthPercent < 0.3) {
                    damage *= 1.5;
                    cout << "Execution bonus! Attacking weakened enemy!" << endl;
                }
            }
            
            cout << player->name << " attacks " << target->name << "!" << endl;
            target->takeDamage(damage, player.get());
        }
    }
    
    void useSpecialAbility() {
        if (!player->equippedGear || player->equippedGear->abilities.empty()) {
            cout << "No special abilities available!" << endl;
            return;
        }
        
        cout << "Choose ability:" << endl;
        for (size_t i = 0; i < player->equippedGear->abilities.size(); i++) {
            cout << i + 1 << ". " << player->equippedGear->abilities[i] << endl;
        }
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        if (choice < 1 || choice > player->equippedGear->abilities.size()) {
            cout << "Invalid choice!" << endl;
            return;
        }
        
        string ability = player->equippedGear->abilities[choice - 1];
        
        if (ability == "Poison") {
            usePoisonAbility();
        } else if (ability == "Multi-Attack") {
            useMultiAttack();
        } else if (ability == "Restrain") {
            useRestrain();
        } else if (ability == "Holy Takedown") {
            useHolyTakedown();
        } else if (ability == "Soul Steal") {
            useSoulSteal();
        } else if (ability == "Divine Protection") {
            useDivineProtection();
        } else {
            cout << "Ability not implemented yet!" << endl;
        }
    }
    
    void usePoisonAbility() {
        if (enemies.empty()) return;
        
        cout << "Choose target to poison:" << endl;
        for (size_t i = 0; i < enemies.size(); i++) {
            cout << i + 1 << ". " << enemies[i]->name << endl;
        }
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        if (choice > 0 && choice <= enemies.size()) {
            enemies[choice - 1]->isPoisoned = true;
            enemies[choice - 1]->poisonTurns = 3;
            cout << enemies[choice - 1]->name << " has been poisoned for 3 turns!" << endl;
        }
    }
    
    void useMultiAttack() {
        cout << player->name << " attacks all enemies!" << endl;
        int damage = player->getTotalDamage() * 0.7;  // Reduced damage for multi-attack
        
        for (auto& enemy : enemies) {
            enemy->takeDamage(damage, player.get());
        }
    }
    
    void useRestrain() {
        if (enemies.empty()) return;
        
        cout << "Choose target to restrain:" << endl;
        for (size_t i = 0; i < enemies.size(); i++) {
            cout << i + 1 << ". " << enemies[i]->name << endl;
        }
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        if (choice > 0 && choice <= enemies.size()) {
            enemies[choice - 1]->isRestrained = true;
            cout << enemies[choice - 1]->name << " has been restrained for 1 turn!" << endl;
        }
    }
    
    void useHolyTakedown() {
        if (enemies.empty()) return;
        
        cout << "Choose target for Holy Takedown:" << endl;
        for (size_t i = 0; i < enemies.size(); i++) {
            cout << i + 1 << ". " << enemies[i]->name << endl;
        }
        cout << "Choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        if (choice > 0 && choice <= enemies.size()) {
            int damage = player->getTotalDamage() + player->getTotalArmor();
            cout << player->name << " performs Holy Takedown!" << endl;
            enemies[choice - 1]->takeDamage(damage, player.get());
        }
    }
    
    void useSoulSteal() {
        cout << player->name << " attempts to steal souls!" << endl;
        for (auto& enemy : enemies) {
            if (enemy->currentHealth < enemy->maxHealth * 0.3) {
                player->souls++;
                enemy->takeDamage(10);
                cout << "Soul partially stolen from " << enemy->name << "!" << endl;
            }
        }
        cout << "Total souls: " << player->souls << endl;
    }
    
    void useDivineProtection() {
        player->worshippers++;
        player->heal(15);
        cout << player->name << " gains a worshipper and divine healing!" << endl;
        cout << "Total worshippers: " << player->worshippers << endl;
    }
    
    void viewEnemyStatus() {
        for (const auto& enemy : enemies) {
            enemy->displayStatus();
        }
    }
    
    void enemyTurns() {
        for (auto& enemy : enemies) {
            if (!enemy->isAlive()) continue;
            
            if (enemy->isRestrained) {
                cout << enemy->name << " is restrained and cannot act!" << endl;
                enemy->isRestrained = false;
                continue;
            }
            
            // Simple AI
            int action = uniform_int_distribution<>(1, 10)(rng);
            
            if (action <= 6) {
                // Regular attack
                cout << enemy->name << " attacks " << player->name << "!" << endl;
                player->takeDamage(enemy->getTotalDamage(), enemy.get());
            } else if (action <= 8 && enemy->equippedGear) {
                // Use special ability
                if (enemy->equippedGear->level == GearLevel::DEMON) {
                    // Poison player
                    if (!player->isPoisoned && uniform_int_distribution<>(1, 2)(rng) == 1) {
                        player->isPoisoned = true;
                        player->poisonTurns = 3;
                        cout << enemy->name << " poisons " << player->name << "!" << endl;
                    } else {
                        cout << enemy->name << " attacks with dark energy!" << endl;
                        player->takeDamage(enemy->getTotalDamage() * 1.2, enemy.get());
                    }
                } else if (enemy->equippedGear->level == GearLevel::GOD) {
                    // Restrain player
                    if (!player->isRestrained && uniform_int_distribution<>(1, 3)(rng) == 1) {
                        player->isRestrained = true;
                        cout << enemy->name << " restrains " << player->name << "!" << endl;
                    } else {
                        cout << enemy->name << " performs a holy strike!" << endl;
                        player->takeDamage(enemy->getTotalDamage() + enemy->getTotalArmor() * 0.5, enemy.get());
                    }
                } else {
                    // Normal attack for normal gear
                    cout << enemy->name << " attacks " << player->name << "!" << endl;
                    player->takeDamage(enemy->getTotalDamage(), enemy.get());
                }
            } else {
                // Heal
                enemy->heal(10);
            }
        }
    }
};

// Main function to start the game
int main() {
    srand(time(nullptr));
    
    cout << "Welcome to the Terminal Combat Game!" << endl;
    cout << "Press Enter to start...";
    cin.get();
    
    Game game;
    
    cout << "\nThanks for playing!" << endl;
    return 0;
}
