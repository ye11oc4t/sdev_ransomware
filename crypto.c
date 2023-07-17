#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

void generate_key(char* key) {
	for (int i = 0; i < 8; i++) {
		key[i] = rand() % 255;
	}
}

int main() {
	unsigned char key[8];
	srand(time(NULL)); 
	generate_key(key); 

	return 0;
}