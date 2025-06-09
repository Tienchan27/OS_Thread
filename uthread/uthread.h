#ifndef UTHREAD_H
#define UTHREAD_H

void uthread_create(void (*func)(void));
void uthread_yield();
void uthread_run();
void uthread_exit();

#endif
