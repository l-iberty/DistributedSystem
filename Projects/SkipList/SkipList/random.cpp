#include <stdlib.h>
#include <time.h>
#include "random.h"

Random::Random() {}

int Random::Get()
{
	unsigned int a = static_cast<unsigned int>(clock());
	unsigned int b = static_cast<unsigned int>(time(nullptr));
	seed_ += static_cast<int>(a * b);
	srand(seed_);
	return rand();
}