//GENETIC CARS
//

//BY MONDAY: read and comment code
//WORK ON CAR.CPP TODO ALSO

//Template: Michael Black, 2014
//
//main.cpp does the following in order:
//	1) creates a population of cars
//	2) creates a racecourse of "walls"
//	3) genetically improves the cars by racing them
//	4) creates the window -> "races" the cars before graphically showing them
//	5) shows the winning cars

#include <iostream>
#include <QtGui>
#include <math.h>
#include "main.h"
#include "physicsengine.h"
#include "car.h"

using namespace std;

const int GENERATIONS=20;		//how many breeding generations
const int MAXCARS=1000;			//maximum # of cars.  more than this will segfault
const int KILLMAX=20;			//kill all but this many cars
const int INITIAL_POPULATION=30;	//how many cars we start with
const int SIMULATION_LENGTH=2000;	//how long you let a car run before giving up and removing from track
const int CAR_BALLS=10;			//number of balls used to make a car
const int BREEDRATE=20;			//% chance two random cars breed
const int MUTATERATE=50;		//% chance a car mutates
const int MUTATECHANCE=10;		//% chance an attribute is mutated in a mutant car

int WIDTH=500,HEIGHT=500;		//screen width and height(adjust to make more room for tracks)
QGraphicsScene* thescene;		//window component
WindowView* view;			//window
int timeunit=1000/660;			//when we're actually showing the car, do a frame every this many milliseconds (make smaller for slower and larger for faster display)

Car* car[MAXCARS];			//cars are stored in this array
int CarCount=INITIAL_POPULATION;	//keeps track of how many valid cars there are
int currentCar;				//which car we're currently racing

int iterations;				//how many frames we have simulated so far

extern bool simulating;			//TRUE if actively racing a car, FALSE during setup
extern bool dontdographics;		//TRUE if no window is open, FALSE if we want to see the cars in the window
int generation=0; 			//ADDED FOR TESTING STEP 5
int coursecount=0;			//keeps track of course iterations
int track=0;				//the current track being used

//sets up a timer for visualization
TimerHandler::TimerHandler(int t)
{
	QTimer* timer=new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer->start(t);
}

//when a timer goes off, ready to show a frame
//this only happens where we're ready to see the winning cars
void TimerHandler::onTimer()
{
	if(!simulating) return;

	doFrame(); //moves everything by one frame

	int pos=car[currentCar]->getCarPosition();

	iterations++;

	if(iterations>=SIMULATION_LENGTH || pos>=WIDTH)
	{
		//cut off car at end of screen or timeout
		qDebug() << iterations<<" iterations, position="<<pos<<endl;
		car[currentCar]->score(iterations,pos);
		car[currentCar]->deconstructCar(); //remove car

		//proceed to next one
		currentCar++;
		if(currentCar>=CarCount) //if we're all done, stop racing cars
		{
			simulating=FALSE;
			for (int i=0; i<CarCount; i++)
			{
				qDebug()<<"Car "<<i<<": itr: "<<car[i]->iterations<<", pos: "<<car[i]->position<<endl;
			}
		}
		else //if not done, make the next car
		{
			car[currentCar]->constructCar();
		}
		iterations=0;
	}
}

WindowView::WindowView(QGraphicsScene *scene, QWidget* parent):QGraphicsView(scene, parent) { }

//if a car is racing, pause by right clicking
void WindowView::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::RightButton)
	{
		simulating=!simulating; return;
	}
}

//if no cars are racing start them by double clicking
void WindowView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		currentCar=0;
		car[currentCar]->constructCar();
		simulating=TRUE;
	}
}

