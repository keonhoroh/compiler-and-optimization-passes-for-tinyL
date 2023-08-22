#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

/*
* Will traverse instructions in groups of 3, searching for a patten of LOADI, LOADI, and ADD | SUB | MUL instructions 
*/

Instruction *constant_folding(Instruction *head) {

	Instruction *ptr = head;
	if(ptr->next != NULL) {
		ptr = ptr->next; // skip r0
	}
	
	if(ptr->next == NULL) {
		return head;
	}
	
	
	while(ptr->next->next != NULL) {
		if(ptr->opcode == LOADI && ptr->next->opcode == LOADI && ((ptr->next->next->opcode == ADD) || ptr->next->next->opcode == SUB || ptr->next->next->opcode == MUL)) {
			switch(ptr->next->next->opcode) {
				case ADD: ;
					if((ptr->next->next->field1 == ptr->field2 && ptr->next->next->field2 == ptr->next->field2) || (ptr->next->next->field2 == ptr->field2 && ptr->next->next->field1 == ptr->next->field2)) {
						int numRegister = ptr->next->next->field3;
						int sum = (ptr->field1) + (ptr->next->field1);
						ptr->next->next->opcode = LOADI;
						ptr->next->next->field1 = sum;
						ptr->next->next->field2 = numRegister;
						if(ptr->next->next->next != NULL) {
							ptr = ptr->next->next->next;
							continue;
						} else {
							return head;
						}
					}
					
				case SUB: ;
					if(ptr->next->next->field1 == ptr->field2 && ptr->next->next->field2 == ptr->next->field2) {
						int numRegister = ptr->next->next->field3;
						int difference = (ptr->field1) - (ptr->next->field1);
						ptr->next->next->opcode = LOADI;
						ptr->next->next->field1 = difference;
						ptr->next->next->field2 = numRegister;
						if(ptr->next->next->next != NULL) {
							ptr = ptr->next->next->next;
							continue;
						} else {
							return head;
						}
					} else {
						if(ptr->next->next->field2 == ptr->field2 && ptr->next->next->field1 == ptr->next->field2) {
							int numRegister = ptr->next->next->field3;
							int difference = (ptr->next->field1) - (ptr->field1);
							ptr->next->next->opcode = LOADI;
							ptr->next->next->field1 = difference;
							ptr->next->next->field2 = numRegister;
							if(ptr->next->next->next != NULL) {
								ptr = ptr->next->next->next;
								continue;
							} else {
								return head;
							}
						}
					}
				case MUL:
					if((ptr->next->next->field1 == ptr->field2 && ptr->next->next->field2 == ptr->next->field2) || (ptr->next->next->field2 == ptr->field2 && ptr->next->next->field1 == ptr->next->field2)) {
						int numRegister = ptr->next->next->field3;
						int product = (ptr->field1) * (ptr->next->field1);
						ptr->next->next->opcode = LOADI;
						ptr->next->next->field1 = product;
						ptr->next->next->field2 = numRegister;
						if(ptr->next->next->next != NULL) {
							ptr = ptr->next->next->next;
							continue;
						} else {
							return head;
						}
					}
				case LOADI: // vvv these cases will never occur vvv
				case LOADAI:
				case STOREAI:
				case DIV:
				case LSHIFTI:
				case RSHIFTI:
				case OUTPUTAI:
					break;
			}
		}
		ptr = ptr->next;
	}
	
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

	head = constant_folding(head);

	if (head) 
		PrintInstructionList(stdout, head);
	
	return EXIT_SUCCESS;
}

