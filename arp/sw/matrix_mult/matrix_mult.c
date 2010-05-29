#include <stdio.h>
#include <math.h>

#define A 150

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

volatile int result[A][A];
volatile int barrier=-1;

int main(int argc, char *argv[]) {

  /* Incrementa o proc id */
  AcquireLocalLock(&lock1);
  proc++;
  int proc_id = proc;
  ReleaseLocalLock(&lock1);

  int i, j, k;
  
  /* Variaveis das matrizes */
  int matriz[A][A];

  for (i=0; i < A; i++)
    for (j=0; j < A; j++)
      matriz[i][j] = i+j;

  /* Especie de barreira para esperar todos chegarem aqui */
  for (i=0; i < 1000; i++);

  i = 0;
  j = proc_id;
    
  while (i < A)
  {
    while (j < A)
    {
      /* Nao precisa de lock porque trabalham em variaveis diferentes */
      result[i][j] = 0;
      for (k = 0; k < A; k++)
      {
        result[i][j] = result[i][j] + (matriz[i][k] * matriz[k][j]);
      }
      /*      
      AcquireLocalLock(&lock1);
      printf("Eu %d fiz (%d,%d)\n", proc_id, i, j);
      ReleaseLocalLock(&lock1);
      */
      /* Increment j */
      j += 1*(proc+1);
    }
    /* Increment i */
    j = proc_id;    
    i += 1;
  }

  AcquireLocalLock(&lock1);
  barrier += 1;
  ReleaseLocalLock(&lock1);

  /* Especie de barreira para esperar todos chegarem aqui */
  AcquireLocalLock(&lock1);
  while (barrier != proc)
  {
    ReleaseLocalLock(&lock1);
    for(i=0; i<100; i++);
    AcquireLocalLock(&lock1);
  }
  ReleaseLocalLock(&lock1);

  /* Proc0 prints result */
  /*if (proc_id == 0)
  {
    for (i=0; i < A; i++)
    {
      for (j=0; j < A; j++)
      {
        printf("%5d ", result[i][j]);
      }
      printf("\n");
    }
  }*/
  
  exit(0); // To avoid cross-compiler exit routine
  return 0; // Never executed, just for compatibility
}

