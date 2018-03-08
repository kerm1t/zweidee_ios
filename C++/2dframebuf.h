// --------------
//
// 2d framebuffer
//
// --------------

#ifndef framebuf2d_hpp
#define framebuf2d_hpp
#ifdef __cplusplus
//#include "stdafx.h"
//#pragma once

//#define GLEW_STATIC
//#include <GL/glew.h>
//#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include <stdio.h>


struct vec3 {
  vec3(float _xr, float _yg, float _zb) : x(_xr), y(_yg), z(_zb) {}
  union
  {
    struct {  float x; float y; float z; };
    struct {  float r; float g; float b; };
  };
  
};
//typedef glm::vec3 vec3;



class FrameBuf2D // which is actually an animated (OpenGL-)Texture
{
public:
  unsigned int width, height;
  unsigned int imageSize;   // = width*height*3

  /*
  (0,0): rgb (1,0): rgb (2,0): rgb
  */
  void setPixel(unsigned char * data, const unsigned int x, const unsigned int y, const unsigned char r, const unsigned char g, const unsigned char b)
  {
    unsigned int _y = height - 1 - y; // hack, texture upside down? 2do: check!!
//    unsigned int _y = y;
    unsigned int pos = 3 * (_y * width + x);
//    assert(pos < imageSize);
    if (pos >= imageSize) return;
    if (x > width) return; // test < 0 ?? <-- need signed then
    data[pos]     = b; // r ?
    data[pos + 1] = g;
    data[pos + 2] = r; // b ?
  }
/*
  GLuint framebuf2D() // purpose: get texture-ID
  {
    // Actual RGB data
    unsigned char * data;

    width  = 64;
    height = 64;
    imageSize = width*height*3;

    // Create a buffer
    data = new unsigned char[imageSize];
    memset(data, 0, imageSize); // clear

    //
    //  draw some stuff
    //
    for (unsigned int i = 1; i < (height-1); i++)
    {
      char r, g, b;
      r = ((i + 1) % 3) * 255;
      g = ( i      % 3) * 255;
      b = ((i - 1) % 3) * 255;
      setPixel(data,       i, i, r, g, b);
      setPixel(data, width-i, i, r, g, b);
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);   // hier gibt es Schwierigkeiten mit .bmp,

    delete data;

    return textureID;
  }*/
};
#endif // __cplusplus
#endif // framebuf2d_hpp
