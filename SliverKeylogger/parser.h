/*
 * Cobalt Strike 4.X BOF compatibility layer
 * -----------------------------------------
 * The whole point of these files are to allow beacon object files built for CS
 * to run fine inside of other tools without recompiling.
 *
 * Built off of the beacon.h file provided to build for CS.
 */
#ifndef BEACON_COMPATIBILITY_H_
 /* Structures as is in beacon.h */

typedef struct {
    char * original; /* the original buffer [so we can free it] */
    char * buffer;   /* current pointer into our buffer */
    int    length;   /* remaining length of data */
    int    size;     /* total size of this buffer */
} datap;


void    BeaconDataParse(datap * parser, char * buffer, int size);
int     BeaconDataInt(datap * parser);
short   BeaconDataShort(datap * parser);
int     BeaconDataLength(datap * parser);
char *  BeaconDataExtract(datap * parser, int * size);

#endif
