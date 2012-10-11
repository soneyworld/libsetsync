//
//  bitset.h
//  syntcrie
//
//  Created by Sebastian Schildt on 18.11.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef syntcrie_bitset_h
#define syntcrie_bitset_h

//http://c-faq.com/misc/bitsets.html

#define CHAR_BIT 8

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)


#endif
