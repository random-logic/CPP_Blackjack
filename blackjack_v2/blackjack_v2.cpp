#include <iostream>
#include <vector>
#include <string>
#include "blackjack.hpp"
using namespace std;

//Function prototypes
int charToInt(char char_to_convert);
int stringToInt(string string_to_convert);
void initGameCards();
void initGameBets();
Blackjack::Choice getUserChoiceRound(int hand_index);
void outputRoundWinnerAndCalculateBets(Blackjack::Hand winner, int hand_index);
void placeBet();

//Make blackjack game class
Blackjack blackjack;

int main()
{
	random::init(); //Init random number generator

	initGameCards(); //Init game card deck

	initGameBets(); //Init game bets

	//If user does not want to bet any money
	if (blackjack.money_to_bet <= 0) {
		//Return funny message an quit program
		cout << "You have no money, how can you gamble?" << endl;
		return 0;
	}

	//Play game until money is out or win goal reached
	int round_counter = 0;
	while (blackjack.money_to_bet > 0 && blackjack.money_to_bet < blackjack.money_to_win) {
		//Declare new round
		blackjack.newRound();

		//Output round count
		cout << "----------------------------------------------" << endl;
		cout << "Round " << ++round_counter << ':' << endl;

		//Prompt user to place a bet
		placeBet();

		//Prompt user for action and take action until user decides to stay
		//Loop through the hand counts so player can decide for every hand they have
		//The first hand starts at 0
		int hand_count = 0;
		while (hand_count < blackjack.getPlayerHands().size()) {
			Blackjack::Choice choice = getUserChoiceRound(hand_count); //Get choice for the hand selected

			//Since the take player action function does not support stay or quit values, manually program it here
			if (choice == Blackjack::STAY)
				hand_count++;
			else if (choice == Blackjack::QUIT) {
				cout << "----------------------------------------------" << endl;
				cout << "You lost your opportunity to reach your goal of $" << blackjack.money_to_win << endl;
				//User does not get the money bet for that round back to penalize them from not completing the round
				cout << "However, you still walk out with $" << blackjack.money_to_bet << endl;
				cout << "Thank you for playing blackjack at this casino!" << endl;
				return 0;
			}
			else
				blackjack.takePlayerAction(choice, hand_count); //Take player action
		}

		//Have dealer take their turn
		blackjack.dealerTakeTurn();

		//Determine the winner of round and calculate bets
		for (int i = 0; i < blackjack.getPlayerHands().size(); i++) {
			Blackjack::Hand winner = blackjack.findRoundWinner(i);
			outputRoundWinnerAndCalculateBets(winner, i);
		}

		//End round with dash lines
		cout << "----------------------------------------------" << endl << endl;
	}

	//Broadcast whether the person won or lost based on the money amount
	if (blackjack.money_to_bet >= blackjack.money_to_win) {
		cout << "You win! Congradulations on achieving your goal of $" << blackjack.money_to_win << '.' << endl;
	}
	else if (blackjack.money_to_bet > 0) {
		cout << "You quit without using all of your money or achieving the goal of $" << blackjack.money_to_win << '!' << endl;
	}
	else {
		cout << "You lose! You went bankrupt (RIP)." << endl;
	}

	cout << "Thank you for playing blackjack at this casino!" << endl;

	return 0;
}

//Convert any single character from zero to nine
//Will return -1 if invalid input
int charToInt(char char_to_convert) {
	int char_converted = static_cast<int>(char_to_convert) - static_cast<int>('0');
	if (char_converted >= 0 && char_converted <= 9) {
		return char_converted;
	}
	else {
		return -1;
	}
}

//Convert any single string to an int
//Will return -1 if any invalid input
int stringToInt(string string_to_convert) {
	int int_to_return = 0;

	for (int i = 0; i < string_to_convert.size(); i++) {
		if (charToInt(string_to_convert[i]) != -1) {
			int_to_return += charToInt(string_to_convert[i]) * static_cast<int>(powf(10, static_cast<int>(string_to_convert.size()) - i));
		}
		else {
			return -1;
		}
	}

	return int_to_return / 10;
}

//Initialize game card deck
void initGameCards() {
	//Prompt user what this is
	cout << "This is a game of Blackjack!" << endl;
	cout << "Your objective is to win money." << endl;

	//Ask user how many decks of cards they want to play with
	string* amt_of_decks = new string;
	cout << "How many decks of card (1-" << Blackjack::MAX_DECKS_ALLOWED << ") should be included in the game? ";
	getline(cin, *amt_of_decks);

	//Validate user input
	while (stringToInt(*amt_of_decks) <= 0 || stringToInt(*amt_of_decks) > Blackjack::MAX_DECKS_ALLOWED) {
		cout << "Please enter a valid number from 1-" << Blackjack::MAX_DECKS_ALLOWED << ": ";
		getline(cin, *amt_of_decks);
	}

	//Init card deck
	blackjack.initGame(stringToInt(*amt_of_decks));
	delete amt_of_decks;
}

//Init game bets
void initGameBets() {
	//Ask the user how much money they want
	string* user_money_amt = new string;
	cout << "How much money (int) do you want to gamble? $";
	getline(cin, *user_money_amt);

	//Validate user input
	while (stringToInt(*user_money_amt) == -1) {
		cout << "Please enter a valid amount of money (int): $";
		getline(cin, *user_money_amt);
	}

	//Set the amount of money user has to bet in the blackjack class and delete pointer variable
	blackjack.money_to_bet = stringToInt(*user_money_amt);
	delete user_money_amt;

	//Ask the user how much money they are seeking to win
	string* win_money_amt = new string;
	cout << "How much money (int) do you seek to win? $";
	getline(cin, *win_money_amt);

	//Validate user input
	while (stringToInt(*win_money_amt) == -1 || stringToInt(*win_money_amt) <= blackjack.money_to_bet) {
		cout << "Please enter a valid amount of money greater than the money (int) you have: $";
		getline(cin, *win_money_amt);
	}

	//Set the amount of money needed to win in the blackjack class and delete pointer variable
	blackjack.money_to_win = stringToInt(*win_money_amt);
	delete win_money_amt;
}

