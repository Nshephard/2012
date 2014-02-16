/*******************************************************************************
* Disjoint Set Header and Functions
* Programmer: Nick Shephard
* Date Created: Apr 20, 20011
* Date Last Modified: Apr 29, 2011
* Description: Header file for disjoint set of rooms, used within the maze class.
*******************************************************************************/
#ifndef _DISJOINTSETS_H_
#define _DISJOINTSETS_H_

#include "Room.h"
#include <vector>
#include <fstream>


class DisjointSets
{
	public:
		DisjointSets(){} //Contructor(s)

        explicit DisjointSets(int numElements): x(numElements)
		{
			for(int index = 0;(unsigned) index < x.size( ); index++)
			{
       			x[index].setName(-1);
			}
		}
	   int ChangeName(int i) //Change name of element in set to group elements
	   {
		return x[i].getName();
	   }

       int find(int i) //find element in set return name
		{
			
			if( x[i].getName() < 0 )
			{
                return i;
			}
       		else
			{
                x[i].setName(find(x[i].getName()));
				return x[i].getName();
			}

		}

        void unionSets(int Set1, int Set2) //Not really union of sets just removing the name from the -1 set list
		{
			int temp= find(Set1);
			int temp2= find(Set2);

			x[temp].setName( temp2 );
		}

		int getSize()const
		{
			return x.size(); //return size
		}

		void setWall(int direction,int index,int newVal)
		{
			x[index].setWall(direction,newVal); //Set Wall value
		}

		bool destroyWall(int direction,int index)
		{
			return x[index].destroyWall(direction); //destroy wall
		}

		void print(int index) //print corresponding index
		{
			x[index].print();
		}

		void getSides(int index,string &south, string &east) //get sides
		{
			x[index].getSides(south,east);
		}

	private:
       vector<Room> x; //a vector of lists disjoint set
};

#endif
