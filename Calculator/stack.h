/*******************************************************************************
* STACK Header and Functions
* Programmer: Nick Shephard
* Date Created: Feb 15, 20011
* Date Last Modified: Feb 21, 2011
* Description: 1st Programming Assignment for CptS 223, Spring 2011.
*******************************************************************************/
#ifndef _STACK_H_
#define _STACK_H_

#include <string.h>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;
using std::string;

template <class T>
class Stack
{
public:
	Stack();
	~Stack();
	bool push(const T item);
	bool pop (T &item);
	bool getTop(T &item);
	bool isEmpty();
	void print();
private:
	struct node{
		T data;
		node *next;
		/* node(const T & d = T(), node* n = 0) : data(d), node(n) { }; */
	};
	node *Top; //Pointer to top of a linked list (struct node)
};
template <class T>
Stack<T>::Stack()
{
	Top = NULL;
}

template <class T>
Stack<T>::~Stack() //Copy'd code written for pop, if the stack isn't empty pop until it is!
{
	while (!isEmpty())
	{
	node *temp = Top; //Temp pointer points to Top of Stack
	Top = Top->next; //Next node is new top node
	delete temp; //Delete old top node
	}
}

template <class T>
bool Stack<T>::push(const T item)
{
	bool status = false;

	node *temp = new node; //Temp pointer to new node
	temp->data = item; //puts item as data in new node
	temp->next = Top; //temp's next pointer points to previous top of stack
	Top = temp; //assign temp to new top of stack

	status = true;
	return status;
}

template <class T>
bool Stack<T>::pop(T &item)
{
	bool status = false;

	node *temp = Top; //Temp pointer points to Top of Stack
	item = Top->data; //Assigns the topmost data element to item
	Top = Top->next; //Next node is new top node
	delete temp; //Delete old top node

	status = true;
	return status;
}

template <class T>
bool Stack<T>::getTop(T &item)
{
	bool status = false;

	item = Top->data; //Assigns the topmost data element to item
	
	status = true;
	return status;
}

template <class T>
bool Stack<T>::isEmpty() //If the stack is empty returns 1, otherwise returns 0
{
	if (Top == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template <class T>
void Stack<T>::print()
{
	node *temp = Top;
	cout << "Contents of Stack: ";
	while (temp != NULL) //iterates through the list until it reaches end
	{
		cout << temp->data << " "; //Prints data in each node followed by a space
		temp = temp->next; //increments 1 node
	}
	cout << "\n";
}


#endif
