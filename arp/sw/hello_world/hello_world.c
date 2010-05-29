#include <stdio.h>

#define GLOBAL_LOCK 0x500000
#define AcquireGlobalLock while(*g_lock)
#define ReleaseGlobalLock *g_lock=0
volatile int *g_lock = (int*) GLOBAL_LOCK;
volatile int lock1 = 0;
volatile int proc=-1;

void inline AcquireLocalLock(volatile int *lock)
{
  int i;
  AcquireGlobalLock;
  while(*lock)
  {
    ReleaseGlobalLock;
    for(i=0; i < 10; i++) {}
    AcquireGlobalLock;
  };
  *lock=1;
  ReleaseGlobalLock;
}

void inline ReleaseLocalLock(volatile int *lock)
{
  AcquireGlobalLock;
  *lock=0;
  ReleaseGlobalLock;
}

int main(int argc, char *argv[]){
  /* Incrementa o proc id */
  proc++;
  int proc_id = proc;
  int i, j;
  int a[1024], sum=0;

  for (i=0; i < 1024; i++)
    a[i] = i;

  for(i=0;i<1000;i++)
  {
    AcquireLocalLock(&lock1);
    printf("Hi from processor MIPS %d!\n", proc_id);
    ReleaseLocalLock(&lock1);
    for(j=0; j < 100; j++) {}
  }

  for (i=0; i< 1024; i++)
    sum+=a[i];

  AcquireLocalLock(&lock1);
  printf("Soma = %d\n", sum);
  ReleaseLocalLock(&lock1);  
  
  exit(0); // To avoid cross-compiler exit routine
  return 0; // Never executed, just for compatibility
}

