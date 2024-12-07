#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sll.h"

static SllNode *sll_create_node(int data_size, void *data) {
  SllNode *node = malloc(sizeof(SllNode));
  node->data = memcpy(malloc(data_size), data, data_size);
  node->next = NULL;

  return node;
}

static void sll_destroy_node(SllNode *node) {
  free(node->data);
  free(node);
}

static SllNode *sll_get_node(Sll *sll, int index) {
  SllNode *node = sll->head;


  for (int i = 0; i < index; ++i)
  {
    node = node->next;
  }
  
  return node;
}

Sll *sll_init(int data_size) {
  Sll *sll = calloc(1, sizeof(Sll));

  sll->head = NULL;
  sll->tail = NULL;
  sll->data_size = data_size;
  sll->size = 0;

  return sll;
}

void sll_destroy(Sll *sll) {
  while (sll->head != NULL)
  {
    SllNode *next = sll->head->next;
    sll_destroy_node(sll->head);
    sll->head = next;
  }
  
  free(sll);
}

bool sll_get(Sll *sll, int index, void *out) {
  if (index >= sll->size) {
    return false;
  }

  memcpy(out, sll_get_node(sll, index)->data, sll->data_size);

  return true;
}

bool sll_add(Sll *sll, void *data) {
  if (sll->size == 0) {
    sll->head = sll_create_node(sll->data_size, data);
    sll->tail = sll->head;
    ++sll->size;

    return true;
  }

  sll->tail->next = sll_create_node(sll->data_size, data);
  sll->tail = sll->tail->next;
  ++sll->size;

  return true;
}

bool sll_set(Sll *sll, int index, void *data) {
  if (index >= sll->size) {
    return false;
  }

  SllNode *node = sll_get_node(sll, index);

  memcpy(node->data, data, sll->data_size);

  return true;
}

bool sll_insert(Sll *sll, int index, void *data) {
  if (index > sll->size) {
    return false;
  }

  if (index == sll->size) {
    return sll_add(sll, data);
  }

  ++sll->size;
  SllNode *prev = NULL;
  SllNode *node = sll->head;

  if (index == 0) {
    sll->head = sll_create_node(sll->data_size, data);
    sll->head->next = node;
    return true;
  }

  for (int i = 0; i < index; ++i)
  {
    prev = node;
    node = node->next;
  }
  
  prev->next = sll_create_node(sll->data_size, data);
  prev->next->next = node;

  return true;
}

bool sll_remove(Sll *sll, int index) {
  if (index >= sll->size) {
    return false;
  }

  --sll->size;
  SllNode *prev = NULL;
  SllNode *node = sll->head;

  if (index == 0) {
    node = sll->head->next;
    sll_destroy_node(sll->head);
    sll->head = node;
    return true;
  }

  for (int i = 0; i < index; ++i)
  {
    prev = node;
    node = node->next;
  }
  
  prev->next = node->next;
  sll_destroy_node(node);

  return true;
}

