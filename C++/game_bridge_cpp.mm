//
//  game_bridge_cpp.m
//  zweidee-iOS
//
//  Created by Wolfgang Schulz on 08.03.18.
//  Copyright © 2018 Apple. All rights reserved.
//

#import <Foundation/Foundation.h>
// http://www.drdobbs.com/cpp/interoperating-between-c-and-objective-c/240165502
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
/*
- (void)gal_init : (nonnull NSData *)p_data // wrapper
{
  p_galgame->init();
  _data = p_data; // set pointer to member variable
}
*/
///- (void)gal_init : (nonnull uint8_t *)p_imgdata // wrapper
- (void)gal_init // wrapper
{
  p_galgame->fbuf2d.width = 500;
  p_galgame->fbuf2d.height = 500;
  p_galgame->fbuf2d.imageSize = 500*500*3;
  p_galgame->init();
//  _imgData = p_imgdata; // set pointer to member variable
}

///- (void)gal_doit
- (void)gal_doit : (nonnull uint8_t *)p_imgdata // wrapper
{
  unsigned char a_data[500*500*3];
  p_galgame->doit(a_data);

  _imgData = p_imgdata; // set pointer to member variable

  NSUInteger _width = 500;
  NSUInteger _height = 500;
  for(NSUInteger y = 0; y < _height; y++)
  {
    // For every column of the current row
    for(NSUInteger x = 0; x < _width; x++)
    {
      NSUInteger dstPixelIndex = 4 * (y * _width + x);
      NSUInteger srcPos = 3 * (y * _width + x);
      
      // Copy BGR channels from the source to the destination
      // Set the alpha channel of the destination pixel to 255
      _imgData[dstPixelIndex + 0] = a_data[srcPos];
      _imgData[dstPixelIndex + 1] = a_data[srcPos+1];
      _imgData[dstPixelIndex + 2] = a_data[srcPos+2];
      _imgData[dstPixelIndex + 3] = 255;
    }
  }
}
- (void)dealloc
{
  delete p_galgame;
//  [super dealloc];
}
@end

