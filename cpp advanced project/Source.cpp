#include <iostream>
#include <string>
#include <fstream>
#include <random>
using namespace std;

int balance = 0;

void addBalance(int newBalance)
{
	balance += newBalance;
}

class Game
{
public:
	virtual int play() = 0;
	int Exit()
	{
		cout << "Exiting..." << endl;
		return 0;
	}
};

class Blackjack : public Game
{
public:
	int play() override
	{
		default_random_engine generator;
		struct card
		{
			string name;
			int value;
			int frequency;
		};
		int betAmount;
		card cards[] = { {"2", 2, 4}, {"3", 3, 4}, {"4", 4, 4}, {"5", 5, 4}, {"6", 6, 4}, {"7", 7, 4}, {"8", 8, 4}, {"9", 9, 4}, {"10", 10, 4}, {"Jack", 10, 4}, {"Queen", 10, 4}, {"King", 10, 4}, {"Ace", 11, 4} };
		uniform_int_distribution<int> distribution(1, sizeof(cards) / sizeof(cards[0]));
		int randomNumber = distribution(generator);
		cout << "Blackjack selected. \nEnter your bet amount: \n";
		while ((cin >> betAmount) && betAmount > balance)
		{
			cout << "Insufficient balance. \nTry again: \n";
		}

		if (betAmount <= balance)
		{
			cout << "Balance Remaining: " << balance - betAmount << endl;

			balance = balance - betAmount;
		}

		return 0;
	}
};

class Slots : public Game
{
public:
	int play() override
	{
		// Slots game logic here
		return 0;
	}
};

class Roulette : public Game
{
public:
	int play() override
	{
		// Roulette game logic here
		return 0;
	}
};

class HorseRace : public Game
{
public:
	int play() override
	{
		// Horse Race game logic here

		return 0;
	}

};

int main()
{
	// create game instances
	Game* game = nullptr;
	int choice = 0;
	cout << "Free balance given: 1000" << endl;
	addBalance(1000);
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
		game = new Blackjack();
		(*game).play();
		break;
	case 2:
		game = new Slots();
		(*game).play();
		break;
	case 3:
		game = new Roulette();
		(*game).play();
		break;
	case 4:
		game = new HorseRace();
		(*game).play();
		break;
	case 5:
		cout << "Exiting..." << endl;
		break;
	default:
		cout << "Invalid selection." << endl;
		break;
	}
	delete game;
	return 0;
}
