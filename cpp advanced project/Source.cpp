using namespace std;
#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <chrono>

int balance = 0;
bool playing = true;

void addBalance(int newBalance)
{
	balance += newBalance;
}

class Game
{
private:
	int betAmount = 0;
	bool inputValid = false;
public:
	int bet = 0;
	virtual void play() = 0;
	int getBet()
	{
		do
		{
			cout << "Enter your bet amount: \n";
			cin.ignore(INT_MAX, '\n');
			cin >> betAmount;
			if ((betAmount > balance || betAmount <= 0 || cin.fail()))
			{
				cout << "Insufficient balance / invalid input. \nTry again: \n";
				inputValid = false;
			}
			else if (betAmount <= balance)
			{
				cout << "\nBalance Remaining: " << balance - betAmount << "\n" << endl;

				inputValid = true;
			}
			else
				cout << "Unexpected error. \nTry again: \n";
			cin.clear();
		} while (inputValid == false);
		return betAmount;
	}
};

class Blackjack : public Game
{
private:
	struct card
	{
		string name;
		int value;
		int frequency;
	};
	// Passes gen by ref using 'auto&' or 'default_random_engine&'
	void drawCard(int hand[], int& count, card cards[], default_random_engine& generator, uniform_int_distribution<int>& distribution)
	{
		int drawnIndex;

		do // keep generating rand num until card is available
		{
			drawnIndex = distribution(generator);
		}
		while (cards[drawnIndex].frequency <= 0);

		hand[count] = drawnIndex; // Adds card to hand if available
		count++;

		cards[drawnIndex].frequency--; // Removes card from deck
		cout << endl;
	}

	void printHand(const int hand[], int count, const card cards[])
	{
		cout << "Your Hand:";
		for (int i = 0; i < count; i++)
		{
			int cardIndex = hand[i];
			cout << " |" << cards[cardIndex].name << "| ";
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


public:
	void play() override
	{
		default_random_engine generator(random_device{}());

		int hand[11];
		int count = 0;
		int currentHandValue = 0;
		int selectedAction = 0;
		bool playing = true;
		card cards[] = { {"2", 2, 4}, {"3", 3, 4}, {"4", 4, 4}, {"5", 5, 4}, {"6", 6, 4}, {"7", 7, 4}, {"8", 8, 4}, {"9", 9, 4}, {"10", 10, 4}, {"Jack", 10, 4}, {"Queen", 10, 4}, {"King", 10, 4}, {"Ace", 11, 4} };
		uniform_int_distribution<int> distribution(0, (sizeof(cards) / sizeof(cards[0])) - 1); // changed index so it works
		int randomNumber = distribution(generator);

		cout << "Blackjack selected. \n";

		bet = getBet();

		drawCard(hand, count, cards, generator, distribution); // changed to drawHand to drawCard so at start 2 cards are drawn
		drawCard(hand, count, cards, generator, distribution);

		printHand(hand, count, cards); // printHand function added to display the player's hand instead of using drawHand
		currentHandValue = calculateHandValue(hand, count, cards);

		cout << "Current Hand Value: " << currentHandValue << endl;
		cout << "Choose action: \n1. Hit\n2. Stand\n3. Double Down\n4. Forfeit(loses bet and exits blackjack)";
		cin >> selectedAction;
		switch (selectedAction)
		{
		case 1: //hit
			bool stand;
			stand = false;
			while ((!stand) && (currentHandValue < 20))
			{
				drawCard(hand, count, cards, generator, distribution);
				printHand(hand, count, cards);
				currentHandValue = calculateHandValue(hand, count, cards); // changed from += to = so that currentHandValue is recalc instead of added to
				cout << "Current Hand Value: " << currentHandValue << endl;
				cout << "Choose action: \n1. Hit\n2. Stand\n3. Forfeit(loses bet and exits blackjack)";
				cin >> selectedAction;
				if (selectedAction == 1) //hit
				{
					stand = false;
				}
				else if (selectedAction == 2) //stand
				{
					stand = true;
				}
				else if (selectedAction == 3) //forfeit
				{
					balance -= bet;
					cout << "You lost your bet of " << bet << ".\n";
					playing = false;
				}
			}
			if (currentHandValue == 21)
			{
				cout << "Blackjack! You win!\nNew Balance: " << balance+(bet*3);
			}
			break;
		case 2: //stand
			
			break;
		case 3: //double down
			bet *= 2;
			cout << "Double down selected. \nBet increased to: " << bet;
			break;
		case 4: //forfeit
			balance -= bet;
			cout << "You lost your bet of " << bet << ".\n";
			break;
		}
	}
};

class Slots : public Game
{
public:
	void play() override
	{

		//bool keepPlaying = true;
		//while (keepPlaying)
		//{
			//if (balance <= 0)
			//{
				//cout << "Insufficient balance to spin again. Returning to main menu.\n";
				//break;
			//}


			cout << "Slots selected. \n";

			bet = getBet();
			balance -= bet;

			// equal chance for numbers to be chosen
			int symbolPool[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
			int poolSize = sizeof(symbolPool) / sizeof(symbolPool[0]);

			// 3 x 1 reel
			int reel1 = symbolPool[rand() % poolSize];
			int reel2 = symbolPool[rand() % poolSize];
			int reel3 = symbolPool[rand() % poolSize];

			// display
			cout << "\n=========================\n";
			cout << "  [ " << reel1 << " ]  [ " << reel2 << " ]  [ " << reel3 << " ] \n";
			cout << "=========================\n\n";

			// results
			if (reel1 == reel2 && reel2 == reel3)
			{
				if (reel1 == 7)
				{
					// jackpot
					int winnings = bet * 10;
					balance += winnings;
					cout << "!! JACKPOT !! Three 7s!\n";
					cout << "You won 10x your bet: +" << winnings << "\n";
				}
				else
				{
					// others
					int winnings = bet * 2;
					balance += winnings;
					cout << "Match! Three of a kind!\n";
					cout << "You won 2x your bet: +" << winnings << "\n";
				}
			}
			else
			{
				cout << "No match. Better luck next spin!\n";
			}
			cout << "Current Balance: " << balance << "\n\n";

			//int gameChoice = 0;
			//cout << "Choose an option:\n1. Play Again\n2. Exit to Casino Menu\nChoice: ";
			//cin >> gameChoice;

			//if (gameChoice == 2)
			//{
				//keepPlaying = false;
			//}
		//}
	}
};

class Roulette : public Game
{
public:
	void play() override
	{
		cout << "Roulette selected. \n";
		bet = getBet();
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
	void playRace(int balance, int horseCount, double finishValue) {
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
	void play() override
	{
		// Horse Race game logic here
		while (true) {
			if (balance <= 0) {
				cout << "You do not have enough balance to play." << endl;
				break;
			}

			cout << "Horse Race selected. \n";
			bet = getBet();

			cout << "\n Choose an area: " << endl;

			cout << "1.Small Area			(3  horses)" << endl;
			cout << "2.Medium Area			(5  horses)" << endl;
			cout << "3.Large Area			(7  horses)" << endl;
			cout << "4.Extra Large Area		(10 horses)" << endl;
			cout << "5.Back" << endl;

			int choice;
			cout << "\nChoice: " << endl;
			cin >> choice;

			switch (choice) 
			{
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
				break;
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

	while (playing)
	{
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
			playing = false;
			break;
		default:
			cout << "Invalid selection." << endl;
			break;
		}
	}
	delete game;
	return 0;
}
