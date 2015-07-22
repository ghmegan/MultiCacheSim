#ifndef __SMPCACHE_H_
#define __SMPCACHE_H_

#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>

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
  typedef enum {
    ReadHits = 0,
    ReadMisses = 1,
    ReadChipMisses = 2,
    
    ReadOnInvalidMisses = 3,
    ReadRequestsSent = 4,
    ReadMissesServicedByOthers = 5,
    ReadMissesServicedByShared = 6,
    ReadMissesServicedByModified = 7,
    
    WriteHits = 8,
    WriteMisses = 9,
    WriteBacks = 10,
    
    WriteOnSharedMisses = 11,
    WriteOnInvalidMisses = 12,
    InvalidatesSent = 13,
    
    TOTAL_CACHE_STATS = 14,
    INVALID_CACHE_STAT = 15
  } stat_id_t;
  
  typedef int stat_t;

  stat_t stats[TOTAL_CACHE_STATS];
  stat_t save_stats[TOTAL_CACHE_STATS];

  typedef stat_t stat_vec_t[TOTAL_CACHE_STATS];  

  void mark_stats() {
    memcpy(save_stats, stats, (TOTAL_CACHE_STATS * sizeof(stat_t)));
  }

  std::string stat2str(stat_id_t statid);

  void diff_stats(stat_t* diff) {
    for (int idx = 0; idx < TOTAL_CACHE_STATS; idx++) {
      diff[idx] = stats[idx] - save_stats[idx];
    }
  }

  SMPCache(int cpuid, cachev_t* cacheVector);

  //Readline performs a read, and uses readRemoteAction to 
  //check for data in other caches
  virtual void readLine(uint32_t rdPC, uint32_t addr, 
			uint32_t& memrd, uint32_t& wrback) = 0;

  //Writeline performs a write, and uses writeRemoteAction
  //to check for data in other caches
  virtual void writeLine(uint32_t wrPC, uint32_t addr, uint32_t& wrback) = 0;
 
  //Fill line touches cache state, bringing addr's block in, and setting its state to mesi_state 
  virtual void fillLine(uint32_t addr, uint32_t mesi_state, uint32_t& wrback) = 0;

  virtual char *Identify() = 0;

  //Dump the stats for this cache to outFile
  virtual void dumpStatsToFile(FILE* outFile, 
			       stat_t* in_stats = NULL,
			       bool trunc = false);

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
