#include "CacheMaker.h"
#include <stdlib.h>
#include <ctime>
#include <dlfcn.h>
#include <string.h>

#define NUM_THREADS 16 
#define NUM_CPUS 4
#define CACHE_SIZE 32767
#define BLOCK_SIZE 64
#define ASSOC 8

std::vector<MultiCacheSim *>mcs;

void *concurrent_accesses(void*np){  
  unsigned long tid = *((unsigned long*)(np));
  for(int i = 0; i < 100000; i++){
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


int main(int argc, char** argv){
  srand(time(NULL));

  pthread_t tasks[NUM_THREADS];

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

  for(int i = 0; i < NUM_THREADS; i++){
    pthread_create(&(tasks[i]), NULL, concurrent_accesses, (void*)(new int(i)));
  }
  
  for(int i = 0; i < NUM_THREADS; i++){
    pthread_join(tasks[i], NULL);
  }

  std::vector<MultiCacheSim *>::iterator i,e;
  for(i = mcs.begin(), e = mcs.end(); i != e; i++){ 
    fprintf(stderr,"%s",(*i)->Identify());
    fprintf(stderr,"--------------------------------\n");
    (*i)->dumpStatsForAllCaches(false);
    fprintf(stderr,"********************************\n");
  }
}


