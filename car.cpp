#include "car.h"
#include "physicsengine.h"
#include "ball.h"
#include <QtGui>

//random number (a,b)
int randint(int a, int b)
{
	return (qrand()%(b-a+1))+a;
}

//TODO
Car* Car::mutate()
{
	Car* newcar=new Car(nodes);
	return newcar;
}

//TODO
Car* Car::breed(Car* c)
{
	Car* newcar=new Car(nodes);
	return newcar;
}

//makes a random car
Car::Car(int n)
{
	//make all the arrays i need
	nodes=n;
	balls_x=new int[nodes];
	balls_y=new int[nodes];
	balls=new int[nodes];
	links=new int*[nodes];
	for(int i=0; i<nodes; i++) links[i]=new int[nodes];
	numlinks=0;
	linkslist=new int[nodes*nodes];
	
	//make random ball locations on screen from 5 to 50
	for(int i=0; i<nodes; i++)
	{
		balls_x[i]=randint(5,50);
		balls_y[i]=randint(5,50);
	}
	//making links, 1/3 probablity link between any 2 balls
	for(int i=0; i<nodes; i++)
	{
		for(int j=0; j<nodes; j++)
		{
			links[i][j]=0;
			if(i<=j) continue;
			if(randint(1,3)==1) // 1/3
			{
				links[i][j]=1;
			}
		}
	}
}
//graphically construct the car so we can race it
void Car::constructCar()
{
	balls=new int[nodes];
	numlinks=0;
	linkslist=new int[nodes*nodes];
	
	for(int i=0; i<nodes; i++)
	{
		balls[i]=makeBall(balls_x[i],balls_y[i]);
	}
	for(int i=0; i<nodes; i++)
	{
		for(int j=0; j<nodes; j++)
		{
			if(links[i][j])
			{
				linkslist[numlinks++]=makeLink(i,j);
			}
		}
	}
}

//graphically destroy the car so we can race someone else
void Car::deconstructCar()
{
	for(int i=0; i<numlinks; i++)
		removeLink(linkslist[i]-i);
	for(int i=0; i<nodes; i++)
		removeBall(balls[i]-i);

	delete balls;
	delete linkslist;
}

//determine where car is at end of race (average)
int Car::getCarPosition()
{
	int sum=0;
	for (int i=0; i<nodes; i++)
		sum+=getBall(i)->position->x;
	return sum / nodes;
}

//save the score
void Car::score(int itr, int pos)
{
	iterations=itr;
	position=pos;
}
