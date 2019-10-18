#ifndef CARD_CLASS_HPP
#define CARD_CLASS_HPP

#include <vector>
#include <string>
#include "random.hpp"
using namespace std;

class Cards {
public:
	//Each card has more than one aspect, so every card should get these unique aspects
	struct Card {
		int pattern; //Store a value to identify the card by pattern
		int number; //Store a value to identify the card by number
	};

private:
	//Holds the card decks
	vector<Card> card_deck;
	vector<Card> cards_disposed;

public:
	//Short hand references
	//To reference back to the patterns of the cards
	static const int HEARTS = 1, DIAMONDS = 2, SPADES = 3, CLUBS = 4;
	//To reference back to the numbers of the cards
	static const int ACE = 1, JACK = 11, QUEEN = 12, KING = 13;

	//Init deck
	void initCardDeck(int decks) {
		//For every parameter deck
		for (int amt_of_decks = 1; amt_of_decks <= decks; amt_of_decks++) {
			//Create a deck of cards
			//For every pattern of the deck
			for (int pattern_name = 1; pattern_name <= 4; pattern_name++) {
				//For every number of the pattern
				for (int number_amt = 1; number_amt <= 13; number_amt++) {
					//Create a card based on a specific pattern and number and add it to the deck
					Card card;
					card.pattern = pattern_name;
					card.number = number_amt;
					card_deck.push_back(card);
				}
			}
		}
	}

	//Push back all cards from a hand (vector) to cards_disposed
	void pushBackDisposeCards(vector<Card> cardsToDispose) {
		for (int i = 0; i < cardsToDispose.size(); i++) {
			cards_disposed.push_back(cardsToDispose[i]);
		}
	}

	//Refills card deck with all of the cards from the cards_disposed vector
	void refillCardDeck() {
		for (int i = 0; i < cards_disposed.size(); i++) {
			card_deck.push_back(cards_disposed[i]);
		}

		//After moving all of the cards back to the cards draw deck, there needs to be no cards left in the cards disposed deck
		cards_disposed.clear();
	}

	//Get and remove a random card from the deck
	Card getRandomCardFromDeck() {
		//If there are no cards present
		if (card_deck.size() == 0) {
			if (cards_disposed.size() > 0)
				//Refill the cards from the cards disposed deck into the draw deck if there are cards present in the cards disposed deck
				refillCardDeck();
			else
				//Return an invalid card
				return Card{ 0, 0 };
		}

		//Select a random int from 0 to max position of the card deck
		int index_chosen = random::getRandomInt(static_cast<int>(card_deck.size()));

		//Based on that position randomly selected, draw and remove that card out of the drawing deck and return it
		Card card_found = card_deck[index_chosen];
		card_deck.erase(card_deck.begin() + index_chosen);
		return card_found;
	}

	//Return the pattern name as a string of the card
	static string getPatternOfCard(Card card) {
		switch (card.pattern)
		{
		case HEARTS:
			return "Hearts";
			break;
		case DIAMONDS:
			return "Diamonds";
			break;
		case SPADES:
			return "Spades";
			break;
		case CLUBS:
			return "Clubs";
			break;
		default:
			return "Invalid Pattern";
			break;
		}
	}

	//Return the correct number as a string of the card
	static string getNumberOfCard(Card card) {
		switch (card.number)
		{
		case ACE:
			return "Ace";
			break;
		case JACK:
			return "Jack";
			break;
		case QUEEN:
			return "Queen";
			break;
		case KING:
			return "King";
			break;
		case 10:
			return "10";
			break;
		default:
			//By default, cast the number of the card as a character and add it to the string
			string str_to_return;
			str_to_return.push_back(static_cast<char>(card.number + static_cast<int>('0')));
			return str_to_return;
			break;
		}
	}
};

#endif