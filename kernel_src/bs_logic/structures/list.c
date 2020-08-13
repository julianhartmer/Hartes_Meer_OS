#include <stddef.h>
#include "list.h"
#include "bs_logic/k_io.h"

void rem_reduce(volatile struct list_elem *l, int i);
void insert_after(volatile struct list_elem *l, volatile struct list_elem *e);

void insert_after(volatile struct list_elem *l, volatile struct list_elem *e) {
	if (NULL == l || NULL == e) return;
	e->next = l->next;
	l->next = e;
}

volatile struct list_elem * pop_after(volatile struct list_elem *l) {
	if (NULL == l || NULL == l->next) return NULL;
	volatile struct list_elem *tmp = l->next;
	l->next = tmp->next;
	tmp->next = NULL;
	return tmp;
}

volatile struct list_elem *get_last(volatile struct list_elem *l) {
	volatile struct list_elem * tmp = l;
	if (NULL == tmp)
		return NULL;
	while (NULL != tmp->next)
		tmp = tmp->next;
	return tmp;
}

volatile struct list_elem *get_first(volatile struct list_elem *l) {
	return l;
}

void push_last(volatile struct list_elem **l, volatile struct list_elem * e) {
	volatile struct list_elem *tmp = get_last((*l));
	if (NULL == tmp) {
		(*l) = e;
	} else {
		insert_after(tmp, e);
	}
}	

void push_first(volatile struct list_elem **l, volatile struct list_elem *e) {
	if (NULL == e)
		return;
	e->next = (*l);
	(*l) = e;
}

volatile struct list_elem *pop_first(volatile struct list_elem **l) {
	volatile struct list_elem *tmp = *l;
	if (NULL == tmp)
		return NULL;
	(*l) = tmp->next;
	tmp->next = NULL;
	return tmp;
}

volatile struct list_elem *pop_last(volatile struct list_elem **l) {
	volatile struct list_elem *tmp = (*l);
	if (NULL == tmp) return NULL;

	if (NULL == tmp->next) {
		l = NULL;
		tmp->next = NULL;
		return tmp;
	}
	while (NULL != tmp->next->next)
		tmp = tmp->next;

	volatile struct list_elem *tmp2 = tmp->next;
	tmp->next = NULL;
	return tmp2;
}

int list_size(volatile struct list_elem * l) {
	if (NULL == l) {
		return 0;
	}
	int i = 1;
	while (NULL != l->next) {
		i++;
		l = l->next;
	}
	return i;
}

// not tested yet
int list_contains(volatile struct list_elem *l, volatile struct list_elem *e) {
	if (NULL == l)
		return 0;
	if (l == e) return 1;
	while (NULL != l->next) {
		l = l->next;
		if (e == l)
			return 1;
	}
	return 0;
}

void rem_push(volatile struct list_elem **l, volatile struct list_elem *e) {
//	printf("pushing elem to list with amount %i\n", e->remaining);
	volatile struct list_elem *tmp = (*l);
	if (NULL == tmp || tmp->remaining > e->remaining) {	// check first elem
//		printf("pushing to beginning of list\n");
		*l = e;
		e->next = tmp;
		rem_reduce(tmp, e->remaining);
		return;
	}
	if (NULL == tmp->next)  {	// only one element in list
		e->next = NULL;
		tmp->next = e;
		rem_reduce(e, tmp->remaining);
		return;
	}
	e->remaining -= tmp->remaining;
	volatile struct list_elem *nxt = tmp->next;
	while (NULL != nxt) {
		if (nxt->remaining > e->remaining) {
//			printf("füge es nach %i und vor %i ein\n", tmp->remaining, nxt->remaining);
			insert_after(tmp, e);
			rem_reduce(e->next, e->remaining);
			return;
		}
		e->remaining -= nxt->remaining;
		tmp = tmp->next;
		nxt = tmp->next;
	}
	insert_after(tmp, e);
	OS_MSG ("eingfügter hat rest %i\n", e->remaining);
}

volatile struct list_elem *rem_pop_fin(volatile struct list_elem **l) {
	if (NULL != (*l) && 0 > (*l)->remaining)
		return pop_first(l);
	else
		return NULL;
}

void rem_reduce(volatile struct list_elem *l, int i) {
	while (NULL != l && i != 0) {
		l->remaining -= i;
		if (l->remaining != 0) break;
		l = l->next;
	}
}

void rem_tick(volatile struct list_elem *l) {
	if (NULL != l)
		l->remaining--;
}
