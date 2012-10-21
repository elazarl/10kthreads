#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
#include "atomic_gcc.h"
#else
#include "atomic_pthread.h"
#endif
