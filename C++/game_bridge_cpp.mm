//
//  game_bridge_cpp.m
//  zweidee-iOS
//
//  Created by Wolfgang Schulz on 08.03.18.
//  Copyright © 2018 Apple. All rights reserved.
//

#import <Foundation/Foundation.h>

// a) concept:  http://www.drdobbs.com/cpp/interoperating-between-c-and-objective-c/240165502
// b) how to access props, ... https://stackoverflow.com/questions/19229574/calling-c-from-objective-c

#include "game_bridge_cpp.h"

@implementation ObjC

- (id)init
{
  self=[super init];
  if (self)
  {
    p_galgame = new galaga::CGalaga();
  }
  return self;
}

- (void)gal_init // wrapper
{
  _w = 64;
  _h = 64;
  _wDst = 512;
  _Blocksize = 8; // 512 / 64 = 8
  
  p_galgame->fbuf2d.width = _w;
  p_galgame->fbuf2d.height = _h;
  p_galgame->fbuf2d.imageSize = _w*_h*3;
  p_galgame->init();
  
//  p_galgame->spacecraft.box.y = 45;//60-1;
}

- (void)gal_doit : (nonnull uint8_t *)p_imgdata : (int)x // wrapper
{
  
//  p_galgame->spacecraft.box.x=32+x;
//  p_galgame->spacecraft.box.x+=x;
  signed int tmp = p_galgame->spacecraft.box.x+x;
  if (tmp < 0) p_galgame->spacecraft.box.x=0;
  else if (tmp > (_w-p_galgame->spacecraft.box.w)) p_galgame->spacecraft.box.x=_w-p_galgame->spacecraft.box.w;
  else p_galgame->spacecraft.box.x = tmp;
  
  unsigned char a_data[_w*_h*3];
  memset(a_data, 0, p_galgame->fbuf2d.imageSize); // clear

  p_galgame->doit(a_data);

  _imgData = p_imgdata; // set pointer to member variable

  NSUInteger _width = _w;
  NSUInteger _height = _h;
  for(NSUInteger y = 0; y < _height; y++)
  {
    // For every column of the current row
    for(NSUInteger x = 0; x < _width; x++)
    {
      NSUInteger srcPos = 3 * (y * _width + x);
      for(NSUInteger yDst = 0; yDst < _Blocksize; yDst++)
      {
        // For every column of the current row
        for(NSUInteger xDst = 0; xDst < _Blocksize; xDst++)
        {
          NSUInteger dstPixelIndex = 4 * ((y*_Blocksize+yDst) * _wDst + (x*_Blocksize+xDst));
          // Copy BGR channels from the source to the destination
          // Set the alpha channel of the destination pixel to 255
          _imgData[dstPixelIndex + 0] = a_data[srcPos];
          _imgData[dstPixelIndex + 1] = a_data[srcPos+1];
          _imgData[dstPixelIndex + 2] = a_data[srcPos+2];
          _imgData[dstPixelIndex + 3] = 255;
        }
      }
    }
  }
}

- (void)gal_fire
{
  p_galgame->fire();
}

- (void)gal_reset
{
  p_galgame->init();
}

- (void)dealloc
{
  delete p_galgame;
//  [super dealloc];
}
@end

