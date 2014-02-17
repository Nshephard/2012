/*******************************************************************************
* Project: Programming Assignment 1
* Programmer: Nick Shephard
* Date Created: Feb 15, 20011
* Date Last Modified: Feb 21, 2011
* Description: 1st Programming Assignment for CptS 223, Spring 2011.
*******************************************************************************/

#include "stack.h" //Header + Libraries
#include "queue.h"
#include "functions.h"
#include <stdio.h>
#include <ctype.h>


int main ()
{
	char A[] = "hello, world";
	int len = 0;
	char * P = A;
	while (*P && *P != 'l')
		++P;
	len = P - A;
	cout << len << endl;
}
