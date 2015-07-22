#include "SMPCache.h"
#include <string.h>

SMPCache::SMPCache(int cpuid, cachev_t* cacheVector){

  CPUId = cpuid;
  allCaches = cacheVector;

  memset(&stats, 0, (TOTAL_CACHE_STATS * sizeof(stat_t)));
}

std::string SMPCache::stat2str(stat_id_t statid) {
  switch(statid) {
  case ReadHits: 
    return std::string("ReadHits");
  case ReadMisses: 
    return std::string("ReadMisses");
  case ReadChipMisses: 
    return std::string("ReadChipMisses");
    
  case ReadOnInvalidMisses: 
    return std::string("ReadOnInvalidMisses");
  case ReadRequestsSent: 
    return std::string("ReadRequestsSent");
  case ReadMissesServicedByOthers: 
    return std::string("ReadMissesServicedByOthers");
  case ReadMissesServicedByShared:
    return std::string("ReadMissesServicedByShared");
  case ReadMissesServicedByModified:
    return std::string("ReadMissesServicedByModified");
    
  case WriteHits:
    return std::string("WriteHits");
  case WriteMisses:
    return std::string("WriteMisses");
  case WriteBacks:
    return std::string("WriteBacks");
    
  case WriteOnSharedMisses:
    return std::string("WriteOnSharedMisses");
  case WriteOnInvalidMisses:
    return std::string("WriteOnInvalidMisses");
  case InvalidatesSent:
    return std::string("InvalidatesSent");
  default:
    return std::string("INVALID_CACHE_STAT");
  }
}

void SMPCache::dumpStatsToFile(FILE* outFile, stat_t* in_stats, bool trunc){
  if (in_stats == NULL)
    in_stats = stats;
  
  fprintf(outFile, "-----Cache %lu of %lu -----\n",CPUId, allCaches->size());

  for (int idx = 0; idx < TOTAL_CACHE_STATS; idx++) {
    if (!trunc || (in_stats[idx] > 0))
      fprintf(outFile, "%-20s: %d\n", 
	      stat2str((stat_id_t)idx).c_str(), in_stats[idx]);
  }
}


