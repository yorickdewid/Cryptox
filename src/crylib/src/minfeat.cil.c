/* minfeat.c - It prompts the user to enter an integer N.
 *             It prints out all the primes up to N included.
 *             It uses a sieve method. As primes are found they are
 *             stored in an array PRIMES and used as possible factors
 *             for the next potential prime.
 */

#include <stdio.h>

#define NPRIMES  1000
#define FALSE 0
#define TRUE  1

void sieve(int n) {
	int flag;
	int level = 0;	/* 1+Number of primes currently in PRIMES */
	int primes[NPRIMES]; /*It will contain the primes smaller than n
                        *that we have already encountered*/

	/* Main body */
	int i,j;
	for (i=2; i<=n; ++i) {
		for (j = 0, flag = TRUE; j<level && flag; ++j) {
			flag = (i % primes[j]);
		}

		/* i is a prime */
		if (flag) {
			printf("%12d\n", i);
			if (level < NPRIMES) {
				primes[level++] = i;
			}
		}
	}
}

int main(void) {
	int n;

	/* Introduction */
	printf("Enter value of N > ");
	scanf("%d", &n);

	sieve(n);
}
