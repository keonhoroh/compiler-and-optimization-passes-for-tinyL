/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Spring 2023                              *
 *  Student Version                          *
 *  Author: Nathan K Roh                     *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

int getSize(Instruction* ptr, Instruction* head) {
	int size = 1;
	ptr = head;
	while(ptr != NULL) {
		size++;
		ptr = ptr->next;
	}

	return size;
}

void printCrit(Instruction* temp) {
	while(temp != NULL) {
		printf("%d ", temp->critical);
		temp = temp->next;
	}
	printf("\n");
}

void markSubInstructions(Instruction* ptr, int* registers) { // ptr at STOREAI->prev; int reg is STOREAI's field1
	if(ptr->opcode == LOADI && ptr->field2 == registers[0]) {
		ptr->critical = 3;
	} else {
		switch(ptr->opcode) {
			case LOADAI:
				if(ptr->field3 == registers[0]) { // LOADAI
					ptr->critical = 3;
					registers[0] = ptr->field1;
					registers[1] = ptr->field2;
					markSubInstructions(ptr->prev, registers);
				} else {
					markSubInstructions(ptr->prev, registers);
				}
			case STOREAI:
				if(ptr->field2 == registers[0] && ptr->field3 == registers[1]) { // STOREAI
					ptr->critical = 3;
					registers[0] = ptr->field1;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
				} else {
					markSubInstructions(ptr->prev, registers);
				}
			case LSHIFTI:
				if(ptr->field3 == registers[0]) { //LSHIFTI
					ptr->critical = 3;
					registers[0] = ptr->field1;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
				} else {
					//markSubInstructions(ptr->prev, registers);
				}
			case RSHIFTI:
				if(ptr->field3 == registers[0]) { // RSHIFTI
					ptr->critical = 3;
					registers[0] = ptr->field1;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
				} else {
					//markSubInstructions(ptr->prev, registers);
				}
			case ADD:
				if(ptr->field3 == registers[0]) { // ADD
					ptr->critical = 3;
					registers[0] = ptr->field1;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
					registers[0] = ptr->field2;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
				} else {
					//markSubInstructions(ptr->prev, registers);
				}
			case SUB:
				if(ptr->field3 == registers[0]) { // SUB
					ptr->critical = 3;
					registers[0] = ptr->field1;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
					registers[0] = ptr->field2;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
				} else {
					//markSubInstructions(ptr->prev, registers);
				}
			case MUL:
				if(ptr->field3 == registers[0]) { // MUL
					ptr->critical = 3;
					registers[0] = ptr->field1;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
					registers[0] = ptr->field2;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
				} else {
					//markSubInstructions(ptr->prev, registers);
				}
			case DIV:
				if(ptr->field3 == registers[0]) { // DIV
					ptr->critical = 3;
					registers[0] = ptr->field1;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
					registers[0] = ptr->field2;
					registers[1] = -1;
					markSubInstructions(ptr->prev, registers);
				} else {
					//markSubInstructions(ptr->prev, registers);
				}
			case LOADI:
				if(ptr->prev != NULL) { // LOADI
					markSubInstructions(ptr->prev, registers);
				} else {
					break;
				}
			case OUTPUTAI:
				markSubInstructions(ptr->prev, registers); // OUTPUTAI
		}
	}
}


void markCriticalInstructions(Instruction* ptr, int OUT1, int OUT2) {
	if(ptr->prev != NULL) {
		ptr = ptr->prev;
	}
	while(ptr != NULL) {
		if(ptr->opcode == STOREAI && ptr->field2 == OUT1 && ptr->field3 == OUT2) {
			ptr->critical = 3; // marking the STOREAI instruction at critical
			int* registers = (int*)malloc(2 * sizeof(int));
			registers[0] = ptr->field1;
			registers[1] = -1; //FREE THIS LATER
			markSubInstructions(ptr->prev, registers);
			free(registers);
			break;
		} else {
			if(ptr->prev != NULL) {
				ptr = ptr->prev;
			} else {
				break;
			}
		}
	}
}

void rewriteInstructions(Instruction* ptr, int size) {
	Instruction* temp;
	while(ptr != NULL) {
		if(ptr->critical != 3) {
			if(ptr->next == NULL) {
				ptr->prev->next = NULL;
				temp = ptr;
				ptr = ptr->next;
				free(temp);
			} else {
				ptr->prev->next = ptr->next;
				ptr->next->prev = ptr->prev;
				temp = ptr;
				ptr = ptr->next;
				free(temp);
			}
		} else {
			ptr = ptr->next;
		}
	}
}

Instruction *dead_code_elimination(Instruction *head) {

	Instruction *ptr = head;
	Instruction* temp = head;

	if(ptr->next != NULL) {
		ptr->critical = 3;
		ptr = ptr->next; // skip r0
	}
	if(ptr->next == NULL) {
		return head;
	}
	
	int size = getSize(ptr, head); // # of all instructions
	
	for(int i = 1; i < size; i++) {
		if(ptr->opcode == OUTPUTAI) {
			ptr->critical = 3;
			markCriticalInstructions(ptr, ptr->field1, ptr->field2);
			if(ptr->next != NULL) {
				ptr = ptr->next;
			} else {
				break;
			}
		} else {
			if(ptr->next != NULL) {
				ptr = ptr->next;
			} else {
				break;
			}
		}
	}

	// rewrite the instruction list
	rewriteInstructions(temp, size);
	return head;
}

int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}

	head = dead_code_elimination(head);

	if (head) 
		PrintInstructionList(stdout, head);
	
	return EXIT_SUCCESS;
}

