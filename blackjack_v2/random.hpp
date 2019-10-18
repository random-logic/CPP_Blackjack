#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <ctime>
#include <cstdlib>
using namespace std;

//Used for the random number generator
namespace random {
	void init() {
		//Get the system time
		time_t seed = time(0);

		//Seed the random number generator
		srand(static_cast<int>(seed));
	}

	int getRandomInt(int max) {
		//Generate a random int from 0 to max excluding max
		return rand() % max;
	}
}

#endif