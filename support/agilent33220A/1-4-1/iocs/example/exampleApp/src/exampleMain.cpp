/* This file was automatically generated on Mon 05 Nov 2012 14:19:21 GMT from
 * source: /dls_sw/prod/R3.14.11/support/agilent33220A/1-4-1/etc/makeIocs/example.xml
 * 
 * *** Please do not edit this file: edit the source file instead. ***
 *  */
#include "epicsExit.h"
#include "epicsThread.h"
#include "iocsh.h"

int main(int argc, char *argv[])
{
    if(argc>=2) {
        iocsh(argv[1]);
        epicsThreadSleep(.2);
    }
    iocsh(NULL);
    epicsExit(0);
    return 0;
}
