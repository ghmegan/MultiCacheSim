#include "SMPCache.h"
#include <string.h>

SMPCache::SMPCache(int cpuid, cachev_t* cacheVector){

  CPUId = cpuid;
  allCaches = cacheVector;

  memset(&stats, 0, sizeof(stats_t));
}

void SMPCache::dumpStatsToFile(FILE* outFile, stats_t* in_stats, bool trunc){
  if (in_stats == NULL)
    in_stats = &stats;
  
  fprintf(outFile, "-----Cache %lu of %lu -----\n",CPUId, allCaches->size());

#define DOPRINT(_stat_) \
  if (!trunc || (in_stats->_stat_ != 0)) {	\
    fprintf (outFile, "%-20s: %d\n", #_stat_, in_stats->_stat_); }

  DOPRINT(ReadHits);
  DOPRINT(ReadMisses);
    
  DOPRINT(ReadOnInvalidMisses);
  DOPRINT(ReadRequestsSent);
  DOPRINT(ReadMissesServicedByOthers);
  DOPRINT(ReadMissesServicedByShared);
  DOPRINT(ReadMissesServicedByModified);
  
  DOPRINT(WriteHits);
  DOPRINT(WriteMisses);
  DOPRINT(WriteBacks);
  
  DOPRINT(WriteOnSharedMisses);
  DOPRINT(WriteOnInvalidMisses);
  DOPRINT(InvalidatesSent);
}


