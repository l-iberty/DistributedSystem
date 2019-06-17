#include "Random.h"

int Random::rangeRand(int lower, int upper)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    int rnd = (rand() + static_cast<int>(clock())) % (upper - lower) + lower;
    return rnd;
}
