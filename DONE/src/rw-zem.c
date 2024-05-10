#include "../lib/rw-zem.h"

void rwlock_init(rwlock_t *rw) {
    printf("an\n");
    rw->readers = 0;
    printf("ta\n");
    sem_init(&rw->lock, 0, 1);
    printf("ni\n");
    sem_init(&rw->writelock, 0, 1);
    printf("!!\n");
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers++;
    if (rw->readers == 1){ // first reader gets writelock
        sem_wait(&rw->writelock);
        sem_post(&rw->lock);
    }
}

void rwlock_release_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers--;
    if (rw->readers == 0){ // last reader lets it go
        sem_post(&rw->writelock);
        sem_post(&rw->lock);
    }
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    sem_wait(&rw->writelock);
}

void rwlock_release_writelock(rwlock_t *rw) {
    sem_post(&rw->writelock);
}