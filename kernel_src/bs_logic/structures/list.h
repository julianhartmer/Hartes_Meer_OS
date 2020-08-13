#ifndef LIST_H
#define LIST_H

struct list_elem {
	volatile struct list_elem *next;
	int remaining;
};

volatile struct list_elem *get_last(volatile struct list_elem *l);
volatile struct list_elem *get_first(volatile struct list_elem *l);
void push_last(volatile struct list_elem **l, volatile struct list_elem * e);
void push_first(volatile struct list_elem **l, volatile struct list_elem *e);
volatile struct list_elem *pop_first(volatile struct list_elem **l);
volatile struct list_elem *pop_last(volatile struct list_elem **l);
int list_size(volatile struct list_elem * l);
int list_contains(volatile struct list_elem *l, volatile struct list_elem *e);

void rem_push(volatile struct list_elem **l, volatile struct list_elem *e);
volatile struct list_elem *rem_pop_fin(volatile struct list_elem **l);
void rem_tick(volatile struct list_elem *l);

#endif
