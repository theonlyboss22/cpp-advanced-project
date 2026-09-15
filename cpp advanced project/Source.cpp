#include <iostream>
#include <string>
#include <fstream>
#include <random>
using namespace std;

class Game
{
	double balance;

public:

	int getBalance()
	{
		return balance;
	}
};

class Blackjack : public Game
{
public:
	int play()
	{
		// Blackjack game logic here
		return 0; //testing 2 3
	}
};

class Slots : public Game
{
public:
	int play()
	{
		// Slots game logic here
		return 0; // testing 3
	}
};

class Roulette : public Game
{
public:
	int play()
	{
		// Roulette game logic here
		return 0;
	}
};

class HorseRace : public Game
{
public:
	int play()
	{
		// Horse Race game logic here

		return 0;
	}

};

int main()
{
	// create game instances
	Blackjack bj;
	Slots slots;
	Roulette roulette;
	HorseRace horseRace;
	int choice = 0;
	cout << "Casino game selection: " << endl;
	cout << "1. Blackjack" << endl;
	cout << "2. Slots" << endl;
	cout << "3. Roulette" << endl;
	cout << "4. Horse Race" << endl;
	cout << "5. Exit" << endl;
	cin >> choice;

	switch (choice)
	{
	case 1:
		bj.play();
		break;
	case 2:
		slots.play();
		break;
	case 3:
		roulette.play();
		break;
	case 4:
		horseRace.play();
		break;
	case 5:
		cout << "Exiting..." << endl;
		break;
	default:
		cout << "Invalid selection." << endl;
		break;
	}

	return 0;
}