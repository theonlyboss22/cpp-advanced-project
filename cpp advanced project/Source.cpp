using namespace std;
#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <chrono>
#include <climits> // added to fix INT_MAX
#include <vector> // added for roullette game to store pocket numbers. -Zane
#include <algorithm> // added for find() function in roulette game. - Zane
#include "raylib.h" // yall will need to install raylib to compile this - Dom
#include <map> // added for card textures in blackjack - Dom


const int MAX_ACCOUNTS = 5;
const float BASE_WIDTH = 1920.0f;
const float BASE_HEIGHT = 1080.0f;

Vector2 mousePosition;
float scale;
float offsetX;
float offsetY;

//handles scaling of any vec2 - Dom
auto scale_transform_vec2 = [&](Vector2 orig) -> Vector2 {
	return Vector2{ (orig.x * scale) + offsetX, (orig.y * scale) + offsetY };
	};

//handles scaling of any rectangle - Dom
auto scale_transform_rect = [&](Rectangle orig) -> Rectangle {
	return Rectangle{ (orig.x * scale) + offsetX,(orig.y * scale) + offsetY,(orig.width * scale), (orig.height * scale), };
	};

struct Account // Account Information
{
	string username;
	string password;
	int balance;
};

int balance = 0;
bool playing = true;

void addBalance(int newBalance)
{
	balance += newBalance;
}

class Game
{
private:
	string name;
	int betAmount = 0;
	bool betLocked = false;
public:
	int bet = 0;
	virtual void play() = 0;
	int getBet()
	{
		Rectangle betLockButton = { 800, 600, 150, 50 };
		Rectangle betIncreaseButton = { 800 + ((strlen(to_string(betAmount).c_str()) * 20)), 500, 25, 25 };
		Rectangle betDecreaseButton = { 800 - 25, 500, 25, 25 };
		bool betIncreaseButtonPressed = CheckCollisionPointRec(mousePosition, scale_transform_rect(betIncreaseButton)) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
		bool betIncreaseButtonUp = CheckCollisionPointRec(mousePosition, scale_transform_rect(betIncreaseButton)) && IsMouseButtonUp(MOUSE_BUTTON_LEFT);
		bool betLockButtonPressed = CheckCollisionPointRec(mousePosition, scale_transform_rect(betLockButton)) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
		bool betLockButtonUp = CheckCollisionPointRec(mousePosition, scale_transform_rect(betLockButton)) && IsMouseButtonUp(MOUSE_BUTTON_LEFT);
		bool betDecreaseButtonPressed = CheckCollisionPointRec(mousePosition, scale_transform_rect(betDecreaseButton)) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
		bool betDecreaseButtonUp = CheckCollisionPointRec(mousePosition, scale_transform_rect(betDecreaseButton)) && IsMouseButtonUp(MOUSE_BUTTON_LEFT);

		DrawText("Select your bet amount:", 800, 300, 30, WHITE);
		DrawText(to_string(betAmount).c_str(), 800, 500, 30, WHITE);
		DrawText("+", 800 + ((strlen(to_string(betAmount).c_str()) * 20)), 500, 30, WHITE);
		DrawText("-", 800 - 25, 500, 30, WHITE);
		DrawText("Lock in bet", 800, 600, 30, WHITE);

		if (betIncreaseButtonPressed && betAmount < balance)
		{
			betAmount++;
		}
		if (betIncreaseButtonUp)
		{
			DrawText("+", 800 + ((strlen(to_string(betAmount).c_str()) * 20)), 500, 30, DARKGRAY);
		}
		if (betLockButtonPressed && betAmount > 0)
		{
			betLocked = true;
		}
		if (betLockButtonUp)
		{
			DrawText("Lock in bet", 800, 600, 30, DARKGRAY);
		}
		if (betDecreaseButtonPressed && betAmount > 0)
		{
			betAmount--;
		}
		if (betDecreaseButtonUp)
		{
			DrawText("-", 800 - 25, 500, 30, DARKGRAY);
		}

		return betLocked ? betAmount : 0;
	}

