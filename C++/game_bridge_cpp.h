//
//  game_bridge_cpp.h
//  zweidee
//
//  Created by Wolfgang Schulz on 08.03.18.
//  Copyright © 2018 Apple. All rights reserved.
//

// a) concept:  http://www.drdobbs.com/cpp/interoperating-between-c-and-objective-c/240165502
// b) how to access props, ... https://stackoverflow.com/questions/19229574/calling-c-from-objective-c

#ifndef game_bridge_cpp_h
#define game_bridge_cpp_h

#include "galaga.h"

@interface ObjC: NSObject
{
#ifdef __cplusplus
  galaga::CGalaga * p_galgame;
#endif // __cplusplus
}
@end

// önd nö gömm' de proöperdies
@interface ObjC ()
- (void)gal_init; // wrapper
- (void)gal_doit : (nonnull uint8_t *)p_imgdata: (int)x; // wrapper
@property (nonatomic, readonly, nonnull) uint8_t *imgData;
@property (nonatomic) int w;
@property (nonatomic) int h;
@property (nonatomic) int wDst;
@property (nonatomic) uint8_t Blocksize;
@end

#endif /* game_bridge_cpp_h */
