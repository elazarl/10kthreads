#ifndef __ATOMIC_GCC_H_
#define __ATOMIC_GCC_H_
typedef struct atomic_int_t {
	volatile int v;
} atomic_int;

static inline int atomic_int_set(atomic_int *a, int new_v) {
	return __sync_lock_test_and_set(&a->v, new_v);
}

static inline int atomic_int_get(atomic_int *a) {
	return a->v;
}

static inline int atomic_int_compare_and_swap(atomic_int *a, int old, int v) {
	return __sync_bool_compare_and_swap(&a->v, old, v);
}

static inline int atomic_int_add(atomic_int *a, int v) {
	return __sync_add_and_fetch(&a->v, v);
}

#define ATOMIC_INT(x) {x}

#endif