	void setName(string gameName)
	{
		name = gameName;
	}
	string getName()
	{
		return name;
	}
	bool isBetLocked() const
	{
		return betLocked;
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
		Texture2D texture;
	};

	map<string, Texture2D> cardTextures = { {"2", LoadTexture("assets/Deck Assets/2s.png")}, { "3", LoadTexture("assets/Deck Assets/3s.png")}, { "4", LoadTexture("assets/Deck Assets/4s.png")}, { "5", LoadTexture("assets/Deck Assets/5s.png")}, { "6", LoadTexture("assets/Deck Assets/6s.png")}, { "7", LoadTexture("assets/Deck Assets/7s.png")}, { "8", LoadTexture("assets/Deck Assets/8s.png")}, { "9", LoadTexture("assets/Deck Assets/9s.png")}, { "10", LoadTexture("assets/Deck Assets/Ts.png")}, { "Jack", LoadTexture("assets/Deck Assets/Js.png")}, { "Queen", LoadTexture("assets/Deck Assets/Qs.png")}, { "King", LoadTexture("assets/Deck Assets/Ks.png")}, { "Ace", LoadTexture("assets/Deck Assets/As.png")} };
	Texture2D blackjackBackground = LoadTexture("assets/background.png");

	// Passes gen by ref using 'auto&' or 'default_random_engine&'
	void drawCard(int hand[], int& count, card cards[], default_random_engine& generator, uniform_int_distribution<int>& distribution)
	{
		int drawnIndex;

		do // keep generating rand num until card is available
		{
			drawnIndex = distribution(generator);
		} while (cards[drawnIndex].frequency <= 0);

		hand[count] = drawnIndex; // Adds card to hand if available
		count++;

		cards[drawnIndex].frequency--; // Removes card from deck
		cout << endl;
	}

