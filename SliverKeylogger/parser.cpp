/*
 * Cobalt Strike 4.X BOF compatibility layer
 * -----------------------------------------
 * The whole point of these files are to allow beacon object files built for CS
 * to run fine inside of other tools without recompiling.
 *
 * Built off of the beacon.h file provided to build for CS.
 */

#include "pch.h"
#include "parser.h"


void BeaconDataParse(datap* parser, char* buffer, int size) {
    if (parser == NULL) {
        return;
    }
    parser->original = buffer;
    parser->buffer = buffer;
    parser->length = size - 4;
    parser->size = size - 4;
    parser->buffer += 4;
    return;
}

int BeaconDataInt(datap* parser) {
    int32_t fourbyteint = 0;
    if (parser->length < 4) {
        return 0;
    }
    memcpy(&fourbyteint, parser->buffer, 4);
    parser->buffer += 4;
    parser->length -= 4;
    return (int)fourbyteint;
}

short BeaconDataShort(datap* parser) {
    int16_t retvalue = 0;
    if (parser->length < 2) {
        return 0;
    }
    memcpy(&retvalue, parser->buffer, 2);
    parser->buffer += 2;
    parser->length -= 2;
    return (short)retvalue;
}

int BeaconDataLength(datap* parser) {
    return parser->length;
}

char* BeaconDataExtract(datap* parser, int* size) {
    uint32_t length = 0;
    char* outdata = NULL;
    /*Length prefixed binary blob, going to assume uint32_t for this.*/
    if (parser->length < 4) {
        return NULL;
    }
    memcpy(&length, parser->buffer, 4);
    parser->buffer += 4;

    outdata = parser->buffer;
    if (outdata == NULL) {
        return NULL;
    }
    parser->length -= 4;
    parser->length -= length;
    parser->buffer += length;
    if (size != NULL && outdata != NULL) {
        *size = length;
    }
    return outdata;
}

