#include <stdio.h>
#include <stdlib.h>

/* IMPLEMENT ME: Declare your functions here */

int add (int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);
int GetOperand();
static void GetValues(int *a, int *b);
typedef int (*operationPtr)(int, int);

int main (void)
{
	int a;
	int b;
	int operation;
	int userResult;
	
	//Can't use Conditional Statements, so store operations in array instead
	operationPtr possibleOperations[] = {add, subtract, multiply, divide};
	
	GetValues(&a, &b);//get actual values being worked with
	
	operation = GetOperand();//figure out which operation
	
	userResult = possibleOperations[operation](a,b);//get results
	
	
	//print results
	printf("x = %d \n" , userResult);
	
	
	
	
	return 0;
}

//MATH FUNCTIONS///////////////////////////////////////////////////////////////
int add (int a, int b) { 
	printf ("Adding 'a' and 'b'\n"); 
	return a + b;
}

int subtract (int a, int b) { 
	printf ("Subtracting 'a' and 'b'\n"); 
	return a - b;
}

int multiply (int a, int b) { 
	printf ("Multiplying 'a' and 'b'\n"); 
	return a * b;
}

int divide (int a, int b) { 
	printf ("Dividing 'a' and 'b'\n"); 
	return a / b;
}

///////////////////////////////////////////////////////////////////////////////

int GetOperand(){
	int operand;
	printf("Specify the operation to perform( 0 : add | 1 : subtract | 2 : multiply | 3 : divide): ");
	
	scanf("%d", &operand);
	return operand;
}

static void GetValues(int *a, int *b){
	//a
	printf("Operand 'a' : " );
	scanf("%d", a);
	
	//b
	printf("Operand 'b' : " );
	scanf("%d", b);

}