//TODO: sort the cars by scoring mechanism and kill off all but the KILLMAX best
//	1)car that gets the furthest wins, average distance ball position in car
//	2)if car distance is bigger than end of track, the lowest time (iterations)
void kill()
{
	for(int i=0; i<CarCount; i++) //bubble sort by position, then iterations
	{
		for(int j=0; j<CarCount-1; j++)
		{
			if(car[j+1]->position > car[j]->position)
			{
				Car* temp = car[j+1];
				car[j+1] = car[j];
				car[j] = temp;
			}

			if(car[j+1]->iterations < car[j]->iterations)
			{
				Car* temp = car[j+1];
				car[j+1] = car[j];
				car[j] = temp;
			}
		}
	}

	for(int i=0; i<CarCount; i++) //kill all cars but the survivors
	{
		if(i>=KILLMAX)
		{
//			cout << "car deleted" << endl;			
			delete car[i];
		}
	}

	CarCount=KILLMAX;
	for(int i=0; i<CarCount; i++) //list the survivors
		cout << "Car " << i << "- Pos: " << car[i]->position << " Itr: " << car[i]->iterations << endl;
	//might be nice to say "delete car" on the ones that are killed
	//use "delete car" only after sure everything else is working
}

//TODO: consider every pair of cars, roll random, if decide to breed
void breed()
{
	int breedpopulation = CarCount;
	for(int i=0; i<breedpopulation; i++)
	{
		for(int j=0; j<breedpopulation; j++)
		{
			if(i==j) //make sure cars don't breed with themselves
				continue;

			if(rand()%100>=BREEDRATE) //only a % of cars will breed with another car
				continue;

			int ballcrossover = rand()%CAR_BALLS; //find the crossover for ball position
			int linkcrossover = rand()%(CAR_BALLS*CAR_BALLS); //find the crossover for links

			Car* child = new Car(CAR_BALLS); //create the child car

			for(int k=0; k<ballcrossover; k++) //change first half of car to one parent up to crossover
			{
				child->balls_x[k] = car[i]->balls_x[k];
				child->balls_y[k] = car[i]->balls_y[k];
			}

			for(int k=ballcrossover; k<CAR_BALLS; k++) //change other half to the other parent
			{
				child->balls_x[k] = car[j]->balls_x[k];
				child->balls_y[k] = car[j]->balls_y[k];
			}

			for(int k=0; k<linkcrossover; k++) //change the links to one parent
			{
				child->links[(k/10)%10][k%10] = car[i]->links[(k/10)%10][k%10];
			}

			for(int k=linkcrossover; k<(CAR_BALLS*CAR_BALLS); k++) //change the other links to the other
			{
				child->links[(k/10)%10][k%10] = car[i]->links[(k/10)%10][k%10];
			}

			car[CarCount++] = child; //add the child to the array of cars to race
		}
	}
//car[i]->breed(car[j]), add that to our list car  [CarCount] = ..., CarCount++
}

//TODO: consider every car, roll random, clone if we decide to
void mutate()
{
	int mutatepopulation = CarCount;
	for(int i=0; i<mutatepopulation; i++)
	{
		if(rand()%100 < MUTATERATE) //the chance to mutate a car
		{
			Car* mutant = new Car(CAR_BALLS); //create the mutant car

			for(int j=0; j<CAR_BALLS; j++) //clone the mutant from the car it spawned from
			{
				mutant->balls_x[j] = car[i]->balls_x[j];
				mutant->balls_y[j] = car[i]->balls_y[j];
			}

			for(int j=0; j<CAR_BALLS*CAR_BALLS; j++) //clone the links too
			{
				mutant->links[(j/10)%10][j%10] = car[i]->links[(j/10)%10][j%10];
			}

			for(int j=0; j<CAR_BALLS; j++)
			{
				if(rand()%100 < MUTATECHANCE) //the chance per element to get mutated in the mutant
				{
					mutant->balls_x[j] = rand()%45+5; //reposition the balls' starting positions
					mutant->balls_y[j] = rand()%45+5; //from 5-50
				}
			}

			for(int j=0; j<CAR_BALLS*CAR_BALLS; j++)
			{
				if(rand()%100 < MUTATECHANCE)
				{
					if((j/10)%10<j%10)
						mutant->links[(j/10)%10][j%10] = !mutant->links[(j/10)%10][j%10]; //switch the state of a link
				}
			}

			car[CarCount++] = mutant; //add the mutant to the array of cars to race
		}
	}
//car[i]->mutate(), add that to our list
}

