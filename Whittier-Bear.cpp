/********************************************************************
*	Author: 	Jason Hachach
*	Class: 		COSC 3336
*	Due: 		4/30/2018 
*	Purpose:	to learn the  use  of pthreads, pthread mutexes  and  
*				pthread  condition variables
********************************************************************/


#include <iostream>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
void * wmCar (void*);
void * bbCar (void*);
void * tunnel(void*);
struct carInfo
{
	int carNumber;
	string Direction;
	int crossTime;	
};

// Shared variables
string tunnelStatus;
int maxCars;
int numCarsInTunnel =0;
int delayed =0;
int numBear = 0;
int numWhitt = 0;

// Mutex
static pthread_mutex_t traffic_lock = PTHREAD_MUTEX_INITIALIZER;

// Conditon
pthread_cond_t clear = PTHREAD_COND_INITIALIZER;


int main()
{
	int numCars =0;
	carInfo c[50];
	pthread_t tid;
	int d;
	string dir;
	int t;

// Tunnel thread
	pthread_t tid1;
	pthread_create(&tid1, NULL, tunnel, NULL);
	
// Keep track of thread ID's
	pthread_t cartid[100];
	
// Ensure input is always maxCars	
	cin >> maxCars;

// Start input	
	while(cin >>d && cin >> dir && cin >> t)
	{
		c[numCars].carNumber = numCars + 1;
		c[numCars].crossTime = t;
		c[numCars].Direction = dir;

// Sleep for required amount of delay to cross tunnel
		sleep(d);

// Determine which thread to create
		if(dir == "WB")
			pthread_create(&tid,NULL,wmCar,&c[numCars]);
		else
			pthread_create(&tid,NULL,bbCar,&c[numCars]);
			
// Store threadID
		cartid[numCars] = tid;

		numCars++;
	}
	
// Join all threads 
	for(int j = 0; j < numCars; j++)
	{
		pthread_join(cartid[j], NULL);
	}


// Print Summary	
	cout << numBear << " car(s) going to Bear Valley arrived at the tunnel" << endl;
	cout << numWhitt << " car(s) going to Whittier arrived at the tunnel" << endl;
	cout << delayed << " car(s) were delayed" << endl;

	
	return(1);

}

// The tunnel thread that only changes the the tunnel status
void *tunnel (void* arg)
{
	int i = 0;
	
// Ensure infinite loop, will be terminated by main thread
	while(1)
	{
		pthread_mutex_lock(&traffic_lock);
		tunnelStatus = "WB";
		pthread_cond_broadcast(&clear);
		cout << "\nTunnel is now open to Whittier-bound cars." << endl<< endl;
		pthread_mutex_unlock(&traffic_lock);
		
		sleep(5);
					
		pthread_mutex_lock(&traffic_lock);
		tunnelStatus = "closed";
		cout << "\nTunnel is closed to all traffic." << endl<< endl;
		pthread_mutex_unlock(&traffic_lock);
		
		sleep(5);
					
		pthread_mutex_lock(&traffic_lock);
		tunnelStatus = "BB";
		pthread_cond_broadcast(&clear);
		cout << "\nTunnel is now open to Bear Valley-bound cars." << endl<< endl;
		pthread_mutex_unlock(&traffic_lock);
		
		sleep(5);
					
		pthread_mutex_lock(&traffic_lock);
		tunnelStatus = "closed";
		cout << "\nTunnel is now closed to all traffic." << endl << endl;
		pthread_mutex_unlock(&traffic_lock);
		
		sleep(5);
	}

	
}

// Car thread that controls cars goint to Whittier
void *wmCar(void *arg)
{
	int i;
	bool delay = false;
// Convert arg to struct pointer
	carInfo *a = (carInfo*)arg;

// Start Mutex	
	pthread_mutex_lock(&traffic_lock);
	cout <<"Car #" << a->carNumber <<" to Wittier arrives at the tunnel" << endl;
	numWhitt++;
	
// See if car is delayed 	
	while(tunnelStatus != a->Direction || numCarsInTunnel >= maxCars)
	{
		if(numCarsInTunnel >= maxCars && tunnelStatus == a->Direction)	
			delay = true;
	
	
	pthread_cond_wait(&clear, &traffic_lock);
	}

// Increment if true
	if(delay)
		delayed++;

	numCarsInTunnel++;
	cout << "Car #" << a->carNumber << " to Wittier will now enter the tunnel"<< endl;
	
// Unlock mutex
	pthread_mutex_unlock(&traffic_lock);
	
// Sleep for crosstime in the tunnel
	sleep(a->crossTime);
	
// Start second mutex	
	pthread_mutex_lock(&traffic_lock);
	cout << "Car #" << a->carNumber << " to Wittier has exited the tunnel" << endl;
	numCarsInTunnel--;
	
// Broadcast change to all threads
	pthread_cond_broadcast(&clear);

// End mutex
	pthread_mutex_unlock(&traffic_lock);
	pthread_exit((void*)0);

}

// Car thread that controls cars goint to Bear Valley
void *bbCar(void *arg)
{
	int i;
	bool delay = false;
	
// Convert argument to struct pointer
	carInfo *b = (carInfo*)arg;
	
// Start mutex
	pthread_mutex_lock(&traffic_lock);	
	cout <<"Car #" << b->carNumber <<" to Bear Valley arrives at the tunnel" << endl;
	numBear++;
	
// Check for delay condition
	while(tunnelStatus != b->Direction || numCarsInTunnel >= maxCars)
	{
		if(numCarsInTunnel >= maxCars && tunnelStatus == b->Direction)
			delay = true;
			
		pthread_cond_wait(&clear, &traffic_lock);
	}
	
// increment if true	
	if(delay)
		delayed++;

	numCarsInTunnel++;
	cout << "Car #" << b->carNumber << " to Bear Valley will now enter the tunnel"<< endl;
	
	
// End mutex
	pthread_mutex_unlock(&traffic_lock);
	
	sleep(b->crossTime);
// Start mutex again	
	pthread_mutex_lock(&traffic_lock);	
	cout << "Car #" << b->carNumber << " to Bear Valley has exited the tunnel" << endl;
	numCarsInTunnel--;
	
// Broadcast change to all threads
	pthread_cond_broadcast(&clear);	
		
// End mutex		
	pthread_mutex_unlock(&traffic_lock);

	pthread_exit((void*)0);

}

