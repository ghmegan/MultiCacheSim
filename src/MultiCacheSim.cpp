#include "MultiCacheSim.h"
#include "MESI_SMPCache.h"
#include "MSI_SMPCache.h"

MultiCacheSim::MultiCacheSim(FILE *cachestats, 
			     int size, 
			     int assoc, 
			     int bsize, 
			     CacheFactory c){

  cacheFactory = c;
  CacheStats = cachestats;
  num_caches = 0;
  cache_size = size;
  cache_assoc = assoc;
  cache_bsize = bsize; 

  #ifndef PIN
  pthread_mutex_init(&allCachesLock, NULL);
  #else
  PIN_InitLock(&allCachesLock);
  #endif

}

//concise print out is removed for now. Will add back later. Maybe. Don't you judge me.
void MultiCacheSim::dumpStatsForAllCaches(bool concise){
   
    std::vector<SMPCache *>::iterator cacheIter = allCaches.begin();
    std::vector<SMPCache *>::iterator cacheEndIter = allCaches.end();
    for(; cacheIter != cacheEndIter; cacheIter++){
      (*cacheIter)->dumpStatsToFile(CacheStats);
    }
}

void MultiCacheSim::createNewCache(){

    #ifndef PIN
    pthread_mutex_lock(&allCachesLock);
    #else
    PIN_GetLock(&allCachesLock,1); 
    #endif

    SMPCache * newcache;
    int cpuid = num_caches;
    num_caches++;
    newcache = this->cacheFactory(cpuid, 
				  &allCaches, 
				  cache_size, 
				  cache_assoc, 
				  cache_bsize, 
				  1, 
				  "LRU", 
				  false);
    allCaches.push_back(newcache);


    #ifndef PIN
    pthread_mutex_unlock(&allCachesLock);
    #else
    PIN_ReleaseLock(&allCachesLock); 
    #endif
}

void MultiCacheSim::readLine(unsigned long tid, unsigned long rdPC, unsigned long addr){
    #ifndef PIN
    pthread_mutex_lock(&allCachesLock);
    #else
    PIN_GetLock(&allCachesLock,1); 
    #endif


    SMPCache * cacheToRead = findCacheByCPUId(tidToCPUId(tid));
    if(!cacheToRead){
      return;
    }
    cacheToRead->readLine(rdPC,addr);


    #ifndef PIN
    pthread_mutex_unlock(&allCachesLock);
    #else
    PIN_ReleaseLock(&allCachesLock); 
    #endif
    return;
}
  
void MultiCacheSim::writeLine(unsigned long tid, unsigned long wrPC, unsigned long addr){
    #ifndef PIN
    pthread_mutex_lock(&allCachesLock);
    #else
    PIN_GetLock(&allCachesLock,1); 
    #endif


    SMPCache * cacheToWrite = findCacheByCPUId(tidToCPUId(tid));
    if(!cacheToWrite){
      return;
    }
    cacheToWrite->writeLine(wrPC,addr);


    #ifndef PIN
    pthread_mutex_unlock(&allCachesLock);
    #else
    PIN_ReleaseLock(&allCachesLock); 
    #endif
    return;
}

int MultiCacheSim::getStateAsInt(unsigned long tid, unsigned long addr){

  SMPCache * cacheToWrite = findCacheByCPUId(tidToCPUId(tid));
  if(!cacheToWrite){
    return -1;
  }
  return cacheToWrite->getStateAsInt(addr);

}

char *MultiCacheSim::Identify(){
  SMPCache *c = findCacheByCPUId(0);
  if(c != NULL){
    return c->Identify();
  }
  return 0;
}
  
MultiCacheSim::~MultiCacheSim(){
    std::vector<SMPCache *>::iterator cacheIter = allCaches.begin();
    std::vector<SMPCache *>::iterator cacheEndIter = allCaches.end();
    for(; cacheIter != cacheEndIter; cacheIter++){
      delete (*cacheIter);
    }
}
