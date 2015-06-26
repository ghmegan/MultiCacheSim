#include "CacheMaker.h"
#include "MSI_SMPCache.h"
#include "MESI_SMPCache.h"

MultiCacheSim* CacheMaker::Make() {

  if (this->_protocol == MSI_SMPCache) {
    this->_cfac = MSI_SMPCache_Create;
  }
  else if (this->_protocol == MESI_SMPCache) {
    this->_cfac = MESI_SMPCache_Create;
  }

  if (this->_cfac == NULL) {
    fprintf (stderr, "Could not set cache factory for protocol %d?\n",
	     this->_protocol);
    return NULL;
  }

  MultiCacheSim* mcs = new MultiCacheSim(this->_statsfp, 
					 this->_cachesize,
					 this->_assoc,
					 this->_blocksize,
					 this->_cfac);

  for (int idx = 0; idx < this->_numcpus; idx++) {
    mcs->createNewCache();
  }
  
  return mcs;
}
