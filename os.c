#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>
#include <signal.h>


struct bridge {
	int north_waiting;
	int north_crossing;
	int north_consecutive;
	int south_waiting;
	int south_crossing;
	int south_consecutive;
    pthread_mutex_t lock;
	pthread_cond_t northbound_done;
	pthread_cond_t southbound_done;
};

void bridge_init(struct bridge *b)
{
	b->north_waiting = 0;
	b->north_crossing = 0;;
	b->north_consecutive = 0;
	b->south_waiting = 0;
	b->south_crossing = 0;
	b->south_consecutive = 0;
	pthread_mutex_init(&b->lock, NULL);
	pthread_cond_init(&b->northbound_done,&b->lock);
	pthread_cond_init(&b->southbound_done,&b->lock);
}
void *bridge_arrive_north(void * br)
{
	struct bridge * b = (struct bridge *) br;
    printf("NORTH car has arrived\n");
    pthread_mutex_lock(&b->lock);
    b->north_waiting++;
	while ((b->south_crossing > 0) || ((b->south_waiting > 0) && (b->north_consecutive >= 5))) {
		pthread_cond_wait(&b->southbound_done, &b->lock);
	}
    printf("NORTH car is on bridge\n");
	b->north_waiting--;
	b->north_crossing++;
	b->north_consecutive++;
	b->south_consecutive = 0;
	pthread_mutex_unlock(&b->lock);
	// bridge_leave_north(b);
}
void bridge_leave_north(struct bridge *b)
{
	pthread_mutex_lock(&b->lock);
	b->north_crossing--;
	if (b->north_crossing == 0) {
		pthread_cond_broadcast(&b->northbound_done);
	}
	pthread_mutex_unlock(&b->lock);
    printf("NORTH car has left the bridge\n");
}
void *bridge_arrive_south(void * br)
{
	struct bridge * b = (struct bridge *) br;
    printf("SOUTH car has arrived\n");
	pthread_mutex_lock(&b->lock);
	b->south_waiting++;
	while ((b->north_crossing > 0) || ((b->north_waiting > 0) && (b->south_consecutive >= 5))) {
		pthread_cond_wait(&b->northbound_done, &b->lock);
	}
    printf("SOUTH car is on bridge\n");
	b->south_waiting--;
	b->south_crossing++;
	b->south_consecutive++;
	b->north_consecutive = 0;
	pthread_mutex_unlock(&b->lock);
      // bridge_leave_south(b);
}
void bridge_leave_south(struct bridge *b)
{
	pthread_mutex_lock(&b->lock);
	b->south_crossing--;
	if (b->south_crossing == 0) {
		pthread_cond_broadcast(&b->southbound_done);
	}
	pthread_mutex_unlock(&b->lock);
    printf("SOUTH car has left the bridge\n");
}

int main() {
	struct bridge b;
	pthread_t tid[20];
	pthread_create(&tid[0],NULL,bridge_arrive_north,(void *)&b);
	pthread_create(&tid[1],NULL,bridge_arrive_north,(void *)&b);
	pthread_create(&tid[2],NULL,bridge_arrive_north,(void *)&b);
	pthread_create(&tid[3],NULL,bridge_arrive_south,(void *)&b);
	pthread_create(&tid[4],NULL,bridge_arrive_south,(void *)&b);
	pthread_create(&tid[5],NULL,bridge_arrive_north,(void *)&b);
	pthread_create(&tid[6],NULL,bridge_arrive_north,(void *)&b);
	pthread_create(&tid[7],NULL,bridge_arrive_north,(void *)&b);
	for(int i=0;i<8;i++){
		pthread_join(tid[i],NULL);
		if(i!=5 && i!=6)
		pthread_create(&tid[i+10],NULL,bridge_leave_north,(void *)&b);
		else
		pthread_create(&tid[i+10],NULL,bridge_leave_south,(void *)&b);
	}
	return 0;
}
