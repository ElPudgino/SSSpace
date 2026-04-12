#ifndef BP_LOADER
#define BP_LOADER

#include "libs.h"
#include "ship.h"

/*!
* Ship blueprints are stored either in .sbp or .sbpb formats
* .sbp is simple and human readable, similar to json
* .sbpb is a binary file, used to store, transfer and load blueprints efficiently
*
* These files can be converted into each other in game:
* Main format is .sbpb, with options to export as/import from .sbp
*
* Notes on parsing: 
* Order of adding logicblocks is important
* 
* When saving need to sort all logic blocks in a ship by their offset
* Also cant save logicblockdata as a single byte array because alignment might mess stuff up when transfering save across devices
*
* If alignment issues can be fixed, then logicblockdata can be saved easily and
* When saving logicblocks in BP, we can just save their offset directly and use it 
*/


#endif