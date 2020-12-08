#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

void read_file();
void write_file();
void sat_solver();
void check_sat();
void comb_check();

int variable_count;
int clause_count;
//int clause[3][5];
//int clause_literal[3];
int **clause;//2D array
int *clause_literal_count;//1D array
int *variable_value;//1D array
int *mark;//1D array
int *variable_occurence;//1D array
int *variable_order;//1D array
int is_sat;
//------------------------------------------------------READ FILE---------------------------------------------------
void read_file()
{
	FILE *fptr; 
	 
   char input_file[20], c; 
   int i, ch, read_check, invert, literal_count, value;

   printf("Enter the input file name: \n");
   scanf("%s", input_file);
	//strcpy(input_file, "cnf1.txt");

	//Open file 
   fptr = fopen(input_file, "r"); 
   if (fptr == NULL) 
   { 
   	printf("Cannot open file \n"); 
  		exit(0); 
   } 

 	//Read contents from file
	variable_count = 0;
	clause_count = 0;

	//Read variable count
	c = fgetc(fptr);
	while (c != ' ')
	{
		ch = c;
		variable_count = (variable_count * 10) + (ch - 48);
		c = fgetc(fptr);
	}

	//Read clause count
	c = fgetc(fptr);
	while (c != '\n')
	{
		ch = c;
		clause_count = (clause_count * 10) + (ch - 48);
		c = fgetc(fptr);
	}

	//Memory allocation for clause
	clause = (int **)calloc(clause_count,sizeof(int*)); 
	for(int i = 0; i < clause_count; i++)
	{
   	clause[i] = (int *)calloc(variable_count,sizeof(int));
	}
	if (clause == NULL) 
	{ 
		printf("Memory not allocated for clause\n"); 
		exit(0); 
   }

	//Memory allocation for clause_literal_count
	clause_literal_count = (int *)calloc(clause_count,sizeof(int));
	if (clause_literal_count == NULL) 
	{ 
		printf("Memory not allocated for clause_literal_count\n"); 
		exit(0); 
   }

	//Memory allocation for variable_value
	variable_value = (int *)calloc(variable_count,sizeof(int));
	if (variable_value == NULL) 
	{ 
		printf("Memory not allocated for variable_value\n"); 
		exit(0); 
   }

	//Memory allocation for mark
	mark = (int *)calloc(variable_count,sizeof(int));
	if (mark == NULL) 
	{ 
		printf("Memory not allocated for mark\n"); 
		exit(0); 
   }

	//Memory allocation for variable_occurence
	variable_occurence = (int *)calloc(variable_count,sizeof(int));
	if (variable_occurence == NULL) 
	{ 
		printf("Memory not allocated for variable_occurence\n"); 
		exit(0); 
   }

	//Memory allocation for variable_order
	variable_order = (int *)calloc(variable_count,sizeof(int));
	if (variable_order == NULL) 
	{ 
		printf("Memory not allocated for variable_order\n"); 
		exit(0); 
   }
	
	//Initialize variable_occurence and variable_order
	for(i = 0; i < variable_count; i++)
	{
		variable_occurence[i] = 0;
		variable_order[i] = i+1;
	}

	//Read Clause
	literal_count = 0;
	read_check = 0;
	invert = 0;
	value = 0;
   while (read_check < clause_count) 
   { 
		c = fgetc(fptr);
   	while (c != '\n')
		{
			if(c == ' ')
			{
				if(invert == 1)
				{
					value = -value;
				}
				clause[read_check][literal_count] = value;
				variable_occurence[abs(value) - 1]++;
				c = fgetc(fptr);
				literal_count++;
				invert = 0;
				value = 0;
			}
			else
			{
				if(c == '-')
				{
				invert = 1;
				}
				else
				{
					ch = c;
					value = (value * 10) + (ch - 48);
				}
				c = fgetc(fptr);
			}
			if(c == '\n')
			{
				if(invert == 1)
				{
					value = -value;
				}
				clause[read_check][literal_count] = value;
				variable_occurence[abs(value) - 1]++;
				literal_count++;
			}
		}
		clause_literal_count[read_check] = literal_count;
		read_check++;
		literal_count = 0;
		invert = 0;
		value = 0;
   }
	fclose(fptr);

	//----------------------------test points
//	for(i = 0; i < variable_count; i++)
//	{
//		printf("variable_occurence[%d] : %d\n", i+1, variable_occurence[i]);
//		printf("variable_order[%d] : %d\n", i+1, variable_order[i]);
//	}

	//Resize Memory allocation for clause(remove extra memory space)
	for(i = 0; i < clause_count; i++)
	{
   	clause[i] = (int *)realloc(clause[i],clause_literal_count[i]*sizeof(int));
	}
}
//-----------------------------------------------------WRITE FILE---------------------------------------------------
void write_file()
{
	FILE *optr; 
	 
   char output_file[20], c;
   int i,value,variable;
	unsigned int mask;
	mask = 1;
	strcpy(output_file, "result.txt");
   //output file
   optr = fopen(output_file, "w");
	if(is_sat == 1)
	{
		fprintf(optr,"SAT\n");
		fprintf(optr,"Variable value for SAT:\n");
		for(i = 0; i < variable_count; i++)
		{
			value = variable_value[i];
	//----------------------------test points
			//printf("%d\n", value);
			variable = i + 1;
			if(value == 0)
			{
				fprintf(optr,"-");
			}
			fprintf(optr,"%d\n", variable);
		}
	//----------------------------test points
		//printf("%d\n", sat_variable_value);
	}
	else
	{
		fprintf(optr,"UNSAT\n");
	}
	fclose(optr);
}
//-----------------------------------------------------SAT SOLVER---------------------------------------------------
void sat_solver()
{
	unsigned int i,j,k,check_length,mask,bit_value,skip_value;
	int temp_clause_value,abs_clause_value,temp_variable_value,mark_check,is_complement,is_uncomplement;
	int unmarked_count,swap1,swap2,t;
	//----------------------------------------------------
	//check for all 1
	for(i = 0; i < variable_count; i++)
	{
		variable_value[i] = 1;
	}
	check_sat();
	if(is_sat == 1) return;
	//----------------------------------------------------
	//check for all 0
	for(i = 0; i < variable_count; i++)
	{
		variable_value[i] = 0;
	}
	check_sat();
	if(is_sat == 1) return;
	//----------------------------------------------------
	//Unit Clause Rule
	//assign 1/0 to unit clause variables
	//marked variale value is not changed
	//terminates with UNSAT in case of conflict
	//Unit clause is implemented only at initial level because sub-class of SAT (n-SAT) n?, is not defined
	for(i = 0; i < clause_count; i++)
	{
		if(clause_literal_count[i] == 1)
		{
			temp_clause_value = clause[i][0];
	//----------------------------test points
			//printf("temp_clause_value : %d\n", temp_clause_value);
			abs_clause_value = abs(temp_clause_value);
			mark_check = mark[(abs_clause_value - 1)];
			if(temp_clause_value < 0)
			{
				temp_variable_value = 0;
			}
			else
			{
				temp_variable_value = 1;
			}
			if(mark_check == 1)//check conflict
			{
	//----------------------------test points
				//printf("!!!CONFLICT!!!\n");
				if(variable_value[(abs_clause_value - 1)] != temp_variable_value)
				{
	//----------------------------test points
					//printf("!!!CONFLICT!!!\n");
					break;
				}
			}
			else
			{
				variable_value[(abs_clause_value - 1)] = temp_variable_value;
				mark[(abs_clause_value - 1)] = 1;
	//----------------------------test points
				//printf("!!! NO CONFLICT!!!\n");
			}
		}
	}
	//Pure Literal Rule
	//assign 1/0 to variable appearing only in uncomlpemented/complemented form respectively
	for(i = 0; i < variable_count; i++)
	{
		mark_check = mark[i];
		if(mark_check == 1)
		{
			continue;
		}
		else
		{	
			is_uncomplement = 0;
			is_complement = 0;
			for(j = 0; j < clause_count; j++)
			{
				for(k = 0; k < clause_literal_count[j]; k++)
				{
					if(clause[j][k] == i+1)
					{
						is_uncomplement = 1;
					}
					if(clause[j][k] == -(i+1))
					{
						is_complement = 1;
					}
				}
			}
			if(is_uncomplement == 0)//variable is not present or present only in complemented form
			{
				mark[i] = 1;//marked as 1 with default value 0
			}
			else if(is_complement == 0 && is_uncomplement == 1)//variable only in uncomlpemented form
			{
				mark[i] = 1;//marked as 1 with default value 1
				variable_value[i] = 1;
			}
		}
	}
	//----------------------------------------------------
	//again check for all 0 except marked variables(as it is)
	check_sat();
	if(is_sat == 1) return;
	//----------------------------------------------------
	//again check for all 1 except marked variables(as it is)
	for(i = 0; i < variable_count; i++)
	{
		if(mark[i] == 0)
		{		
			variable_value[i] = 1;
		}
	}
	check_sat();
	if(is_sat == 1) return;
	//----------------------------------------------------
	//sorting the variables for assignment in ascending order
	for(i = 0; i < variable_count - 1; i++)
	{
		for(j = 0; j < variable_count - i - 1; j++)
		{
			if(variable_occurence[j] > variable_occurence[j+1])
			{
				swap1 = variable_occurence[j];
				variable_occurence[j] = variable_occurence[j+1];
				variable_occurence[j+1] = swap1;
				swap2 = variable_order[j];
				variable_order[j] = variable_order[j+1];
				variable_order[j+1] = swap2;
			}
		}
	}
	//----------------------------------------------------
	//heuristic guess with respect to maximum occurence
	//previously all values except the marked ones are set to 1 with mark
	int temp, temp1;
	temp = variable_count - 1;
	for(t = temp; t >= 0; t--)
	{
		temp1 = variable_order[i] - 1;
		if(mark[temp1] == 1)
		{
			continue;
		}
		else
		{
			variable_value[temp1] = 0;
		}
		check_sat();
		if(is_sat == 1) 
		break;
		else
		variable_value[temp1] = 1;
	}
	if(is_sat == 1) return;
	

	//----------------------------------------------------
	//check for diferent combination of values which are unmarked
	comb_check();
}
//--------------------------------------------------COMBINATION CHECK-----------------------------------------------
void comb_check()
{
	//----------------------------------------------------
	//check for diferent combination of values which are unmarked
	unsigned int i,j,k,check_length,mask,bit_value;
	int unmarked_count;

	unmarked_count = 0;
	for(i = 0; i < variable_count; i++)
	{
		if(mark[i] == 0)
		{
			unmarked_count++;
		}
	}
	check_length = pow(2,unmarked_count);
	//----------------------------test points
	printf("check_length : %d\n", check_length);
	mask = 1;
	for(i = 0; i < check_length; i++)
	{
	//----------------------------test points
		//printf("loop execution : %u\n", i);
		k = 0;
		for(j = 0; j < variable_count; j++)
		{
			if(mark[j] == 0)
			{
				bit_value = i & (mask << k);
				bit_value = bit_value >> k;
				variable_value[j] = bit_value;
	//----------------------------test points
				//printf("variable value : %d\n", variable_value[j]);
				k++;
			}
		}
		check_sat();
		if(is_sat == 1) 
		{
			break;
		}
	}
	//----------------------------test points
	//check_sat();
	//if(is_sat == 1) return;
}
//------------------------------------------------------CHECK SAT---------------------------------------------------
void check_sat()
{
	int i,j,k,l;
	int unity_count,clause_length,unity_check,temp_clause_value,abs_clause_value,bit_value,mask;
	mask = 1;
	unity_count = 0;
	for(i = 0; i < clause_count; i++)
	{
		clause_length = clause_literal_count[i];
	//----------------------------test points
		//printf("clause_length :	%d\n", clause_length);
		unity_check = 0;
		for(j = 0; j < clause_length; j++)
		{
			temp_clause_value = clause[i][j];
			abs_clause_value = abs(temp_clause_value);

	//----------------------------test points
			//printf("abs_clause_value :	%d\n", abs_clause_value);
			bit_value = variable_value[(abs_clause_value - 1)];
	//----------------------------test points
			//bit_value = ((int)pow(2,(abs_clause_value - 1))) & i;
			if(temp_clause_value < 0)
			{
				bit_value = (~bit_value) & mask;
			}
			unity_check = unity_check | bit_value;
	//save check time, break if any literal evaluates to 1
			if(unity_check == 1) 
			{
				break;
			}
		}
		if(unity_check == 1)
		{
			unity_count++;
		}
		else
		{
			break;
		}
	}
	//----------------------------test points
	//printf("unity_count :	%d\n", unity_count);
	if(unity_count == clause_count)
	{
		is_sat = 1;
	}
}
//--------------------------------------------------------MAIN------------------------------------------------------
int main() 
{ 
	int i,j;
	is_sat = 0;

	//Read CNF Epression
	read_file();

	//----------------------------------------------time----------------------------------------
	double total_time;
	clock_t start, end;
	//time count starts 
	start = clock();
	srand(time(NULL));
	//----------------------------------------------time----------------------------------------

	printf("------------------------------MY_SAT_SOLVER------------------------------\n");
	//Display read expression
	printf("Variable Count : %d\n", variable_count);
	printf("Clause Count : %d \n", clause_count);
/*
	for(i = 0; i < clause_count; i++)
	{
		for(j = 0; j < clause_literal_count[i]; j++)
		{
			printf("%d	", clause[i][j]);
		}
		printf("\n");
	}
	printf("Clause Length : ");
	for(i = 0; i < clause_count; i++)
		{
			printf("%d	", clause_literal_count[i]);
		}
	printf("\n");
*/		
	//check satisfiability
	sat_solver();
	//print result on terminal
	if(is_sat == 1)
	{
		printf("Result : SAT\n");
		//printf("%d\n", sat_variable_value);
	}
	else
	{
		printf("Result : UNSAT\n");
	}
	//----------------------------------------------time----------------------------------------
	//time count stops 
	end = clock();
	//----------------------------------------------time----------------------------------------
	//write result to output file
	write_file();
	printf("Result is written in file **result.txt**\n");

	//free memory
	free(clause);
	free(clause_literal_count);
	free(variable_value);
	printf("-----------------------------------DONE----------------------------------\n");
	//----------------------------------------------time----------------------------------------
	total_time = ((double) (end - start)) / CLOCKS_PER_SEC;
	//calulate total time
	printf("Time taken is: %f seconds\n", total_time);
	//----------------------------------------------time----------------------------------------
	return 0; 
}