//docars does all the racing, breeding...behind the scenes
//when called, cars are random
//when ended, all genetically superior cars are made, ready to display winners
void doCars()
{
		generation++; //ADDED FOR TESTING STEP 5
		qDebug() << "****** GENERATION "<<generation<<" **********" <<endl;

	//for every car, race it and get its score
	for (int i=0; i<CarCount; i++)
	{
		//construct it
		//put the car into the physics engine
		car[i]->constructCar();

		//tell the physics engine it's allowed to simulate now
		simulating=TRUE;
		int t=0;	//frame count
		int pos;	//how far the car has gotten
		for(t=0; t<SIMULATION_LENGTH; t++) //runs some max amount of frames and give up if car doesnt finish
		{
			doFrame(); //move everything by one epsilon
			pos=car[i]->getCarPosition(); //get car position and save it
			if(pos>=WIDTH) break; //stops simulating if car finishes track
		}

//		qDebug() << t<<" iterations, position="<<pos<<endl; //print out what happened
		//pass to the car scoring function: how long it ran and how far it got		
		car[i]->score(t,pos);
		simulating=FALSE; //stop simulation
		car[i]->deconstructCar(); //yank car off track
	}
//	for (int i=0; i<CarCount; i++) //debugging loop: print out all the cars' scores
//	{
//		qDebug()<<"Car "<<i<<": itr: "<<car[i]->iterations<<", pos: "<<car[i]->position<<endl;
//	}

	kill(); //call kill function
	breed(); //call breed function
	mutate(); //call mutate function
}

