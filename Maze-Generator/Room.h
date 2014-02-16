/*******************************************************************************
* Room Class Header and Functions
* Programmer: Nick Shephard
* Date Created: Apr 20, 20011
* Date Last Modified: Apr 29, 2011
* Description: Header file for each room, gives each room a wall value for each
* direction as well as a string value for printing.
*******************************************************************************/
#ifndef _ROOM_H_
#define _ROOM_H_

#include<iostream>
#include<fstream>
#include<string>

using namespace std;

#define NORTH		1
#define WEST		2
#define SOUTH		3
#define EAST		4

class Room{
public:
	//Constructor
	Room():North(NORTH),West(WEST),South(SOUTH),East(EAST),
	PrintNorth("_"),PrintSouth("_"),PrintWest("|"),PrintEast("|"),name(-1){}

	bool destroyWall(int direction) //Destroys wall based input direction
	{
		bool status = false;
		switch(direction){
		case NORTH:
			if(North==0||North==-1)
			{
				status= false;
			}
			else
			{
				North= 0;
				PrintNorth= " ";
				status= true;
			}
			break;

		case WEST:
			if(West==0||West==-1)
			{
				status= false;
			}
			else
			{
				West= 0;
				PrintWest= " ";
				status= true;
			}
			break;

		case SOUTH:
			if(South==0 || South==-1)
			{
				status=false;
			}
			else
			{
				South= 0;
				PrintSouth= " ";
				status= true;
			}
			break;

		case EAST:
			if(East==0 || East==-1)
			{
				status= false;
			}
			else
			{
				East= 0;
				PrintEast= " ";
				status= true;
			}
			break;
		}

		return status;
	}

	void setName(int newName)
	{
	name= newName;
	} //sets the name of room

	void setWall(int direction, int newVal) //Sets wall value to new input value
	{
		switch(direction){
		case NORTH:
			North= newVal;
			break;

		case WEST:
			West= newVal;
			break;

		case SOUTH:
			South= newVal;
			break;

		case EAST:
			East= newVal;
			break;

		}
	} //for reading from file + setting edges

	int getName()const
	{
	return name;
	}

	void print () //Prints South and East sides
	{
		cout<< PrintSouth << PrintEast;
	}

	void getSides(string &x, string &y)
	{
		x= PrintSouth;
		y= PrintEast;
	}

	bool operator <(const Room &rhs)const //operator overloads
	{
	return (name < rhs.name);	
	}

	bool operator ==(const Room &rhs)const
	{
	return (name == rhs.name);
	}

	Room& operator =(const Room &rhs)
	{
		if(this != &rhs)
		{
			name= rhs.name;
		}
		return *this;	
	}

private:
	int name; //Name to quantify root

	int North,South,West,East;	
		//side values to determine walls

	string PrintNorth,PrintSouth,PrintWest,PrintEast;
		//string value of the cell walls in order to print 

};

#endif

