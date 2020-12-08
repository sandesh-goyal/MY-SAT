#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
		
//------------------------------------------------------------------------------------------------------------------
int main() 
{ 

	FILE *fptr;
	FILE *optr;
	 
   char input_file[20], output_file[20],c,c_prev,c_next; 
   int int_c,clause_check,clause_count;

	printf("Enter the input file name: \n");
   scanf("%s", input_file);
	fptr = fopen(input_file, "r"); 
   if (fptr == NULL) 
   { 
   	printf("Cannot open file \n"); 
  		exit(0); 
   } 	

	strcpy(output_file, "out.txt");
   optr = fopen(output_file, "w");

	c = fgetc(fptr);
	//remove the comments starting with c
	while(c == 'c')
	{
		c = fgetc(fptr);
		while(c != '\n')
		{
			c = fgetc(fptr);
		}
		c = fgetc(fptr);
	}
	//remove thc characters in line starting with p
	c = fgetc(fptr);
	c = fgetc(fptr);
	c = fgetc(fptr);
	c = fgetc(fptr);
	c = fgetc(fptr);
	c = fgetc(fptr);
	
	while(c != ' ')
	{
		putc(c,optr);
		c = fgetc(fptr);
	}

	c = fgetc(fptr);
	putc(c,optr);
	c = fgetc(fptr);
	clause_count = 0;
	while(c != ' ')
	{
		int_c = (int)c - 48;
		clause_count = (clause_count * 10) + int_c;
		putc(c,optr);
		c = fgetc(fptr);
	}
	c_prev = c;
	c_next = fgetc(fptr);
	while(c != EOF)
	{
		int_c = (int)c;
		if(c <= 57)
		{
			if((c_next == '\n' && c == ' ') || (c_next == '0' && c == ' ') || (c_prev == '\n' && c == ' ') || (c_prev == ' ' && c == '0'))
			{
				
			}
			else
			{
				if(clause_check <= clause_count)
				{
					putc(c,optr);
					if(c == '\n')
					{
						clause_check++;
					}		
				}		
			}
		}
		c_prev = c;
		c = c_next;
		c_next = fgetc(fptr);
	}

	fclose(fptr);
	fclose(optr);
	printf("%d\n", clause_count);
	printf("-----------------------------------DONE----------------------------------\n");
	return 0; 
}
