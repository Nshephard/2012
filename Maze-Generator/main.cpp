/*******************************************************************************
* Project: Programming Assignment 4
* Programmer: Nick Shephard
* Date Created: Apr 20, 20011
* Date Last Modified: Apr 29, 2011
* Description: 4th Programming Assignment for CptS 223, Spring 2011. MAIN FUNC
*******************************************************************************/
#include "Room.h"
#include "DisjointSets.h"
#include "Maze.h"

int main()
{
	int x = 0;
	char y = NULL;
	string readfile; //Creating Title Page to determine operation
	cout << "Welcome to Nick Shephard's RAT IN A MAZE!"<<endl;
	cout << "Please Enter 1 to input dimension or 2 to Read Maze From File: ";
	cin >> y;
	
	if (y != '1' && y != '2') //Error Checking
	{
	cout << "INVALID COMMAND DUMMY! GOODBYE" << endl;
	}

	else if (y == '1') //Generate Maze section
	{
		cout << "Please specify the dimension of the maze you wish to create: ";
		cin >> x;
		Maze A (x);
		A.setUpMaze();
		cout << "Initial Maze:" << endl;
		A.print();
		
		//A.createMaze();
		/*while (!A.checkIfMaze())
		{
			A.createMaze();
			system("CLS");
			cout << "Generating Maze." << endl;
			A.print();
		} */
	}
	
	else if (y == '2') //Read Maze from File section
	{
		 
		cout <<"Please enter the exact filename you wish to create the maze from: ";
		cin >> readfile; //Prompt user to input file to read from
	
		ifstream input(readfile.c_str());

		if (input.good()) //While input good
		{
			input >> x; //dimension of maze
			Maze B(x); //create maze
			B.setUpMaze();
			for (int t = 0; t <= ((x*x)-1); t++) //rows
			{
				for (int i = 1; i < 5 ; i++) //each col within row (corresponding to wall value)
				{
					int value;
					input >> value; //Each input is checked for 0 meaning wall is destroyed
					if (i == 1 && value == 0) //Destroy Corresponding walls
					{
						B.DestroyNorth(x,NORTH,t);
					}
					if (i == 2 && value == 0)
					{
						B.DestroySouth(x,SOUTH,t);
					}
					if (i == 3 && value == 0)
					{
						B.DestroyEast(x,EAST,t);
					}
					if (i == 4 && value == 0)
					{
						B.DestroyWest(x,WEST,t);
					}
				}
			}
			cout << "Generated Maze: "<<endl; //Print Result maze
			B.print();
		}
		else
		{
			cout << "Unrecognized filename Dummy! Please restart when you know your stuff!!!" <<endl;
		}
	}

}
