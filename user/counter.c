#include "my_io.h"
#include "syscalls.h"

volatile char c;
volatile int i;

void counter(int *id_p);

void new_process(char *cp) {
	c = cp[0];
	i = ((int) c)/7;
	int id = 3;
	sys_create_thread(counter, &id, 4 ,0);
	id = 2;
	sys_create_thread(counter, &id, 4 ,0);
	id = 1;
	counter(&id);
}

void counter(int *id_p) {
	int id = id_p[0];
	int j = 0;
	while (i > 1) {
		i--;
		j++;
		printf("%c%i: %i (%i)\n", c, id, i, j);
		//printf("%x adresse von c\n", &c, c);
		if (c >= 'A' && c <= 'Z') 
			sys_sleep_thread(10);
		else 
			for (volatile int k = 0x1FFFF; k > 0; k--);
	}
	printf("%c%i fertig\n", c, id);

}

