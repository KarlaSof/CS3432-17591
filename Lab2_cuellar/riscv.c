#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h> // malloc & free
#include <stdint.h> // use guaranteed 64-bit integers
#include "tokenizer.h" // Create header file and reference that
#include "memory.h" // built-in functions to read and write to a specific file
#include "memory.c"


int32_t* reg; // Array of 32 32-bit registers

void init_regs();
bool interpret(char* instr);
void write_read_demo();

/**
 * Initialize register array for usage.
 * Malloc space for each 32-bit register then initializes each register to 0.
 * Do not alter this function!
 */
void init_regs(){
	int reg_amount = 32;
	reg = malloc(reg_amount * sizeof(int32_t)); // 32 * 4 bytes
	for(int i = 0; i < 32; i++)
		reg[i] = i;
}
void print_regs(){
	int col_size = 10;
	for(int i = 0; i < 8; i++){
		printf("X%02i:%.*lld", i, col_size, (long long int) reg[i]);
		printf(" X%02i:%.*lld", i+8, col_size, (long long int) reg[i+8]);
		printf(" X%02i:%.*lld", i+16, col_size, (long long int) reg[i+16]);
		printf(" X%02i:%.*lld\n", i+24, col_size, (long long int) reg[i+24]);
	}
}

// function to retrieve user instructions
char* get_riscv(){
  char* instructions = (char*) malloc(150 * sizeof(char));
  printf("Please enter RISC-V command(s): ");
  scanf("%149[^\n]", instructions);
  return instructions;
}
//function using strtok() to convers instructions to tokens
//seperator = what to use to seperate into tokens in this case " "
char** to_tokens(char* instructions, char *seperator){
  char** tokens = (char**) malloc(150 * sizeof(char));
  int count=0;
  int words=0;
  char *p;
  p = strtok(instructions, seperator);
  while(p != NULL){
    tokens[count++]=p;
    words +=1;
    p = strtok(NULL,seperator);
  }
  for(int i=0; i<words; i++){
    printf("token[%d]: %s\n", i, tokens[i]);
  }
  return tokens;
}
//returns if two strings are equal to each other
bool string_comp(char* str1, char* str2){
	char *p1 = NULL;
	char *p2 = NULL;
	//using two pointers to move through string
	bool comp = true;
	p1 = str1;
	p2 = str2;
	if(p1 == p2){
		p1++;
		p2++;
	}
	else{
		comp = false;
	}
	//printf("%d", comp);
	return comp;
}
//created own atoi to ignore the first character X by skipping
int myatoi(char* str){
	int num = 0;
	for(int i=1; str[i] != '\0';i++){
		num = num * 10 + str[i] - '0';

	}
	return num;
}
int execute_instruction(char** tokens){
	int rd, rs1, rs2, imm;
	if(string_comp(tokens[0],"ADD")){
		rd = myatoi(tokens[1]);
		rs1 = myatoi(tokens[2]);
		rs2 = myatoi(tokens[3]);
		//add new value to destination register
		reg[rd] = reg[rs1] + reg[rs2];
		return 0; //ADD
	}
	if(string_comp(tokens[0],"LW")){
		rd = myatoi(tokens[1]);
		//seperate last two instructions with '('
 		char** inst = to_tokens(tokens[2],"(");
		// i also use the regular atoi() function to convert to int if no character is first
		rs1 = atoi(inst[0]);
		imm = myatoi(inst[1]);
		//get new address from info give
		int address = reg[rs1]+imm;
		//use fucntion read_address() to save new info in destination register
		int32_t read = read_address(address,"mem.txt");
		reg[rd]= read;
		return 1; //LW

	}
	if(string_comp(tokens[0],"SW")){
		rd = myatoi(tokens[1]);
 		char** inst = to_tokens(tokens[2],"(");
		rs1 = atoi(inst[0]);
		imm = myatoi(inst[1]);
		int address = reg[rs1]+imm;
		int data = reg[rd];
		//use write_address to change value of address of rd
		write_address(address,data,"mem.txt");
		return 2; //SW

	}
	else{
	 //instruction should be ADDI
	 rd = myatoi(tokens[1]);
 	 rs1 = myatoi(tokens[2]);
 	 rs2 = atoi(tokens[3]);
	 //add new value to distination register
	 reg[rd] = reg[rs1] + imm;
	return -1; //ADDI
}
printf("Does not take instructions of this type");
return -2; //Not executable
}


/**
 * Fill out this function and use it to read interpret user input to execute RV64 instructions.
 * You may expect that a single, properly formatted RISC-V instruction string will be passed
 * as a parameter to this function.
 */
bool interpret(char* instr){
	char** tokens = to_tokens(instr," ");
	execute_instruction(tokens);
	return true;
}


/**
 * Simple demo program to show the usage of read_address() and write_address() found in memory.c
 * Before and after running this program, look at mem.txt to see how the values change.
 * Feel free to change "data_to_write" and "address" variables to see how these affect mem.txt
 * Use 0x before an int in C to hardcode it as text, but you may enter base 10 as you see fit.
 */
void write_read_demo(){
	int32_t data_to_write = 0xFFF; // equal to 4095
	int32_t address = 0x98; // equal to 152
	char* mem_file = "mem.txt";

	// Write 4095 (or "0000000 00000FFF") in the 20th address (address 152 == 0x98)
	int32_t write = write_address(data_to_write, address, mem_file);
	if(write == (int32_t) NULL)
		printf("ERROR: Unsucessful write to address %0X\n", 0x40);
	int32_t read = read_address(address, mem_file);

	printf("Read address %lu (0x%lX): %lu (0x%lX)\n", address, address, read, read); // %lu -> format as an long-unsigned

}

/**
 * Your code goes in the main
 *
 */
int main(){
	// Do not write any code between init_regs
	init_regs(); // DO NOT REMOVE THIS LINE
	//print_regs();
	printf(" RV32 Interpreter.\nType RV32 instructions. Use upper-case letters and space as a delimiter.\nEnter 'EOF' character to end program\n");

	char* instruction = malloc(1000 * sizeof(char));
	bool is_null = false;
	// fgets() returns null if EOF is reached.
	is_null = fgets(instruction, 1000, stdin) == NULL;
	while(!is_null){
		interpret(instruction);
		printf("\n");
		print_regs();
		printf("\n");

		is_null = fgets(instruction, 1000, stdin) == NULL;
	}

	printf("Good bye!\n");
	return 0;

}
