#include "Random.h"

int Random::rangeRand(int lower, int upper)
{
    srand((unsigned int) time(NULL));
    int rnd = (rand() + (int) clock()) % (upper - lower) + lower;
    return rnd;
}