//race course is stored here
//make sure walls are linked, trends downhill, and no sharp angles
//for step 5: make random race courses OR make 10 and alternate between them randomly
extern Wall* walls[];
extern int WallCount;
void makeRaceCourse()
{
	if(coursecount==0)
		track=rand()%3;
	coursecount++;
	if(coursecount==4)
		coursecount=0;
//	track=0;
//	if((generation+1)==GENERATIONS)
//		track=0;
	cout << endl << "Track Number: " << track << endl;

	if(track==0)
	{
		WallCount=9;

		walls[0]=new Wall(1,500,499,500);
		walls[1]=new Wall(-20,132,123,285);
		walls[2]=new Wall(104,285,203,277);
		walls[3]=new Wall(202,275,271,344);
		walls[4]=new Wall(271,344,320,344);
		walls[5]=new Wall(321,345,354,318);
		walls[6]=new Wall(354,318,394,324);
		walls[7]=new Wall(394,324,429,390);
		walls[8]=new Wall(429,391,498,401);
	}
/*
	else if(track==1)
	{
		WallCount=9;

		walls[0]=new Wall(1,500,499,500);
		walls[1]=new Wall(-20,132,100,250);
		walls[2]=new Wall(100,250,200,235);
		walls[3]=new Wall(200,235,275,360);
		walls[4]=new Wall(275,360,325,360);
		walls[5]=new Wall(325,360,380,320);
		walls[6]=new Wall(380,320,415,335);
		walls[7]=new Wall(415,335,429,390);
		walls[8]=new Wall(429,390,498,401);
	}

	else if(track==2)
	{
		WallCount=9;

		walls[0]=new Wall(1,500,499,500);
		walls[1]=new Wall(-20,132,100,250);
		walls[2]=new Wall(100,250,170,240);
		walls[3]=new Wall(170,240,300,375);
		walls[4]=new Wall(300,375,350,375);
		walls[5]=new Wall(350,375,400,335);
		walls[6]=new Wall(400,335,430,350);
		walls[7]=new Wall(430,350,460,380);
		walls[8]=new Wall(460,380,498,415);
	}*/

	else if(track==1)
	{
		WallCount=11;

		walls[0]=new Wall(1,500,499,500);
		walls[1]=new Wall(-20,75,100,250);
		walls[2]=new Wall(100,250,150,240);
		walls[3]=new Wall(150,240,175,250);
		walls[4]=new Wall(175,250,225,300);
		walls[5]=new Wall(225,300,250,310);
		walls[6]=new Wall(250,310,300,310);
		walls[7]=new Wall(300,310,340,285);
		walls[8]=new Wall(340,285,375,295);
		walls[9]=new Wall(375,295,450,400);
		walls[10]=new Wall(450,400,498,420);
	}

	else if(track==2)
	{
		WallCount=9;

		walls[0]=new Wall(1,500,499,500);
		walls[1]=new Wall(-20,75,125,225);
		walls[2]=new Wall(125,225,175,250);
		walls[3]=new Wall(175,250,225,240);
		walls[4]=new Wall(225,240,300,300);
		walls[5]=new Wall(300,300,340,300);
		walls[6]=new Wall(340,300,370,280);
		walls[7]=new Wall(370,280,425,315);
		walls[8]=new Wall(425,315,498,350);
	}

	else if(track==3)
	{
		WallCount=9;

		walls[0]=new Wall(1,500,499,500);
		walls[1]=new Wall(-20,60,100,150);
		walls[2]=new Wall(100,150,150,170);
		walls[3]=new Wall(150,170,200,155);
		walls[4]=new Wall(200,155,250,180);
		walls[5]=new Wall(250,180,300,250);
		walls[6]=new Wall(300,250,350,250);
		walls[7]=new Wall(350,250,400,235);
		walls[8]=new Wall(400,235,498,290);
	}

	else if(track==4)
	{
		WallCount=11;

		walls[0]=new Wall(1,500,499,500);
		walls[1]=new Wall(-20,75,75,250);
		walls[2]=new Wall(75,250,120,300);
		walls[3]=new Wall(120,300,150,310);
		walls[4]=new Wall(150,310,180,300);
		walls[5]=new Wall(180,300,230,300);
		walls[6]=new Wall(230,300,260,280);
		walls[7]=new Wall(260,280,300,290);
		walls[8]=new Wall(300,290,370,350);
		walls[9]=new Wall(370,350,415,350);
		walls[10]=new Wall(415,350,498,400);
	}

	for(int i=0; i<WallCount; i++)
			thescene->addItem(walls[i]);

		/*WallCount=9;

		walls[0]=new Wall(1,500,499,500);
		walls[1]=new Wall(-20,132,123,285);
		walls[2]=new Wall(104,285,203,277);
		walls[3]=new Wall(202,275,271,344);
		walls[4]=new Wall(271,344,320,344);
		walls[5]=new Wall(321,345,354,318);
		walls[6]=new Wall(354,318,394,324);
		walls[7]=new Wall(394,324,429,390);
		walls[8]=new Wall(429,391,498,401);

		for(int i=0; i<WallCount; i++)
			thescene->addItem(walls[i]);*/
}

//program starts here
int main(int argc, char **argv)
{
	//seed random number generator
	QApplication app(argc,argv);
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	
	//make the window
	thescene=new QGraphicsScene();
	thescene->setSceneRect(0,0,WIDTH,HEIGHT);

	//make the initial population of cars
	for (int i=0; i<CarCount; i++)
		car[i]=new Car(CAR_BALLS);

	//racing the cars and breeding them
	dontdographics=TRUE; //no window, so don't make the cars visible

	//do the whole genetic process GENERATIONS times
	for(int gen=0; gen<GENERATIONS; gen++)
	{
		makeRaceCourse(); //make the race course
		doCars();
	}

	dontdographics=FALSE; //enable graphics, so when cars are constructed they appear on screen

	//time to display the winners
	//set up the first car to run
	currentCar=0;
	car[currentCar]->constructCar();
	simulating=TRUE;

	//make the window
	view=new WindowView(thescene);
	view->setWindowTitle("Genetic Cars");
	view->resize(WIDTH+50,HEIGHT+50);
	view->show();
	view->setMouseTracking(true);

	//start the timer
	TimerHandler timer(timeunit);

	return app.exec();
}
