#ifndef ISISDAEDRIVER_H
#define ISISDAEDRIVER_H
 
#include "asynPortDriver.h"

class isisdaeInterface;

class isisdaeDriver : public asynPortDriver 
{
public:
    isisdaeDriver(isisdaeInterface* iface, const char *portName);
 	static void pollerThreadC(void* arg);
                
    // These are the methods that we override from asynPortDriver
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
	virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
	virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason);
	virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
    virtual asynStatus readFloat64Array(asynUser *pasynUser, epicsFloat64 *value, size_t nElements, size_t *nIn);
    virtual asynStatus readInt32Array(asynUser *pasynUser, epicsInt32 *value, size_t nElements, size_t *nIn);

private:

    int P_GoodFrames; // int
    int P_RawFrames; // int
	#define FIRST_ISISDAE_PARAM P_GoodFrames
	#define LAST_ISISDAE_PARAM P_RawFrames

	isisdaeInterface* m_iface;
	
	void pollerThread();
	
	template<typename T> asynStatus writeValue(asynUser *pasynUser, const char* functionName, T value);
    template<typename T> asynStatus readValue(asynUser *pasynUser, const char* functionName, T* value);
    template<typename T> asynStatus readArray(asynUser *pasynUser, const char* functionName, T *value, size_t nElements, size_t *nIn);

};

#define NUM_ISISDAE_PARAMS (&LAST_ISISDAE_PARAM - &FIRST_ISISDAE_PARAM + 1)

#define P_GoodFramesString	"GOODFRAMES"
#define P_RawFramesString	"RAWFRAMES"

#endif /* ISISDAEDRIVER_H */
