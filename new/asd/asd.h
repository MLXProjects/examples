#ifndef asd_h
#define asd_h
#include <aroma.h>

//define mutex things
#ifdef LIBAROMA_CONFIG_OPENMP
	#include <omp.h>
	#define LIBAROMA_MUTEX omp_nest_lock_t
	#define libaroma_mutex_init(x) omp_init_nest_lock(&x)
	#define libaroma_mutex_free(x) omp_destroy_nest_lock(&x)
	#define libaroma_mutex_lock(x) omp_set_nest_lock(&x)
	#define libaroma_mutex_unlock(x) omp_unset_nest_lock(&x)
#else
	#define LIBAROMA_MUTEX pthread_mutex_t
	#define libaroma_mutex_init(x) pthread_mutex_init(&x,NULL)
	#define libaroma_mutex_free(x) pthread_mutex_destroy(&x)
	#define libaroma_mutex_lock(x) pthread_mutex_lock(&x)
	#define libaroma_mutex_unlock(x) pthread_mutex_unlock(&x)
#endif

#define PROG_NAME "asd"

#define logi(...) {printf("[INFO ] "); printf(__VA_ARGS__); printf("\n");}
#define loge(...) {printf("[ERROR] "); printf(__VA_ARGS__); printf("\n");}

extern void winmain();

#endif //asd_h