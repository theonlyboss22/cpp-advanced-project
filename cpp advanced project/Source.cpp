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

struct card
{
	string name;
	int value;
	int frequency;
};

// Passes gen by ref using 'auto&' or 'default_random_engine&'
void drawHand(int hand[], int& count, card cards[], default_random_engine& generator, uniform_int_distribution<int>& distribution)
{
	cout << "Your Hand:";
	for (int i = 0; i < 2; i++)
	{
		int drawnIndex = distribution(generator);
		hand[count] = drawnIndex;
		count++;

		cout << " |" << cards[drawnIndex].name << "| ";
	}
	cout << endl;
}

int calculateHandValue(const int hand[], int count, const card cards[])
{
	int total = 0;
	int aceCount = 0;

	// Sums base values and tracks Aces
	for (int i = 0; i < count; i++)
	{
		int cardIndex = hand[i];
		total += cards[cardIndex].value;
		if (cards[cardIndex].name == "Ace")
		{
			aceCount++;
		}
	}

	// Adjusts Ace values from 11 to 1 if total exceeds 21
	while (total > 21 && aceCount > 0)
	{
		total -= 10;
		aceCount--;
	}

	return total;
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
		default_random_engine generator(random_device{}());

		int hand[11];
		int count = 0;
		int betAmount;
		int currentHandValue;

		card cards[] = { {"2", 2, 4}, {"3", 3, 4}, {"4", 4, 4}, {"5", 5, 4}, {"6", 6, 4}, {"7", 7, 4}, {"8", 8, 4}, {"9", 9, 4}, {"10", 10, 4}, {"Jack", 10, 4}, {"Queen", 10, 4}, {"King", 10, 4}, {"Ace", 11, 4} };
		uniform_int_distribution<int> distribution(0, (sizeof(cards) / sizeof(cards[0])) - 1); // changed index so it works
		int randomNumber = distribution(generator);
		cout << "Blackjack selected. \nEnter your bet amount: \n";

		while ((cin >> betAmount) && betAmount > balance)
		{
			cout << "Insufficient balance. \nTry again: \n";
		}

		if (betAmount <= balance)
		{
			cout << "\nBalance Remaining: " << balance - betAmount << "\n" << endl;

			balance = balance - betAmount;
		}

		drawHand(hand, count, cards, generator, distribution);

		currentHandValue = calculateHandValue(hand, count, cards);
		cout << "Current Hand Value: " << currentHandValue << endl;

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


// Horse Class
class Horse {
private:
	string name;
	double distance;
	bool finished;
	int place;

public:
	Horse(string horseName) {
		name = horseName;
		distance = 1.0;
		finished = false;
		place = 0;
	}

	void run() {
		int randomNumber = rand() % 100;

		if (randomNumber < 50) {
			distance += 0;
		}
		else if (randomNumber < 80) {
			distance += 1.09;
		}
		else if (randomNumber < 95) {
			distance += 2.29;
		}
		else {
			distance += 4.59;
		}
	}

	string getName() {
		return name;
	}

	double getDistance() {
		return distance;
	}

	bool isFinished() {
		return finished;
	}

	int getPlace() {
		return place;
	}

	void setFinished(bool value) {
		finished = value;
	}

	void setPlace(int value) {
		place = value;
	}
};

class HorseRace : public Game
{
private:
	void playRace(balance, int horseCount, double finishValue) {
		while (true) {

			// Horse Number
			Horse horse[10] = {
				Horse("Horse 1"),
				Horse("Horse 2"),
				Horse("Horse 3"),
				Horse("Horse 4"),
				Horse("Horse 5"),
				Horse("Horse 6"),
				Horse("Horse 7"),
				Horse("Horse 8"),
				Horse("Horse 9"),
				Horse("Horse 10")
			};

			// Select Horse and Bet
			int selectHorse;
			double bet;
			string startChoice;

			while (true) {

			}

		}
	}
public:
	int play() override
	{
		// Horse Race game logic here
		while (true) {
			if (balance <= 0) {
				cout << "You do not have enough balance to play." << endl;
				return 0;
			}

			cout << "Horse Race" << endl;
			cout << "\nBalance: " << balance << endl;

			cout << "\n Choose an area: " << endl;

			cout << "1.Small Area			(3  horses)" << endl;
			cout << "2.Medium Area			(5  horses)" << endl;
			cout << "3.Large Area			(7  horses)" << endl;
			cout << "4.Extra Large Area		(10 horses)" << endl;
			cout << "5.Back" << endl;

			int choice;
			cout << "\nChoice: " << endl;
			cin >> choice;

			switch (choice) {
			case 1:
				playRace(balance, 3, 100);
				break;

			case 2:
				playRace(balance, 5, 200);
				break;

			case 3:
				playRace(balance, 7, 350);
				break;

			case 4:
				playRace(balance, 10, 500);
				break;

			case 5:
				return 0;

			default:
				cout << "Invalid selection." << endl;
				break;
			}

		}
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
