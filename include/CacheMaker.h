#ifndef MULTICACHESIM_CACHEMAKER_H
#define MULTICACHESIM_CACHEMAKER_H

#include "MultiCacheSim.h"
#include <string>

class CacheMaker {
 private:
  FILE *_statsfp;
  int _blocksize, _cachesize, _assoc, _numcpus, _protocol;
  CacheFactory _cfac;
  
 public:

  typedef enum {
    INVALID = 0,
    MSI_SMPCache = 1,
    MESI_SMPCache = 2
  } protocol_t;

  //Because C++ has const... haha
  static const int protocol_t_max = 2;

  static std::string Proto2Str( protocol_t proto ) {
    switch (proto) {
    case MSI_SMPCache: return std::string("MSI_SMPCache");
    case MESI_SMPCache: return std::string("MESI_SMPCache");
    default: return std::string("INVALID");
    }
  }

  static protocol_t Str2Proto( const std::string &pstr ) {
    for (int proto = 1; proto <= protocol_t_max; proto++) {
      if (pstr.compare(Proto2Str((protocol_t)proto)) == 0) {
	return (protocol_t)proto;
      }
    }
    return INVALID;
  }

  static std::string ProtoList() {
    std::string ret;
    for (int proto = 1; proto <= protocol_t_max; proto++) {
      if (proto > 1) {
	ret += ", ";
      }
      ret += Proto2Str((protocol_t)proto);
    }
    return ret;
  }

  bool SetProtocol(protocol_t proto) {
    switch (proto) {
    case MSI_SMPCache:
    case MESI_SMPCache:
      _protocol = proto; return true;
    default:
      fprintf (stderr, "Invalid protocol: %d. See %s for valid protocol codes\n", proto, __FILE__);
      return false;
    }
  }

  bool IsPow2(int adx) {
    //while adx is even and greater than 1
    while (((adx & 1) == 0) && (adx > 1))
      adx >>= 1; //divide by 2
    return (adx == 1); // check if only single 1 bit
  }

  bool SetBlockSize(int bsize) { 
    if (!IsPow2(bsize)) {
      fprintf (stderr,
	       "Block size cannot be set to %d, not a power of 2\n",
	       bsize);
      return false;
    }
    _blocksize = bsize; 
    return true;
  }

  void SetCacheSize(int csize) { _cachesize = csize; }

  bool SetAssoc(int assoc) 
  {
    if (!IsPow2(assoc)) {
      fprintf (stderr,
	       "Associativity cannot be set to %d, not a power of 2\n",
	       assoc);
      return false;
    }
    _assoc = assoc; 
    return true;
  }


  void SetNumCPUs(int ncpus) { _numcpus = ncpus; }

  void SetStatsFile(FILE *statsfp) { _statsfp = statsfp; }

  CacheMaker(FILE *statsfp = stdout, 
	     int bsize = 32, int csize = 16384, int assoc = 4, 
	     protocol_t protocol = MSI_SMPCache, int ncpus = 1)
    : _statsfp(statsfp), 
    _cachesize(csize), _numcpus(ncpus), 
    _cfac(NULL)
    {
      if (!this->SetBlockSize(bsize)) _blocksize=32;
      if (!this->SetAssoc(assoc)) _assoc = 4;
      if (!this->SetProtocol(protocol)) _protocol = MSI_SMPCache;
    }

  MultiCacheSim* Make();
}; 

#endif
