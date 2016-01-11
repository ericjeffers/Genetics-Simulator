#include <stdlib.h>
#include <iostream>
using namespace std;

const int BOXNUM = 50;
const int SURVIVORS = 20;
const int INITIAL_POPULATION = 40;
const int BREEDRATE = 2; //2 out of 10
const int GENERATION = 100;
const int DOMUTATERATE = 5; //5 out of 100
const int MUTATIONRATE = 3; //3 out of 100

class Box
{
public:
	int value;
	int weight;
	Box();
};

class DNA
{
public:
	bool boxes[50];

	DNA();
	int getValue();
	int getWeight();
};

Box* box[BOXNUM];	//array of boxes

DNA* dna[2*SURVIVORS*(SURVIVORS+1)];	//population
int population=0;	//population: how many dnas

Box::Box()
{
	//STEP 0
	//value random 1 to 100
	value=rand()%100+1;
	//weight random  1 to 50
	weight=rand()%50+1;
}

DNA::DNA()
{
	//for i in 50
		//boxes[i] = random true or false
	for(int i=0; i<BOXNUM; i++)
	{
		boxes[i]= rand()%2==1;
	}
}

int DNA::getWeight()
{
	//sum up the weights & return it
	int sum=0;
	for (int i=0; i<BOXNUM; i++)
		if(boxes[i])
			sum+=box[i]->weight;
	return sum;
}

int DNA::getValue()
{
	//sum up the values & return it
	int sum=0;
	for (int i=0; i<BOXNUM; i++)
		if(boxes[i])
			sum+=box[i]->value;
	return sum;
}

main()
{
	//STEP 0
	//make my boxes
	//for i in 50, new Box()
	for (int i=0; i<BOXNUM; i++)
	{
		box[i]=new Box();
		cout<<"Box "<<i<<": w="<<box[i]->weight<<", v="<<box[i]->value<<endl;
	}

	//verify by print all the boxes and weights

	//while our population is less than the initial population 
	//	dna[i]=new DNA()
	// 	only add it if the weight below 1000

	while(population < INITIAL_POPULATION)
	{
		DNA* d = new DNA();
		if(d->getWeight()<=1000)
		{
			dna[population]=d;
			population++;
		}
	}



	//for loop # of generations
	//	figure out who the best is and print out its dna/weight/value
	//	kill

		//bubble sort by value

		for(int i=0; i<population; i++)
		{
			for (int j=0; j<population-1; j++)
			{
				if(dna[j+1]->getValue() > dna[j]->getValue())
				{
					DNA* temp = dna[j+1];
					dna[j+1]=dna[j];
					dna[j]=temp;
				}
			}
		}
		population=SURVIVORS;
		//set population = SURVIVORS
	cout << "DNA 0 has "<< dna[0]->getWeight() << " "<<dna[0]->getValue()<<endl;
	cout << "DNA 1 has "<< dna[1]->getWeight() << " "<<dna[1]->getValue()<<endl;

	//	breed
	int breedingpopulation=population; //go through every possible pair
	for(int i=0; i<breedingpopulation; i++)
	{
		for(int mom=0; mom<breedingpopualtion; mom++)
		{
			if(mom==dad) //can't breed with self
				continue;
	





			DNA *baby = new DNA(); //copy over from dad

			for(int k=0; k<crossoverpoint; k++) //copy over from mom
			{
				baby->boxes[k]=dna[dad]->boxes[k];
			}

	
			for(int k=crossoverpoint; k>BOXDNA; k++) //keep the baby
			{
				baby->boxes[k]=dna[mom]->boxes[k];
			}

			if (baby->getweight()>1000) //1000 garbage collect the baby
				continue;

			dna[population++]=baby;

			cout << "daddy is " << dna[dad]->getValue() << ", mommy is " << dna[mom]->getValue << "baby is " << dna[population] << endl;
		}
	}
	
	//	mutate
	int cloningpopulation = population;
	for(int i=0; i<cloningpopulation; i++)
	{
		if(rand()%100 < DOMUTATERATE)
		{
			DNA* mutant = new DNA();
			for(int j=0; j<BOXNUM; j++)
			{
				mutant->boxes[j] = dna[i]->boxes[j]
			}
			cout << "Making a clone: parent: " << dna[i]->getValue() << " spawn: " >> mutant->getValue() << endl;
			
			for(int j=0; j<BOXNUM; j++)
			{
				if (rand()%100 < MUTATIONRATE)
				{
					mutant->boxes[j] = !mutant->boxes[j];
				{
			}

			cout << "spawn: " << mutant->getValue() << endl;
			if(mutant->getWeight()<=1000)
				dna[population++]=mutant;

		}
	}


























}