//Get user choice for what they want to do with their hand
Blackjack::Choice getUserChoiceRound(int hand_index) {
	//If the player has a hard bust already, no need to get user choice
	if (blackjack.getSumOfHand(blackjack.getPlayerHands()[hand_index]).hard_sum >= Blackjack::BLACK_JACK_VALUE) {
		return Blackjack::STAY;
	}

	//Display to the user the current situation of the cards
	cout << "The dealer has the following cards:" << endl;
	cout << "1) " << Cards::getNumberOfCard(blackjack.getDealerHand()[0]) << ' ' << Cards::getPatternOfCard(blackjack.getDealerHand()[0]) << endl;
	cout << "2) Hidden" << endl;
	cout << "You have the following cards in hand " << hand_index + 1 << ':' << endl;
	for (int i = 0; i < blackjack.getPlayerHands()[hand_index].size(); i++) {
		cout << i + 1 << ") " << Cards::getNumberOfCard(blackjack.getPlayerHands()[hand_index][i]) << ' ' << Cards::getPatternOfCard(blackjack.getPlayerHands()[hand_index][i]) << endl;
	}

	//Determine whether the user's hand is eligible to split and if the user has enough money to split
	string * input = new string;
	if (blackjack.handCanSplit(hand_index) && blackjack.money_to_bet >= blackjack.bet_placed) {
		//Get input and include option to split
		cout << "Hit(1), Stay(2), Split(3), or Quit(4)? ";
		getline(cin, *input);

		//Check to see if input is valid
		while (stringToInt(*input) < 1 || stringToInt(*input) > 4) {
			cout << "Please enter a valid integer from 1-8: ";
			getline(cin, *input);
		}

		//Determine user action based on input
		int int_input = stringToInt(*input);
		delete input;
		if (int_input == 1) {
			return Blackjack::HIT;
		}
		else if (int_input == 2) {
			return Blackjack::STAY;
		}
		else if (int_input == 3) {
			blackjack.money_to_bet -= blackjack.bet_placed;
			return Blackjack::SPLIT;
		}
		else if (int_input == 4) {
			return Blackjack::QUIT;
		}
	}
	else {
		//Get input and exclude option to split
		cout << "Hit(1), Stay(2), or Quit(3)? ";
		getline(cin, *input);

		//Check to see if input is valid
		while (stringToInt(*input) < 1 || stringToInt(*input) > 3) {
			cout << "Please enter a valid integer from 1-3: ";
			getline(cin, *input);
		}

		//Determine user action based on input
		int int_input = stringToInt(*input);
		delete input;
		if (int_input == 1) {
			return Blackjack::HIT;
		}
		else if (int_input == 2) {
			return Blackjack::STAY;
		}
		else if (int_input == 3) {
			return Blackjack::QUIT;
		}
	}
}

void outputRoundWinnerAndCalculateBets(Blackjack::Hand winner, int hand_index) {
	//Output all dealer cards
	cout << "The dealer has the following cards:" << endl;
	for (int i = 0; i < blackjack.getDealerHand().size(); i++) {
		cout << i + 1 << ") " << Cards::getNumberOfCard(blackjack.getDealerHand()[i]) << ' ' << Cards::getPatternOfCard(blackjack.getDealerHand()[i]) << endl;
	}

	//Output all player cards
	cout << "The player has the following cards for hand " << hand_index + 1 << ":" << endl;
	for (int i = 0; i < blackjack.getPlayerHands()[0].size(); i++) {
		cout << i + 1 << ") " << Cards::getNumberOfCard(blackjack.getPlayerHands()[hand_index][i]) << ' ' << Cards::getPatternOfCard(blackjack.getPlayerHands()[hand_index][i]) << endl;
	}

	//Say who wins the round based on the parameter winner and calculate bets accordingly
	if (winner == Blackjack::DEALER) {
		cout << "Dealer wins for this hand!" << endl; //Don't return bet
	}
	else if (winner == Blackjack::PLAYER) {
		cout << "Player wins for this hand!" << endl;
		blackjack.money_to_bet += blackjack.bet_placed * 2; //Return bet with double the amount for winning
	}
	else if (winner == Blackjack::PUSH) {
		cout << "Draw for this hand!" << endl;
		blackjack.money_to_bet += blackjack.bet_placed; //Return the original bet
	}
}

void placeBet() {
	//Ask the user how much money they want to bet for the round
	string* user_bet_amt = new string;
	cout << "You have $" << blackjack.money_to_bet << endl;
	cout << "How much money (int) do you want to bet? $";
	getline(cin, *user_bet_amt);

	//Validate user input
	while (stringToInt(*user_bet_amt) == -1 || stringToInt(*user_bet_amt) > blackjack.money_to_bet) {
		cout << "Please enter a valid amount of money (int) within your money you have to bet: $";
		getline(cin, *user_bet_amt);
	}

	//Set the bet in the blackjack class and delete pointer variable
	blackjack.bet_placed = stringToInt(*user_bet_amt);
	delete user_bet_amt;

	blackjack.money_to_bet -= blackjack.bet_placed;
}