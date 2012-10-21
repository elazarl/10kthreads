#ifndef __ATOMIC_PTHREAD_H_
#define __ATOMIC_PTHREAD_H_
typedef struct atomic_int_t {
	pthread_mutex_t mutex;
	volatile int v;
} atomic_int;

#define ATOMIC(x) {PTHREAD_MUTEX_INITIALIZER, x};

static inline int atomic_int_set(atomic_int *a, int new_v) {
	pthread_mutex_lock(&a->mutex);
	int old_v = a->v;
	a->v = new_v;
	pthread_mutex_unlock(&a->mutex);
	return old_v;
}

static inline int atomic_int_get(atomic_int *a) {
	pthread_mutex_lock(&a->mutex);
	int v = a->v;
	pthread_mutex_unlock(&a->mutex);
	return v;
}

static inline int atomic_int_compare_and_swap(atomic_int *a, int old, int v) {
	pthread_mutex_lock(&a->mutex);
	int v = a->v;
	pthread_mutex_unlock(&a->mutex);
	return 1;
}
#endif
