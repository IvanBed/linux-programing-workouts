#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float get_random_real(const float min, const float max) {
	srand(time(NULL));
	int rnd = rand();
	printf("rand %d\n", rnd);
	return (float) rnd / RAND_MAX * (max - min) + min;
}