#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <stdint.h>

#include "callingcpp.h"

char threadAction[200], ip[20], fp[20], fId[20], sc[200], line[256];
struct stack // Creating stack
{
	char info[100];
	struct stack *link;
};
struct stack *start = NULL;

int main(int argc, char **argv) {
	system("/home/mazumdar/DRTSIM/drt-code/tregression.sh"); //Calling DRT script from here

	setenv("DRT_DEBUG", "4", 0); //setting the environmental variable
	system("./fib-tsu4 > output.txt"); //calling DRT and saving output to a file
	fileRead();
	//helloWorld();
	return 0;
}

// The following code is to read the file and parse it
fileRead(void) { 
       int counter;
	static const char filename[] = "output.txt";
	FILE *file = fopen(filename, "r");
	if (file != NULL) {

		while (fgets(line, sizeof line, file) != NULL) /* read a line */
		{
			fputs(line, stdout); /* write the line into console*/
			sscanf(line, "%s %s %s %s %s", threadAction, fId, ip, fp, sc); // Parsing the line by line
			dtsStack(ip); //Calling stack creation for IP values
			counter = lffttable (fp, " Used ");
 			counter = wtTable (fp, ip, sc, " Used ");
			counter = plqTable (fp, ip, " Used ");
	
 			//printf(" counter = %d \n", counter);
			printf("%s     %s     %s    %s   %s\n", threadAction,fId,ip,fp,sc);   // Print the parsed output
		}
		fclose(file);
		display("IP"); // Calling to display Stack
		callPrint(counter, " LFFT ");
		fflush(stdout);
		callPrint(counter, " WT ");
		fflush(stdout);
		callPrint(counter, " PLQ ");
		fflush(stdout);
	} else {
		perror(filename);
	}
	return 0;
}
dtsStack(char *value) // generating the stack based on the different inputs (IP, FP, SC). All will have their own stack
{
	struct stack *new, *temp;
	int i = 0;
	for (temp = start; temp != NULL; temp = temp->link) {
		i++;
	}
	if (i == 100) //limit of the stack size (e.g=100)
			{
		printf("Stack Overflow");
	} else {
		new = (struct stack *) malloc(sizeof(struct stack));
		strcpy((new->info), ip);
		new->link = start;
		start = new;
	}
}

display(char *stackName) // Displays the stack based on the different inputs (IP, FP, SC).
{
	struct stack *temp;
	printf("\n****%s stack****\n", stackName);
	for (temp = start; temp != NULL; temp = temp->link) {
		printf("%s\n", temp->info);
	}
}

