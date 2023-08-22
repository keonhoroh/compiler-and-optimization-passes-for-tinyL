#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "InstrUtils.h"
#include "Utils.h"

/*
* Will return log2(n) if it is an integer (meaning n is a power of 2), and -1 otherwise
*/
int int_log2(int n) {

	int s = 1, i = 0;

	while (s < n) {
		s = s << 1;
		i++;
	}

	if (s == n)
		return i;
	else
		return -1;

}

/*
* Will traverse instructions in groups of two, searching for a patten of LOADI and MUL | DIV instructions 
* where LOADI is a power of 2, and collapse these instructions into on shift operation of LSHIFTI | RSHIFTI
*/
Instruction *strength_reduction(Instruction *head) {

	Instruction *ptr = head;
	if(ptr->next != NULL) {
		ptr = ptr->next; // skip r0
	}
	
	if(ptr->next == NULL) {
		return head;
	}

	while(ptr->next != NULL) {
		if(ptr->opcode != LOADI || (ptr->opcode == LOADI && int_log2(ptr->field1) == -1)) { // skips all nodes that aren't LOADI or non power of 2
			ptr = ptr->next;
			continue;
		}
		if(ptr->next->opcode == DIV || ptr->next->opcode == MUL) {
			switch(ptr->next->opcode) {
				case DIV:
					if(ptr->field2 == ptr->next->field2) {
						int rshift = int_log2(ptr->field1);
						ptr->next->opcode = RSHIFTI;
						ptr->next->field2 = rshift;
						if(ptr->next->next != NULL) {
							ptr = ptr->next->next;
							continue;
						} else {
							return head;
						}
					}
				case MUL:
					if(ptr->field2 == ptr->next->field2 || ptr->field2 == ptr->next->field1) {
						int lshift = int_log2(ptr->field1);
						ptr->next->opcode = LSHIFTI;
						if(ptr->field2 == ptr->next->field1) {
							ptr->next->field1 = ptr->next->field2;
						}
						ptr->next->field2 = lshift;
						if(ptr->next->next != NULL) {
							ptr = ptr->next->next;
							continue;
						} else {
							return head;
						}
					}
				case ADD: // vvv these cases will never occur vvv
				case SUB:
				case LOADI: 
				case LOADAI:
				case STOREAI:
				case LSHIFTI:
				case RSHIFTI:
				case OUTPUTAI:
					break;
			}
		}

		
		if(ptr->next->next->opcode == DIV || ptr->next->next->opcode == MUL) {
			switch(ptr->next->next->opcode) {
				case DIV:
					if(ptr->field2 == ptr->next->next->field2) {
						int rshift = int_log2(ptr->field1);
						ptr->next->next->opcode = RSHIFTI;
						ptr->next->next->field2 = rshift;
						if(ptr->next->next->next != NULL) {
							ptr = ptr->next->next->next;
							continue;
						} else {
							return head;
						}
					}
							
				case MUL:
					if(ptr->field2 == ptr->next->next->field2 || ptr->field2 == ptr->next->next->field1) {
						int lshift = int_log2(ptr->field1);
						ptr->next->next->opcode = LSHIFTI;
						if(ptr->field2 == ptr->next->next->field1) {
							ptr->next->next->field1 = ptr->next->next->field2;
						}
						ptr->next->next->field2 = lshift;
						if(ptr->next->next->next != NULL) {
							ptr = ptr->next->next->next;
							continue;
						} else {
							return head;
						}
					}

				case ADD: // vvv these cases will never occur vvv
				case SUB:
				case LOADI: 
				case LOADAI:
				case STOREAI:
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

	head = strength_reduction(head);

	if (head) 
		PrintInstructionList(stdout, head);
	
	return EXIT_SUCCESS;
}

