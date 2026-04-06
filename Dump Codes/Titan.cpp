#include <iostream>
using namespace std;

// Struct
struct TitanFall {
    string PilotName;
    string TitanName;
    string Weapon;
    string position;
    int Health;
    int DamageMin;
    int DamageMax;
    double Speed;
};

// Function to display a Titan
void displayTitan(TitanFall t) {
    cout << "==========================" << endl;
    cout << "Pilot Name: " << t.PilotName << endl;
    cout << "Titan Name: " << t.TitanName << endl;
    cout << "Weapon: " << t.Weapon << endl;
    cout << "Faction: " << t.position << endl;
    cout << "Health: " << t.Health << endl;
    cout << "Damage: " << t.DamageMin << " - " << t.DamageMax << endl;
    cout << "Speed: " << t.Speed << endl;
}

int main() {
    // Create 3 Titans
    TitanFall t1, t2, t3;

    // Titan 1
    t1.PilotName = "Viper";
    t1.TitanName = "NorthStar";
    t1.Weapon = "Sniper";
    t1.position = "Apex Predators";
    t1.Health = 7500;
    t1.DamageMin = 3000;
    t1.DamageMax = 15000;
    t1.Speed = 10.16;

    // Titan 2
    t2.PilotName = "Blisk";
    t2.TitanName = "Legion";
    t2.Weapon = "Minigun";
    t2.position = "Apex Predators";
    t2.Health = 12000;
    t2.DamageMin = 2000;
    t2.DamageMax = 8000;
    t2.Speed = 6.5;

    // Titan 3
    t3.PilotName = "Kane";
    t3.TitanName = "Scorch";
    t3.Weapon = "Flame Core";
    t3.position = "IMC";
    t3.Health = 10000;
    t3.DamageMin = 2500;
    t3.DamageMax = 9000;
    t3.Speed = 5.8;

    // Display all Titans
    displayTitan(t1);
    displayTitan(t2);
    displayTitan(t3);

    return 0;
}