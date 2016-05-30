#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/time.h>
#include <math.h>

#define CHAIRS 5

sem_t customers;
sem_t barbers;
pthread_mutex_t mutex;
int waiting = 0;
int if_no_more = 0;

void* barber(void *xxx)
{
	while(1)
	{
		sem_wait( &customers); /* get a customer */

		pthread_mutex_lock( &mutex);
		printf("barber: Begin to cut! ");
		printf("%d customers waiting now\n", --waiting);
		pthread_mutex_unlock( &mutex);

		//cut_hair();
		sleep(rand()%7 + 1);

		sem_post( &barbers); /* tell this customer job done */

		/* when $if_no_more is true, $waiting is not in shared area */
		if ( if_no_more && 0 == waiting)
			break;
	}

	sleep(3); /* wait for the last customer left */
	printf("barber: offtime, Bye!\n");
	return;
}

void* customer(void *xxx)
{
	int color = rand()%6 + 31;
#define START "\033[1;%dm"
#define END "\033[0m\n"
	pthread_mutex_lock( &mutex);

	if( waiting < CHAIRS)
	{
		printf(START"\t%d customers waiting now including me #%u"END, \
				color, ++waiting, (unsigned int)pthread_self());
		pthread_mutex_unlock( &mutex);

		sem_post( &customers); /* stand in queue */
		sem_wait( &barbers); /* wait for the barber's job */

		printf(START"\tcustomer #%u: Nice job, Bye!"END, color, (unsigned int)pthread_self());
	}
	else
	{
		printf("customer #%u: Too many customers, Bye!\n", (unsigned int)pthread_self);
		pthread_mutex_unlock( &mutex);
	}
	return;
}

int main()
{
	int i;
	pthread_t barber_id, customer_id;

	sem_init(&customers, 0, 0);
	sem_init(&barbers, 0, 0);
	pthread_mutex_init( &mutex, NULL);
	srand(time(0));

	pthread_create( &barber_id, NULL, barber, NULL);

	for(i=0; i<15; i++)
	{
		sleep(rand()%4 + 1);
		pthread_create( &customer_id, NULL, customer, NULL);
	}

	if_no_more = 1; /* tell the barber that no more customers */
	pthread_join( barber_id, NULL);

	sem_destroy( &customers);
	sem_destroy( &barbers);
	pthread_mutex_destroy( &mutex);

	return 0;
}
