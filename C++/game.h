#pragma once

#include "zweidee_types.h"
#ifdef __cplusplus

namespace game
{
  class CGame
  {
  public:
    FrameBuf2D fbuf2d;

#define NUM_EPISODES 3
    char a_episode_len[NUM_EPISODES];
    bool a_episode_active[NUM_EPISODES];

    unsigned int iepisode;
    int iloopy;

    virtual int init() = 0;
    virtual int doit(unsigned char * data) = 0;
    // keys
    virtual int left() = 0;
    virtual int right() = 0;
    virtual int fire() = 0;
  };
}
#endif // __cplusplus
