#ifndef BLACKJACK_HPP
#define BLACKJACK_HPP

#include "card_class.hpp"
using namespace std;

class Blackjack {
private:
	//Holds the card hands
	vector<Cards::Card> dealer_hand;
	vector<vector<Cards::Card>> player_hands;

	//Get cards ready for the game
	Cards cards;

public:
	int bet_placed = 0; //Bet placed for each round
	int money_to_bet = 0; //Money for the user to bet
	int money_to_win = 0; //Goal for the user to beat

	//Possible choices that can be made in blackjack by player
	enum Choice {
		HIT = 1, STAY = 2, SPLIT = 3, QUIT = 4
	};

	//Dealer or player
	enum Hand {
		DEALER = 1, PLAYER = 2, PUSH = 3
	};

	//Card hand sum with sum assuming all aces = 1 and number of ace cards
	struct CardHandSum {
		int hard_sum;
		int num_of_aces;
	};

	static const int MAX_DECKS_ALLOWED = 8; //Maximum amount of decks allowed in the game
	static const int MAX_PLAYER_HANDS_PER_ROUND = 4; //The maximum amount of hands the player can have in a given round
	static const int ACE_SOFT_VALUE = 11; //Ace soft values are 11 in blackjack
	static const int ACE_HARD_VALUE = 1; //Ace hard values are 1 in blackjack
	static const int ACE_DIFFERENCE = ACE_SOFT_VALUE - ACE_HARD_VALUE; //Determine the difference of the two possible ace values
	static const int FACE_CARDS_WORTH = 10; //Determine how much face cards are worth
	static const int MIN_DEALER_HAND_SUM = 17; //Determine the threshold of the minimum soft sum the dealer must have
	static const int BLACK_JACK_VALUE = 21; //Determine how much a blackjack is worth, any sum above this threshold is a bust

	//Init game
	void initGame(int amt_of_decks) {
		cards.initCardDeck(amt_of_decks);
	}

	//Returns whether the hand is eligible to split or not
	bool handCanSplit(int hand_index) {
		if (player_hands[hand_index].size() != 2)
			return false;

		//Get the two values of the cards and adjust the face cards to be worth 10
		int card1 = player_hands[hand_index][0].number,
			card2 = player_hands[hand_index][1].number;
		if (card1 == Cards::JACK || card1 == Cards::QUEEN || card1 == Cards::KING)
			card1 = FACE_CARDS_WORTH;
		if (card2 == Cards::JACK || card2 == Cards::QUEEN || card2 == Cards::KING)
			card2 = FACE_CARDS_WORTH;

		//Return whether the hand can split based on whether the cards are equal to each other and check that the player can still have more hands in the round
		return card1 == card2 && player_hands.size() < MAX_PLAYER_HANDS_PER_ROUND;
	}

	//Take action on whatever the player decided to do
	void takePlayerAction(Choice choice, int hand_index) {
		if (choice == HIT) {
			//Add card to whichever player hand is selected
			player_hands[hand_index].push_back(cards.getRandomCardFromDeck());
		}
		else if (choice == SPLIT && handCanSplit(hand_index)) {
			//Split cards
			vector<Cards::Card> new_hand;
			new_hand.push_back(player_hands[hand_index][1]);
			player_hands[hand_index].pop_back();
			player_hands.push_back(new_hand);
		}
	}

	//Draw new hands for the dealer and player and dispose old hands
	void newRound() {
		//Dispose and clear old cards
		cards.pushBackDisposeCards(dealer_hand);
		dealer_hand.clear();
		for (int i = 0; i < player_hands.size(); i++) {
			cards.pushBackDisposeCards(player_hands[i]);
		}
		player_hands.clear();

		//Draw 2 new cards for the dealer and player
		vector<Cards::Card> player_hand_add;
		player_hands.push_back(player_hand_add);
		player_hands[0].push_back(cards.getRandomCardFromDeck());
		dealer_hand.push_back(cards.getRandomCardFromDeck());
		player_hands[0].push_back(cards.getRandomCardFromDeck());
		dealer_hand.push_back(cards.getRandomCardFromDeck());
	}

