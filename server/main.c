#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../utils/sll.h"

int main() {
	Sll *sll = sll_init(sizeof(int));

	for (int i = 0; i < 10; ++i)
	{
		sll_add(sll, &i);
	}
	
	printf("%d\n", sll->size);
	SllNode *node = sll->head;
	
	for (int i = 0; i < sll->size; ++i)
	{
		printf("%d", *(int *)node->data);
		node = node->next;
		if (node != NULL) {
			printf(" -> ");
		}
	}
	printf("\n");

	int data = 10;
	sll_set(sll, 3, &data);
	++data;
	sll_insert(sll, 0, &data);

	node = sll->head;
	for (int i = 0; i < sll->size; ++i)
	{
		printf("%d", *(int *)node->data);
		node = node->next;
		if (node != NULL) {
			printf(" -> ");
		}
	}
	printf("\n");
	
	printf("%d\n", sll->size);

	sll_destroy(sll);

	return 0;
}
