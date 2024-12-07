#pragma once

#include <stdbool.h>

typedef struct {
  SllNode *next;
  void *data;
} SllNode;

typedef struct {
  SllNode *head;
  SllNode *tail;
  int size;
  int data_size;
} Sll;

Sll *sll_init(int data_size);
void sll_destroy(Sll *sll);
bool sll_get(Sll *sll, int index, void *out);
bool sll_add(Sll *sll, void *data);
bool sll_set(Sll *sll, int index, void *data);
bool sll_insert(Sll *sll, int index, void *data);
bool sll_remove(Sll *sll, int index);