	//Read the hand and get the sum of the hand
	//Using a vector since there is multiple possibilities of sums as an ace can be 1 or 11
	CardHandSum getSumOfHand(vector<Cards::Card> card_hand) {
		//Declares and initializes variable to store the sum that should be returned
		CardHandSum sum_to_return;
		sum_to_return.num_of_aces = 0;
		sum_to_return.hard_sum = 0;

		//Add card number value to the sum for each card
		for (int i = 0; i < card_hand.size(); i++) {
			if (card_hand[i].number == Cards::JACK || card_hand[i].number == Cards::QUEEN || card_hand[i].number == Cards::KING)
				sum_to_return.hard_sum += FACE_CARDS_WORTH;
			else if (card_hand[i].number == Cards::ACE) {
				sum_to_return.num_of_aces += 1;
				sum_to_return.hard_sum += ACE_HARD_VALUE;
			}
			else
				sum_to_return.hard_sum += card_hand[i].number;
		}

		return sum_to_return;
	}

private:
	//Decide whether the dealer should keep hitting or not
	bool dealerKeepHitting() {
		CardHandSum card_hand_sum = getSumOfHand(dealer_hand);

		//If hard sum already meets threshold, dealer has to stop drawing
		if (card_hand_sum.hard_sum >= MIN_DEALER_HAND_SUM) {
			return false;
		}

		//See if any of the soft sums from the aces are within MIN_DEALER_HAND_SUM and BLACK_JACK_VALUE
		//If so, soft sum is favorable and casino rules make the dealer not want to hit
		for (int i = 1; i <= card_hand_sum.num_of_aces; i++) {
			int card_hand_soft_sum = card_hand_sum.hard_sum + card_hand_sum.num_of_aces * ACE_DIFFERENCE;
			if (card_hand_soft_sum >= MIN_DEALER_HAND_SUM && card_hand_soft_sum <= BLACK_JACK_VALUE) {
				return false;
			}
		}

		//Happens if hard sum is not above threshold of MIN_DEALER_HAND_SUM and no soft sums are favorable
		return true;
	}

public:
	//Dealer will start hitting until hard or soft sum of >=17 is reached
	//Soft sums will only count if it is <=21
	void dealerTakeTurn() {
		//Keep drawing until dealer sum is >= 17
		while (dealerKeepHitting())
			dealer_hand.push_back(cards.getRandomCardFromDeck());
	}

	//Get the best calculated sum most favorable to the hand
	//For the hands, the best would be to get the highest value 21 and under, busting is the worst
	int getMostFavorableSum(CardHandSum card_hand_sum) {
		int mostFavorableSum = card_hand_sum.hard_sum; //Store the most favorable sum

		//For every ace card, see if the potential soft sum can benefit the player
		for (int i = 1; i <= card_hand_sum.num_of_aces; i++) {
			//Calculate the soft sum for i number of soft aces
			int soft_sum = card_hand_sum.hard_sum + i * ACE_DIFFERENCE;

			//See if the new soft sum will be busted, if not, the soft sum is better
			//No need to test more soft sums of higher values if soft sum did bust
			if (soft_sum <= BLACK_JACK_VALUE) {
				mostFavorableSum = soft_sum;
			}
			else {
				break;
			}
		}

		return mostFavorableSum;
	}

	//Determine the winner of the round after all draws from the deck have been made
	Hand findRoundWinner(int player_hand_index) {

		//Get the sum of the hands
		CardHandSum dealer_hand_sum = getSumOfHand(dealer_hand);
		CardHandSum player_hand_sum = getSumOfHand(player_hands[player_hand_index]);

		//Get the best sums for the dealer and the player
		int best_dealer_sum = getMostFavorableSum(dealer_hand_sum);

		int best_player_sum = getMostFavorableSum(player_hand_sum);

		if ((best_dealer_sum > best_player_sum || best_player_sum > BLACK_JACK_VALUE) && best_dealer_sum <= BLACK_JACK_VALUE) {
			return DEALER;
		}
		else if ((best_player_sum > best_dealer_sum || best_dealer_sum > BLACK_JACK_VALUE) && best_player_sum <= BLACK_JACK_VALUE) {
			return PLAYER;
		}
		else {
			return PUSH;
		}
	}

	//Get all player hands
	vector<vector<Cards::Card>> getPlayerHands() {
		return player_hands;
	}

	//Get the dealer's hand
	vector<Cards::Card> getDealerHand() {
		return dealer_hand;
	}
};

#endif