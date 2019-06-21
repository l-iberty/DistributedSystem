#ifndef RANDOM_H
#define RANDOM_H

class Random
{
public:
	Random();
	~Random() = default;
	int Get();
private:
	unsigned int seed_;
};

#endif /* RANDOM_H */