	void printHand(const int hand[], int count, card cards[])
	{
		cout << "Your Hand:";
		for (int i = 0; i < count; i++)
		{
			int cardIndex = hand[i];
			cards[cardIndex].texture = cardTextures[cards[cardIndex].name];
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

	// Win/Lose logic
	bool evaluateHand(int currentHandValue, int currentBet)
	{
		if (currentHandValue == 21)
		{
			balance += currentBet;
			cout << "Blackjack! You win!\nNew Balance: " << balance << "\n";
			return true; // Round is over
		}
		else if (currentHandValue > 21)
		{
			balance -= currentBet;
			cout << "You Busted!\nCurrent Balance: " << balance << ".\n";
			return true; // Round is over
		}
		return false; // Game continues
	}

public:
	void play() override
	{

		default_random_engine generator(random_device{}());

		int hand[11];
		int count = 0;
		int currentHandValue = 0;
		bool playerTurn = true;

		setName("Blackjack");

		DrawTexture(blackjackBackground, 0, 0, WHITE);
		DrawText(getName().c_str(), 800, 100, 50, WHITE);
		DrawText(("Balance: $" + to_string(balance)).c_str(), 800, 200, 30, WHITE);

		card cards[] = { {"2", 2, 4}, {"3", 3, 4}, {"4", 4, 4}, {"5", 5, 4}, {"6", 6, 4}, {"7", 7, 4}, {"8", 8, 4}, {"9", 9, 4}, {"10", 10, 4}, {"Jack", 10, 4}, {"Queen", 10, 4}, {"King", 10, 4}, {"Ace", 11, 4} };
		uniform_int_distribution<int> distribution(0, (sizeof(cards) / sizeof(cards[0])) - 1); // changed index so it works
		int randomNumber = distribution(generator);

		if (!isBetLocked())
		{
			bet = getBet();
			if (bet == 0)
			{
				return;
			}
		}

		/*
		int selectedAction = 0;


		cout << "Blackjack selected. \n";


		drawCard(hand, count, cards, generator, distribution); // changed to drawHand to drawCard so at start 2 cards are drawn. -Zane
		drawCard(hand, count, cards, generator, distribution);

		printHand(hand, count, cards); // printHand function added to display the player's hand instead of using drawHand
		currentHandValue = calculateHandValue(hand, count, cards);

		cout << "Current Hand Value: " << currentHandValue << endl;

		// Check initial hand for win condition
		if (evaluateHand(currentHandValue, bet))
		{
			return;
		}

		while (playerTurn) // Added to determine when player can and can't play. -Zane
		{
			cout << "Choose action: \n1. Hit\n2. Stand\n3. Double Down\n4. Forfeit(loses bet and exits blackjack)\n";
			cin >> selectedAction;
			switch (selectedAction)
			{
			case 1: //hit
				bool stand;
				stand = false;
				drawCard(hand, count, cards, generator, distribution);
				printHand(hand, count, cards);
				currentHandValue = calculateHandValue(hand, count, cards); // changed from += to = so that currentHandValue is recalc instead of added to. -Zane
				cout << "Current Hand Value: " << currentHandValue << endl;

				if (evaluateHand(currentHandValue, bet))
				{
					playerTurn = false;
				}
				break;
			case 2: //stand
				cout << "You stood with a hand value of " << currentHandValue << ".\n";
				playerTurn = false;
				break;
			case 3: //double down
				bet *= 2;
				cout << "Double down selected. \nBet increased to: " << bet;
				drawCard(hand, count, cards, generator, distribution); // Draws extra card now, logic was incorrect before. -Zane
				printHand(hand, count, cards);
				currentHandValue = calculateHandValue(hand, count, cards);
				cout << "Current Hand Value: " << currentHandValue << endl;

				if (!evaluateHand(currentHandValue, bet))
				{
					cout << "Stand after double down. Final value: " << currentHandValue << "\n";
				}
				playerTurn = false;
				break;
			case 4: //forfeit
				balance -= bet;
				cout << "You lost your bet of " << bet << ".\n";
				playerTurn = false;
				break;

			default: // Added default didn't know if this was necessary but I added it just in case -Zane
				cout << "Invalid action. Try again.\n";
				break;

			}
		}
		*/
		for (auto& element : cardTextures)
		{
			UnloadTexture(element.second);
		}
	}
};

class Slots : public Game
{
public:
	void play() override
	{
		setName("Slots");

		Texture2D background = LoadTexture("assets/background.png"); // gets background image from assets folder and loads it in memory - Dom
		DrawTexture(background, 0, 0, WHITE);
		DrawText(getName().c_str(), 800, 100, 50, WHITE);
		DrawText(("Balance: $" + to_string(balance)).c_str(), 800, 200, 30, WHITE);

		/*bool keepPlaying = true;
		while (keepPlaying)
		{
			if (balance <= 0)
			{
				cout << "Insufficient balance to spin again. Returning to main menu.\n";
				break;
			}


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

			int gameChoice = 0;
			cout << "Choose an option:\n1. Play Again\n2. Exit to Casino Menu\nChoice: ";
			cin >> gameChoice;

			if (gameChoice == 2)
			{
				keepPlaying = false;
			}
		}*/
	}
};

class Roulette : public Game // Recycled code from previous project of mine, and from Slots + Blackjack. -Zane
{
private:
	struct PocketGroup // holds color name and associated pocket numbers
	{
		string colorName;
		vector<int> pocketNumbers;
	};

public:
	void play() override
	{
		setName("Roulette");

		Texture2D background = LoadTexture("assets/background.png"); // gets background image from assets folder and loads it in memory - Dom
		DrawTexture(background, 0, 0, WHITE);
		DrawText(getName().c_str(), 800, 100, 50, WHITE);
		DrawText(("Balance: $" + to_string(balance)).c_str(), 800, 200, 30, WHITE);

		/*bool keepPlaying = true;
		while (keepPlaying)
		{
			if (balance <= 0)
			{
				cout << "Insufficient balance to play Roulette. Returning to main menu.\n";
				break;
			}

			cout << "Roulette selected. \n";
			bet = getBet();
			balance -= bet;

			int choice = 0;
			int specificNumberBet = -1; // To store the chosen number if applicable
			cout << "\nChoose your bet type:\n";
			cout << "1. Red (Pays 2x)\n";
			cout << "2. Black (Pays 2x)\n";
			cout << "3. Even (Pays 2x)\n";
			cout << "4. Odd (Pays 2x)\n";
			cout << "5. Specific Number (0-36) (Pays 35x)\n";
			cout << "Choice: ";
			cin >> choice;

			if (choice == 5)
			{
				cout << "Enter a number to bet on (0-36): ";
				cin >> specificNumberBet;
				if (specificNumberBet < 0 || specificNumberBet > 36)
				{
					cout << "Invalid number. Returning your bet and exiting to menu.\n";
					balance += bet;
					break;
				}
			}
			else if (choice < 1 || choice > 4)
			{
				cout << "Invalid choice. Returning your bet and exiting to menu.\n";
				balance += bet;
				break;
			}

			default_random_engine generator(random_device{}());
			uniform_int_distribution<int> wheelDistribution(0, 36);
			int winningPocket = wheelDistribution(generator);

			cout << "\nThe roulette wheel spins...\n";
			cout << "Winning Number: " << winningPocket << endl;

			PocketGroup blackGroup = { "Black", {2, 4, 6, 8, 10, 11, 13, 15, 17, 20, 22, 24, 26, 28, 29, 31, 33, 35} };
			PocketGroup redGroup = { "Red", {1, 3, 5, 7, 9, 12, 14, 16, 18, 19, 21, 23, 25, 27, 30, 32, 34, 36} };
			PocketGroup greenGroup = { "Green", {0} };

			vector<PocketGroup> allPocketGroups = { blackGroup, redGroup, greenGroup }; // Groups all pocket groups into a vector for easier access
			vector<int> targetNumbers = { winningPocket }; // holds the winning pocket number for comparison

			string winningColor = "";
			for (int target : targetNumbers)
			{
				for (const auto& group : allPocketGroups) // searches through each pocket group to find the winning color
				{
					if (find(group.pocketNumbers.begin(), group.pocketNumbers.end(), target) != group.pocketNumbers.end())
					{
						winningColor = group.colorName;
						cout << group.colorName << endl;
					}
				}
			}

			bool isEven = false;
			if (winningPocket == 0) // checks if the winning pocket is 0
			{
				// Zero is neither even nor odd
			}
			else if (winningPocket % 2 == 0) // check for even logic
			{
				cout << "Even" << endl;
				isEven = true;
			}
			else // check for odd logic
			{
				cout << "Odd" << endl;
				isEven = false;
			}

			bool won = false;
			if (choice == 1 && winningColor == "Red")
			{
				won = true;
			}
			else if (choice == 2 && winningColor == "Black")
			{
				won = true;
			}
			else if (choice == 3 && winningPocket != 0 && isEven)
			{
				won = true;
			}
			else if (choice == 4 && winningPocket != 0 && !isEven)
			{
				won = true;
			}
			else if (choice == 5 && winningPocket == specificNumberBet)
			{
				won = true;
				int payoutMultiplier = 35;
			}

			if (won)
			{
				int winnings = bet * 2;
				balance += winnings;
				cout << "You won! Payout: +" << winnings << "\n";
			}
			else
			{
				cout << "You lost your bet.\n";
			}

			cout << "Current Balance: " << balance << "\n\n";

			int gameChoice = 0;
			cout << "Choose an option:\n1. Play Again\n2. Exit to Casino Menu\nChoice: ";
			cin >> gameChoice;

			if (gameChoice == 2)
			{
				keepPlaying = false;
			}
		}*/
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
			distance += 0.59;
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
	void playRace(int balance, int horseCount, double finishValue)
	{

		while (true)
		{

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
				cout << "==========================" << endl;
				cout << "       HORSE RACE" << endl;
				cout << "==========================\n" << endl;

				cout << "Balance: $" << balance << endl;
				cout << "\nHorse:" << endl;
				for (int i = 0; i < horseCount; i++) {
					cout << i + 1 << ". " << horse[i].getName() << endl;
				}

				cout << "\nChoose your horse: " << endl;
				cin >> selectHorse;

				if (cin.fail())
				{
					cout << "Invalid input. Please enter a number.\n";

					cin.clear();
					cin.ignore(INT_MAX, '\n');

					continue;
				}

				if (selectHorse < 1 || selectHorse > horseCount) {
					cout << "Invalid selection." << endl;
					continue;
				}

				bet = getBet();

				cout << "\nYou selected " << horse[selectHorse - 1].getName() << endl;
				cout << "Bet: $" << bet << endl;

				cout << "\nStart race? (yes/no): " << endl;
				cin >> startChoice;

				if (startChoice == "yes") {
					break;
				}
				else if (startChoice == "no") {
					cout << "\nnChoose your horse and bet again." << endl;
				}
				else {
					cout << "Invalid Enter, try again." << endl;
				}
			}

			// Take Bet
			balance -= bet;

			// Race
			int finishedHorses = 0;

			cout << "==========================" << endl;
			cout << "      RACE START!" << endl;
			cout << "==========================\n" << endl;

			while (finishedHorses < horseCount) {
				for (int i = 0; i < horseCount; i++) {
					if (!horse[i].isFinished()) {
						horse[i].run();

						if (horse[i].getDistance() >= finishValue) {
							finishedHorses++;

							horse[i].setFinished(true);
							horse[i].setPlace(finishedHorses);

							cout << horse[i].getName() << " finished in plance " << finishedHorses << "!" << endl;
						}
					}
				}
			}

			// Show Results
			cout << "==========================" << endl;
			cout << "       RACE RESULTS" << endl;
			cout << "==========================\n" << endl;

			for (int place = 1; place <= horseCount; place++) {
				for (int i = 0; i < horseCount; i++) {
					if (horse[i].getPlace() == place) {
						cout << place << ". " << horse[i].getName() << endl;
					}
				}
			}

			// Determine Multiplier
			double multiplier = 0;
			int playerPlace = horse[selectHorse - 1].getPlace();

			if (horseCount == 3) {
				if (playerPlace == 1) {
					multiplier = 2.0;
				}
			}
			else if (horseCount == 5) {
				if (playerPlace == 1) {
					multiplier = 3.0;
				}
				else if (playerPlace == 2) {
					multiplier = 2.0;
				}
				else if (playerPlace == 3) {
					multiplier = 1.5;
				}
			}
			else if (horseCount == 7) {
				if (playerPlace == 1) {
					multiplier = 4.0;
				}
				else if (playerPlace == 2) {
					multiplier = 3.0;
				}
				else if (playerPlace == 3) {
					multiplier = 2.0;
				}
				else if (playerPlace == 4) {
					multiplier = 1.5;
				}
			}
			else if (horseCount == 10) {
				if (playerPlace == 1) {
					multiplier = 5.0;
				}
				else if (playerPlace == 2) {
					multiplier = 4.0;
				}
				else if (playerPlace == 3) {
					multiplier = 3.0;
				}
				else if (playerPlace == 4) {
					multiplier = 2.0;
				}
				else if (playerPlace == 5) {
					multiplier = 1.5;
				}
			}

			//Payout Amount
			cout << "\nYour horse: " << horse[selectHorse - 1].getName() << endl;
			cout << "Your horse finished in place " << playerPlace << "." << endl;

			if (multiplier > 0) {
				int payout = bet * multiplier;
				balance += payout;

				cout << "Multiplier: x" << multiplier << endl;
				cout << "You won: $" << payout << endl;
			}
			else {
				cout << "Multiplier: N/A" << endl;
				cout << "You lost your bet." << endl;
			}

			cout << "\nCurrent balance: $" << balance << endl;

			//Play Again / Back
			string nextChoice;
			while (true) {
				cout << "\nType 'again' to play this area again" << endl;
				cout << "Type 'back' to choose another area" << endl;

				cout << "\nChoice: " << endl;
				cin >> nextChoice;

				if (nextChoice == "again") {
					break;
				}
				else if (nextChoice == "back") {
					return;
				}
				else {
					cout << "Invalid choice." << endl;
				}
			}

		}
	}
public:
	void play() override
	{
		setName("Horse Race");

		Texture2D background = LoadTexture("assets/background.png"); // gets background image from assets folder and loads it in memory - Dom
		DrawTexture(background, 0, 0, WHITE);
		DrawText(getName().c_str(), 800, 100, 50, WHITE);
		DrawText(("Balance: $" + to_string(balance)).c_str(), 800, 200, 30, WHITE);

		/*while (true)
		{
			if (balance <= 0) {
				cout << "You do not have enough balance to play." << endl;
				break;
			}

			cout << "==========================" << endl;
			cout << "    Horse Race selected" << endl;
			cout << "==========================" << endl;

			cout << "\n Choose an area: " << endl;

			if (cin.fail())
			{
				cout << "Invalid input. Please enter a number.\n";

				cin.clear();
				cin.ignore(INT_MAX, '\n');

				continue;
			}

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
				cout << "Returning to Game Menu\n" << endl;
				return;
			default:
				cout << "Invalid selection." << endl;
				break;
			}

		}*/
	}
};

int main()
{
	// ACCOUNT SYSTEM
	Account accounts[MAX_ACCOUNTS];
	int accountCount = 0;
	int currentAccount = -1;
	int choice;
	string username;
	string password;

	// read saved accounts from file
	ifstream inFile("accounts.txt");
	if (inFile)
	{
		while (accountCount < MAX_ACCOUNTS &&
			inFile >> accounts[accountCount].username
			>> accounts[accountCount].password
			>> accounts[accountCount].balance)
		{
			accountCount++;
		}
		inFile.close();
	}

	// save all accounts and balances
	auto saveAccounts = [&]()
		{
			ofstream outFile("accounts.txt");
			for (int i = 0; i < accountCount; i++)
			{
				outFile << accounts[i].username << " " << accounts[i].password << " " << accounts[i].balance << endl;
			}
		};

	// login and sign in
	while (currentAccount == -1)
	{
		cout << "CASINO GAME\n" << endl;
		cout << "1. Login" << endl;
		cout << "2. Create Account" << endl;
		cout << "3. Exit" << endl;
		cout << "\nChoose an option: ";
		cin >> choice;

		if (choice == 3)
		{
			cout << "Goodbye!" << endl;
			return 0;
		}
		else if (choice == 1)
		{
			cout << "\nUsername: ";
			cin.ignore(INT_MAX, '\n');
			cin >> username;
			cout << "Password: ";
			cin >> password;

			for (int i = 0; i < accountCount; i++)
			{
				if (accounts[i].username == username && accounts[i].password == password)
				{
					currentAccount = i;
					break;
				}
			}

			if (currentAccount != -1)
			{
				cout << "\nLogin successful!" << endl;
				cout << "Welcome, " << accounts[currentAccount].username << "!" << endl;
				cout << "Your balance: $" << accounts[currentAccount].balance << endl;
			}
			else
			{
				cout << "\nInvalid username or password." << endl;
			}
		}
		else if (choice == 2)
		{
			if (accountCount >= MAX_ACCOUNTS)
			{
				cout << "\nThe maximum of 5 accounts has been reached." << endl;
				continue;
			}

			cout << "\nCreate username: ";
			cin >> username;
			bool usernameExists = false;
			for (int i = 0; i < accountCount; i++)
			{
				if (accounts[i].username == username)
				{
					usernameExists = true;
					break;
				}
			}
			if (usernameExists)
			{
				cout << "Username already exists." << endl;
				continue;
			}

			cout << "Create password: ";
			cin >> password;

			accounts[accountCount].username = username;
			accounts[accountCount].password = password;
			accounts[accountCount].balance = 1000;
			currentAccount = accountCount;
			accountCount++;
			saveAccounts();

			cout << "\nAccount created successfully!" << endl;
			cout << "Welcome, " << username << "!" << endl;
			cout << "Starting balance: $1000" << endl; //  new account starts with a balance of $1000
		}
		else
		{
			cout << "\nInvalid choice. Please enter login, sign, or exit." << endl;
		}
	}

	// synchronize the current account balance to the balance
	balance = accounts[currentAccount].balance;

	bool bjButtonDown = false;
	bool bjButtonUp = false;
	bool slotsButtonDown = false;
	bool slotsButtonUp = false;
	bool rouletteButtonDown = false;
	bool rouletteButtonUp = false;
	bool horseRaceButtonDown = false;
	bool horseRaceButtonUp = false;
	bool exitButtonDown = false;
	bool exitButtonUp = false;
	Vector2 basicButtonScaleXY = { 50, 50 };
	SetConfigFlags(FLAG_WINDOW_RESIZABLE); // allows window to be resized - Dom
	InitWindow(BASE_WIDTH, BASE_HEIGHT, "CasinoGame"); // initializes window with base width and height resolution - Dom
	SetTargetFPS(60);

	Texture2D background = LoadTexture("assets/background.png"); // gets background image from assets folder and loads it in memory - Dom
	Texture2D gameOverScreen = LoadTexture("assets/gameOver.png"); // gets game over image from assets folder and loads it in memory - Dom

	RenderTexture2D canvas = LoadRenderTexture(BASE_WIDTH, BASE_HEIGHT); // creates a render texture to scale to and render our window within - Dom

	Rectangle blackjackButton = { 100, 200, basicButtonScaleXY.x, basicButtonScaleXY.y }; // button to play blackjack (x, y, width, height) - Dom
	Rectangle slotsButton = { 100, 300, basicButtonScaleXY.x, basicButtonScaleXY.y }; // button to play slots (x, y, width, height) - Dom
	Rectangle rouletteButton = { 100, 400, basicButtonScaleXY.x, basicButtonScaleXY.y }; // button to play roulette (x, y, width, height) - Dom
	Rectangle horseRaceButton = { 100, 500, basicButtonScaleXY.x, basicButtonScaleXY.y }; // button to play horse race (x, y, width, height) - Dom
	Rectangle exitButton = { 100, 600, basicButtonScaleXY.x, basicButtonScaleXY.y }; // button to exit game (x, y, width, height) - Dom
	Game* game = nullptr;

	// this block renders all our textures and text - Dom
	while (!WindowShouldClose())
	{
		mousePosition = GetMousePosition(); // gets mouse position - Dom
		scale = min(GetScreenWidth() / BASE_WIDTH, GetScreenHeight() / BASE_HEIGHT); // determining scale for render window - Dom
		offsetX = (GetScreenWidth() - BASE_WIDTH * scale) * 0.5f; // determining offset for render window - Dom
		offsetY = (GetScreenHeight() - BASE_HEIGHT * scale) * 0.5f; // determining offset for render window - Dom
		BeginTextureMode(canvas); // sets the render texture as the target for drawing - Dom
		DrawTexture(background, 0, 0, WHITE);
		DrawText("Casino Game", 800, 100, 50, WHITE);
		DrawText(("Balance: $" + to_string(balance)).c_str(), 800, 200, 30, WHITE);
		DrawRectangleRec(blackjackButton, BLACK); // draws the button to play blackjack - Dom
		DrawText("Blackjack", blackjackButton.x + blackjackButton.width + 25, (blackjackButton.y + (blackjackButton.height / 3)), 30, WHITE);
		DrawRectangleRec(slotsButton, BLACK); // draws the button to play slots - Dom
		DrawText("Slots", slotsButton.x + slotsButton.width + 25, (slotsButton.y + (slotsButton.height / 3)), 30, WHITE);
		DrawRectangleRec(rouletteButton, BLACK); // draws the button to play roulette - Dom
		DrawText("Roulette", rouletteButton.x + rouletteButton.width + 25, (rouletteButton.y + (rouletteButton.height / 3)), 30, WHITE);
		DrawRectangleRec(horseRaceButton, BLACK); // draws the button to play horse race - Dom
		DrawText("Horse Race", horseRaceButton.x + horseRaceButton.width + 25, (horseRaceButton.y + (horseRaceButton.height / 3)), 30, WHITE);
		DrawRectangleRec(exitButton, BLACK); // draws the button to exit game - Dom
		DrawText("Exit", exitButton.x + exitButton.width + 25, (exitButton.y + (exitButton.height / 3)), 30, WHITE);
		if (bjButtonDown)
		{
			if (game == nullptr)
			{
				game = new Blackjack();
			}
		}
		if (bjButtonUp)
		{
			DrawRectangleRec(blackjackButton, DARKGRAY);
		}
		if (slotsButtonDown)
		{
			if (game == nullptr)
			{
				game = new Slots();
			}
		}
		if (slotsButtonUp)
		{
			DrawRectangleRec(slotsButton, DARKGRAY);
		}
		if (rouletteButtonDown)
		{
			if (game == nullptr)
			{
				game = new Roulette();
			}
		}
		if (rouletteButtonUp)
		{
			DrawRectangleRec(rouletteButton, DARKGRAY);
		}
		if (horseRaceButtonDown)
		{
			if (game == nullptr)
			{
				game = new HorseRace();
			}
		}
		if (horseRaceButtonUp)
		{
			DrawRectangleRec(horseRaceButton, DARKGRAY);
		}
		if (exitButtonDown)
		{
			break;
		}
		if (exitButtonUp)
		{
			DrawRectangleRec(exitButton, DARKGRAY);
		}
		if (game != nullptr)
		{
			game->play();
		}
		if (balance <= 0)
		{
			DrawTexture(gameOverScreen, 0, 0, WHITE);
			accounts[currentAccount].balance = balance;
			saveAccounts();
			break;
		}
		EndTextureMode();

		bjButtonDown = false;
		bjButtonUp = false;
		slotsButtonDown = false;
		slotsButtonUp = false;
		rouletteButtonDown = false;
		rouletteButtonUp = false;
		horseRaceButtonDown = false;
		horseRaceButtonUp = false;
		exitButtonDown = false;
		exitButtonUp = false;

		Rectangle sourceRec = { 0.0f, 0.0f, BASE_WIDTH, -BASE_HEIGHT }; // source rectangle for render texture - Dom
		Rectangle destRec = { offsetX, offsetY, BASE_WIDTH * scale, BASE_HEIGHT * scale }; // destination rectangle for render texture - Dom



		//all of these are getting the mouse position and checking if the mouse is over the button and if the mouse button is down or up - Dom
		bjButtonDown = (CheckCollisionPointRec(mousePosition, scale_transform_rect(blackjackButton)) && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
		bjButtonUp = (CheckCollisionPointRec(mousePosition, scale_transform_rect(blackjackButton)) && IsMouseButtonUp(MOUSE_BUTTON_LEFT));
		slotsButtonDown = (CheckCollisionPointRec(mousePosition, scale_transform_rect(slotsButton)) && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
		slotsButtonUp = (CheckCollisionPointRec(mousePosition, scale_transform_rect(slotsButton)) && IsMouseButtonUp(MOUSE_BUTTON_LEFT));
		rouletteButtonDown = (CheckCollisionPointRec(mousePosition, scale_transform_rect(rouletteButton)) && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
		rouletteButtonUp = (CheckCollisionPointRec(mousePosition, scale_transform_rect(rouletteButton)) && IsMouseButtonUp(MOUSE_BUTTON_LEFT));
		horseRaceButtonDown = (CheckCollisionPointRec(mousePosition, scale_transform_rect(horseRaceButton)) && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
		horseRaceButtonUp = (CheckCollisionPointRec(mousePosition, scale_transform_rect(horseRaceButton)) && IsMouseButtonUp(MOUSE_BUTTON_LEFT));
		exitButtonDown = (CheckCollisionPointRec(mousePosition, scale_transform_rect(exitButton)) && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
		exitButtonUp = (CheckCollisionPointRec(mousePosition, scale_transform_rect(exitButton)) && IsMouseButtonUp(MOUSE_BUTTON_LEFT));


		//draws the scaled canvas with all elements drawn within it to the window - Dom
		BeginDrawing();
		ClearBackground(BLACK);
		DrawTexturePro(canvas.texture, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE); // draws the scaled and offset canvas to the window - Dom
		EndDrawing();
	}

	UnloadRenderTexture(canvas);
	UnloadTexture(background);
	delete game;
	CloseWindow();
	return 0;
}
