//car object balls and links
class Car
{
public:
	//define the car
	int* balls; //array of the balls that make up the car
	int* balls_x; //x coordinate of each ball
	int* balls_y; //y coordinate of each ball

	//just for car race
	int* linkslist; //array of links that make up the car
	int** links; //matrix of which ball is connected to which ball

        int nodes; //how many balls in the car
        int numlinks; //how many links in the car
	int iterations; //race results: iterations=how many frames
	int position; //race result: how far it got

	Car(int); //constructor (# of nodes)
	void constructCar(); //given ballsx ballsy and links, actually make balls and links for car
	void deconstructCar(); //clears the car out of the race track
	
	int getCarPosition(); //tells us how good the car was
	void score(int,int); //sets iterations and positions at the end of the race

	Car* breed(Car*); //haven't made: breeds the car with another car, returning a new car
	Car* mutate(); //haven't made: makes a new mutant car

	//destruct the car on a delete car (type: delete car)
	~Car()
	{
		delete balls_x;
		delete balls_y;
		for(int i=0; i<nodes; i++)
			delete links[i];
		delete links;
	}
};
