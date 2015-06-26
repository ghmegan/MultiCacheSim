#ifndef __SMPCACHE_H_
#define __SMPCACHE_H_

#include <vector>
#include <stdint.h>
#include <stdio.h>

class SMPCache {

public:
  unsigned long CPUId;
  int getCPUId() { return CPUId; }  

  //A vector of all the caches in the SMP
  //Use this to do remote data coherence
  typedef std::vector<SMPCache*> cachev_t;
  cachev_t *allCaches;
  cachev_t *getCacheVector() { return allCaches; }  

  //Stats about the events the cache saw during execution
  int numReadHits;
  int numReadMisses;

  int numReadOnInvalidMisses;
  int numReadRequestsSent;
  int numReadMissesServicedByOthers;
  int numReadMissesServicedByShared;
  int numReadMissesServicedByModified;

  int numWriteHits;
  int numWriteMisses;

  int numWriteOnSharedMisses;
  int numWriteOnInvalidMisses;
  int numInvalidatesSent;
  
  SMPCache(int cpuid, cachev_t* cacheVector);

  //Readline performs a read, and uses readRemoteAction to 
  //check for data in other caches
  virtual void readLine(uint32_t rdPC, uint32_t addr) = 0;

  //Writeline performs a write, and uses writeRemoteAction
  //to check for data in other caches
  virtual void writeLine(uint32_t wrPC, uint32_t addr) = 0;
 
  //Fill line touches cache state, bringing addr's block in, and setting its state to mesi_state 
  virtual void fillLine(uint32_t addr, uint32_t mesi_state) = 0;

  virtual char *Identify() = 0;

  //Dump the stats for this cache to outFile
  virtual void dumpStatsToFile(FILE* outFile);
  virtual void conciseDumpStatsToFile(FILE* outFile);
  
  virtual int getStateAsInt(unsigned long addr) = 0;

};

typedef SMPCache *(*CacheFactory)(int, //cpuid
				  SMPCache::cachev_t*, //cache vector
				  int, //cache size
				  int, //associativity
				  int, //block size
				  int, //addressable unit (e.g. 1 byte)
				  const char *, //replacement policy
				  bool); //is skew cache?

#endif
