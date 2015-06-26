#include "CacheMaker.h"
#include <stdlib.h>
#include <ctime>
#include <dlfcn.h>
#include <string.h>
#include <time.h>

#define NUM_THREADS 16 
#define NUM_CPUS 4
#define CACHE_SIZE 32767
#define BLOCK_SIZE 64
#define ASSOC 8
#define ACCS_PER_TD 100000

std::vector<MultiCacheSim *>mcs;
size_t mcs_idx = 0;

/*
void *concurrent_accesses(void* tidvp){  
  //unsigned long tid = *((unsigned long*)(np));
  unsigned long tid = (unsigned long)(tidvp);

  for(int i = 0; i < 1000; i++){
    unsigned long addr = 1; 
    unsigned long pc = rand() % 0xdeadbeff + 0xdeadbeef; 
    unsigned long type = rand() % 2;
    if(type == 0){
      std::vector<MultiCacheSim *>::iterator i,e;
      for(i = mcs.begin(), e = mcs.end(); i != e; i++){ 
        (*i)->readLine(tid, pc, addr);
      }
    }else{
      std::vector<MultiCacheSim *>::iterator i,e;
      for(i = mcs.begin(), e = mcs.end(); i != e; i++){ 
        (*i)->writeLine(tid, pc, addr);
      }
    }
  }
  return NULL;
}
*/

void do_single_cache_test() {
  fprintf (stderr,"Single Cache test for multicachesim #%lu\n", mcs_idx);
  clock_t start = clock(), diff;

  unsigned long total_accs = ACCS_PER_TD * NUM_THREADS;
  MultiCacheSim* mc = mcs[mcs_idx];
  SMPCache* mycache = mc->findCacheByCPUId(mc->tidToCPUId(0));

  for(unsigned long i = 0; i < total_accs; i++){
    unsigned long addr = 1; 
    unsigned long pc = rand() % 0xdeadbeff + 0xdeadbeef; 
    unsigned long type = rand() % 2;

    if(type == 0){
      mycache->readLine(pc, addr);
    }
    else{
      mycache->writeLine(pc, addr);
    }
  }

  diff = clock() - start;
  int msec = diff * 1000 / CLOCKS_PER_SEC;
  fprintf(stderr,
	  "Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
}

void *concurrent_accesses_fast(void* tidvp){  
  unsigned long tid = (unsigned long)(tidvp);
  MultiCacheSim* mc = mcs[mcs_idx];
  SMPCache* mycache = mc->findCacheByCPUId(mc->tidToCPUId(tid));

  for(int i = 0; i < ACCS_PER_TD; i++){
    unsigned long addr = 1; 
    unsigned long pc = rand() % 0xdeadbeff + 0xdeadbeef; 
    unsigned long type = rand() % 2;

    if(type == 0){
      mycache->readLine(pc, addr);
    }
    else{
      mycache->writeLine(pc, addr);
    }
  }
  return NULL;
}


void *concurrent_accesses_slow(void* tidvp){  
  unsigned long tid = (unsigned long)(tidvp);
  MultiCacheSim* mc = mcs[mcs_idx];

  for(int i = 0; i < ACCS_PER_TD; i++){
    unsigned long addr = 1; 
    unsigned long pc = rand() % 0xdeadbeff + 0xdeadbeef; 
    unsigned long type = rand() % 2;

    if(type == 0){
      mc->readLine(tid, pc, addr);
    }
    else{
      mc->writeLine(tid, pc, addr);
    }
  }
  return NULL;
}

void do_slow_concurrent_test() {
  pthread_t tasks[NUM_THREADS];

  fprintf (stderr,"Slow test for multicachesim #%lu\n", mcs_idx);
  clock_t start = clock(), diff;

  for(int i = 0; i < NUM_THREADS; i++){
    pthread_create(&(tasks[i]), 
		   NULL, 
		   concurrent_accesses_slow, 
		   (void*)(i));
  }
  
  for(int i = 0; i < NUM_THREADS; i++){
    pthread_join(tasks[i], NULL);
  }

  diff = clock() - start;

  int msec = diff * 1000 / CLOCKS_PER_SEC;
  fprintf(stderr,
	  "Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
}


void do_fast_concurrent_test() {
  pthread_t tasks[NUM_THREADS];

  fprintf (stderr,"Fast test for multicachesim #%lu\n", mcs_idx);
  clock_t start = clock(), diff;

  for(int i = 0; i < NUM_THREADS; i++){
    pthread_create(&(tasks[i]), 
		   NULL, 
		   concurrent_accesses_fast, 
		   (void*)(i));
  }
  
  for(int i = 0; i < NUM_THREADS; i++){
    pthread_join(tasks[i], NULL);
  }

  diff = clock() - start;

  int msec = diff * 1000 / CLOCKS_PER_SEC;
  fprintf(stderr,
	  "Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
}



int main(int argc, char** argv){
  srand(time(NULL));

  //Invoke this program with a comman separated list of all
  //coherence protocols to be used.
  char *ct = strtok(argv[1],","); 

  while(ct != NULL){

    CacheMaker::protocol_t proto;
    proto = CacheMaker::Str2Proto(std::string(ct));

    fprintf(stderr, "Making multicache for protocol %s [code=%d]\n",
	    ct, proto);

    CacheMaker cmk;
    
    if (!cmk.SetProtocol(proto)) {
      fprintf (stderr, "Could not set protocol %s... exiting\n", ct);
      exit(1);
    }

    cmk.SetBlockSize(BLOCK_SIZE);
    cmk.SetCacheSize(CACHE_SIZE);
    cmk.SetAssoc(ASSOC);
    cmk.SetNumCPUs(NUM_CPUS);

    MultiCacheSim *newmcs = cmk.Make();
    if (newmcs == NULL) {
      fprintf (stderr, "Problem making new cache... exiting\n");
      exit(1);
    }
    mcs.push_back(newmcs);
   
    ct = strtok(NULL,",");
   }

  for (mcs_idx = 0; mcs_idx < mcs.size(); mcs_idx++) {
    do_slow_concurrent_test();
    do_slow_concurrent_test();
    do_fast_concurrent_test();
    do_fast_concurrent_test();
    do_single_cache_test();
    do_single_cache_test();
  }

  std::vector<MultiCacheSim *>::iterator i,e;
  for(i = mcs.begin(), e = mcs.end(); i != e; i++){ 
    fprintf(stderr,"%s",(*i)->Identify());
    fprintf(stderr,"--------------------------------\n");
    (*i)->dumpStatsForAllCaches(false);
    fprintf(stderr,"********************************\n");
  }
}


