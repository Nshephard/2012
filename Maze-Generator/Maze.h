/*******************************************************************************
* Maze Class Header and Functions
* Programmer: Nick Shephard
* Date Created: Apr 20, 20011
* Date Last Modified: Apr 29, 2011
* Description: This header encompasses all of the previous classes because it contains
* most major operations relative to the project.
*******************************************************************************/
#ifndef _MAZE_H_
#define _MAZE_H_

#include<iostream>
#include<fstream>
#include<list>
#include<vector>
#include <algorithm>
#include <time.h> //for srand
#include "DisjointSets.h" //my headers
#include "queue.h"
#include "stack.h"

using namespace std;

#define NORTH	1
#define WEST	2
#define SOUTH	3
#define EAST	4

class Maze{
public:
	Maze(); //Constructor(s)
	Maze(int dim):dimension(dim),TotalRooms(dim*dim),DisjointSet(TotalRooms)
	{
		for(int i= 0; i<TotalRooms; i++)
			myList.push_back(i);	
	}

	void ChangeWall(int direction, int index, int input) //Change walls (mostly for read from file)
	{
		DisjointSet.setWall(direction,index,input);
	}

	void setUpMaze() //Setting up maze structure
	{
	int last= (TotalRooms-1);
	//Destroy North wall of START, and South wall of FINISH
	DisjointSet.destroyWall(NORTH, 0);
	DisjointSet.destroyWall(SOUTH,last);

		for(int i=0; i<TotalRooms; i+=dimension) //Each Row
		{
			for(int m=0,k=i; m < dimension; k++,m++) //Each Room
			{
				if(i==0)//Top Edge
				{
					DisjointSet.setWall(NORTH,k,-1);
				}
				if(k%dimension==0 && i!=0)	//Left Edge
				{
					DisjointSet.setWall(WEST,k,-1);
				}
				if(m==(dimension-1) && k!= last) //Right Edge
				{
					DisjointSet.setWall(EAST,k,-1);
				}
			}
		}
		for(int a= last, p=dimension; p>0; p--,a--) //Bottom Edge
		{
			DisjointSet.setWall(SOUTH,a,-1);
		}
	}
	void createMaze()
	{
		int temp,direction,index;
		int size= TotalRooms-1;			//Maximum Cell value for given maze

		list<int>::iterator i;	
		// srand(time(0)); TURN ON FOR PURE RANDOM GENERATION (NO SEED) 
							//Uncommented to avoid massive runtime!! 
		temp = rand() % myList.size();		//Select random index
		direction =	rand() % 4 +1;			//Select random direction

		int j=0;
		for(i= myList.begin() ;(unsigned) j<myList.size(); i++,j++)
			if(j==temp)
			{
				index= *i;
				break; //iterate thru to check
			}	

		if(direction==SOUTH) //Destroy random generated side
			DestroySouth(size,direction,index);

		else if(direction==EAST)
			DestroyEast(size,direction,index);

		else if(direction==NORTH)
			DestroyNorth(size,direction,index);

		else if(direction==WEST)
			DestroyWest(size,direction,index);	
	}
/* ****************UNFINISHED COMMENTED OUT ********************
void BreadthFirstSearch()
{
	Queue<int> x;

	for (int i = 0; i <= TotalRooms-1;i++)
	{
		x.enqueue(i);
		DisjointSet.ChangeName(i);
	}
	DisjointSet.changeName(0); //Setting start to element
	x.dequeue(0); //Dequeue start
	while (DisjointSet.ChangeName(0) != DisjointSet.ChangeName(TotalRooms-1)) //While start name is not connected to finish
	{
		for (t = 0; t <= TotalRooms; t++)
		{
			DisjointSet.getSides(t);
			if (DisjointSet.setWall(EAST,t) != 1) //if there isnt a wall to EAST
			{
				DisjointSet.unionSets(t, t+1); //union set to east
			}
			else if (DisjointSet.setWall(SOUTH,t) != 1) //isnt wall to south
			{
				DisjointSet.unionSets(t, t+dim); //union t + size of maze for element below t
			}
		}
	}
}
 */
/*
void DepthFirstSearch()
{

}*/

	bool checkIfMaze() //To Check if maze check how many have names >0
	{		
		int counter = 0;
		for (int i = 0; i <= TotalRooms-1; i++)
		{	
			if (DisjointSet.ChangeName(i) < 0)
			{
			counter= counter + 1;
			}
		}
		if (counter <= 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void print() //print maze ASCII
	{
		cout<<"  ";					//NORTH open for index 0

		for(int j=1; j<dimension;j++)	//Print Top Edge 
			cout<<" "<<"_";

		cout<<endl<< "|";			//Left Edge index 0

		for(int i= 0; i<TotalRooms; i+=dimension) //Remaining Rooms
		{
			if(i!= 0)
			{
				cout<<endl<<"|";		
			}
			for(int m= 0,k=i; m < dimension; k++,m++)
			{
				DisjointSet.print(k);				
			}
		}
		cout<<endl;
	}



void DestroySouth(int size,int direction, int index)
{
	if((direction==SOUTH) && (index < size-dimension)) //size-dimension = Index at least higher than last row
	{
		if((DisjointSet.find(index)!=DisjointSet.find(index+dimension))&&
				DisjointSet.destroyWall(direction,index)) //Not in same set + wall destroyed
		{	
			DisjointSet.unionSets(index,index+dimension); //Union
		}
	}
}

void DestroyEast(int size,int direction, int index)
{
	if((direction==EAST)&&(index<size)) //Index not finish
	{
		if((DisjointSet.find(index)!=DisjointSet.find(index+1))&&
				DisjointSet.destroyWall(direction,index))//Not in same set + wall destroy
		{
			DisjointSet.unionSets(index,index+1); //Union
		}
	}
}

void DestroyNorth(int size,int direction, int index)
{
	if((direction== NORTH) && (index >= dimension)) //Index must be in 2nd row
	{
		if((DisjointSet.find(index)!=DisjointSet.find(index-dimension))&&
				DisjointSet.destroyWall(SOUTH,index-dimension))//Not in same set + wall destroyed
		{
			DisjointSet.unionSets(index,index-dimension); //Union
		}
	}
}

void DestroyWest(int size, int direction, int index)
{
	if((direction== WEST) && (index > 0)) //Index not 0
	{
		if((DisjointSet.find(index)!=DisjointSet.find(index-1))&&
				DisjointSet.destroyWall(EAST,index-1))//Not in same set + wall destroyed
		{
			DisjointSet.unionSets(index,index-1); //union
		}
	}
}

private:
int dimension;				//n = dimension of maze
int TotalRooms;				//Total number of rooms in maze
DisjointSets DisjointSet;	//Disjoint set for maze
list<int> myList;			//List of all rooms not already in zero set

};

#endif
