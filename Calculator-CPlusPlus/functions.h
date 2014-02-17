/*******************************************************************************
* Misc functions: IsOperator, ISP, ICP, and calculate
* Programmer: Nick Shephard
* Date Created: Feb 15, 20011
* Date Last Modified: Feb 21, 2011
* Description: 1st Programming Assignment for CptS 223, Spring 2011.
*******************************************************************************/
#include <math.h>
#include "queue.h"
#include "stack.h"

bool isOperator(char operator1) //Checks if input is operator
 {
         if ((operator1 == '+')||(operator1 == '-')||(operator1 =='*')||(operator1 == '^')||(operator1 =='(')||(operator1 ==')')||(operator1 =='/'))
                 return 1;
         else
                 return 0;
 } 

int ISP(char operator1) //In-stack priority
 {
         if ((operator1 == '+')||(operator1 == '-'))
                 return 1;
         else if ((operator1 == '*')||(operator1 == '/'))
                 return 2;
         else if (operator1 == '^')
                 return 3;
         else if (operator1 == '(')
                 return 0;
		 else
				 return -1;
 }

  int ICP(char operator1)  //In-coming-priority
 {
         if ((operator1 == '+')||(operator1 == '-'))
                 return 1;
         else if ((operator1 == '*')||(operator1 == '/'))
                 return 2;
         else if (operator1 == '^')
                 return 4;
         else if (operator1 == '(')
                 return 4;
		 else
				 return -1;
 }

double calculate (double op1, double op2, char operator1) //Calculate function considers what the operator is and applies nessacery calculations
{
	double result = 0;

	if (operator1 == '^')
	{
		result = pow(op1,op2);
	}
	else if (operator1 == '*')
	{
		result = op1 * op2;
	}
	else if (operator1 == '/')
	{
		result = op1 / op2;
	}
	else if (operator1 == '+')
	{
		result = op1 + op2;
	}
	else if (operator1 == '-')
	{
		result = op1 - op2;
	}
	else
	{
		printf("ERROR! Invalid Operator!\n");
	}
return result;
}
