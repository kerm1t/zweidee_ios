#pragma once
#ifdef __cplusplus
namespace zweidee
{

  struct point
  {
    unsigned int x;
    unsigned int y;
    point(const unsigned int _x, const unsigned int _y) : x(_x), y(_y) {}
  };

}
#endif // __cplusplus
