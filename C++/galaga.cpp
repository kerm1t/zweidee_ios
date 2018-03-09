//#include "stdafx.h"
#ifdef __cplusplus
#include "galaga.h"

//#include "math.h"
#include "draw_lib.h"

int galaga::CGalaga::init()
{
  a_episode_len[0] = fbuf2d.width - (2 * 10); // size of progress bar ;-)
  a_episode_active[0] = false;
  iepisode = 1;//0;
  iloopy = 0;

  spacecraft.state = spacecraft::normal;
  spacecraft.lives = 4;
  spacecraft.dir = 0;
  spacecraft.box.x = (int)(fbuf2d.width/2) - (int)7/2;
  spacecraft.box.y = 60-1;
  spacecraft.box.w = 7;
  spacecraft.box.h = 3;

  idx_shots = 0;
  // enemies:
  //   xxx
  //    x
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    a_enemies[i].state = enemy::on;
    a_enemies[i].box.x = 3 + i * 8;
    a_enemies[i].box.y = i;
    a_enemies[i].box.w = 3;
    a_enemies[i].box.h = 2;
  }

  for (int i = 0; i < NUM_SHOTS; i++)
  {
    a_shots[i].box.w = 1;
    a_shots[i].box.h = 1;
  }

  return 0;
}

// ---------------
// main game cycle
// ---------------
int galaga::CGalaga::doit(unsigned char * data)
{
  switch (iepisode)
  {
  case 0:
    if (!a_episode_active[0])
    {
      iloopy = a_episode_len[iepisode];
    }
    else
    {
//    lib.draw_progress(data);
    }
    if (iloopy >= a_episode_len[iepisode]) iepisode++;
    break;

  case 1:
//     lib.draw_arrow(data);
    draw_starfield_vert(data);
    for (unsigned int i = 0; i<spacecraft.lives; i++) fbuf2d.setPixel(data, 10 + i * 2, 5, 255, 255, 255); // bgr
    if (spacecraft.state == spacecraft::normal)
    {
      switch (spacecraft.dir)
      {
      case 0: draw_spacecraft(data); break;
      case 1: draw_spacecraft_turnleft(data); break;
      case 2: draw_spacecraft_turnright(data); break;
      }
    }
    else if (spacecraft.state == spacecraft::explode)
    {
      draw_spacecraft_explode(data);
      spacecraft.explode_counter--;
      if (spacecraft.explode_counter == 0)
      {
        spacecraft.state = spacecraft::normal;
        spacecraft.lives--; // reduce only after explosion fully executed ;-)
      }
    }
    else ;
    spacecraft.dir = 0; // set to neutral again
    move_shots();
//    collisioncheck();  // 1st Collisioncheck, after Shot-move
    draw_shots(data);
    move_enemies();
    draw_enemies(data);
    collisioncheck();  // 2nd Collisioncheck after enemies move (if test only once, shot and enemy might just bypass each other in the same cycle ;-)
    if (spacecraft.lives == 0) iepisode++;
    break;

  case 2:
    draw_gameover(data);
    break;
  }

  iloopy++;

  return 0;
}

int galaga::CGalaga::left()
{
  if (spacecraft.state == spacecraft::explode) return true;
  if (spacecraft.box.x > 0) spacecraft.box.x--;
  spacecraft.dir = 1;
  return true;
}

int galaga::CGalaga::right()
{
  if (spacecraft.state == spacecraft::explode) return true;
  if (spacecraft.box.x + spacecraft.box.w < fbuf2d.width) spacecraft.box.x++;
  spacecraft.dir = 2;
  return true;
}

int galaga::CGalaga::fire()
{
  if (iepisode == 2)
  {
    init(); // restart!
  }
  else
  {
    if (spacecraft.state == spacecraft::normal)
    {
      a_shots[idx_shots].on = true;
      a_shots[idx_shots].box.x = spacecraft.box.x + (int)(spacecraft.box.w/2);
      a_shots[idx_shots].box.y = spacecraft.box.y;
      idx_shots++;
      if (idx_shots == NUM_SHOTS) idx_shots = 0; // wrap around (ringbuffer)
    }
  }
  return true;
}

int galaga::CGalaga::draw_starfield_vert(unsigned char * data) // cheap trick ... 2do: replace with random starfield
{
  char r, g, b;

  for (unsigned int i = 0; i < fbuf2d.height; i++)
  {
    r = ((i + 1) % 3) * 255;
    g = ( i      % 3) * 255;
    b = ((i - 1) % 3) * 255;
    fbuf2d.setPixel(data, cos((float)i) *                      i, (iloopy + i) % 64, r, g, b);
    fbuf2d.setPixel(data, cos((float)i) * (fbuf2d.width - 2) - i, (iloopy + i) % 64, r, g, b);
    // funny effect:
//    fbuf2d.setPixel(data, (unsigned int)cos(i) *                      i, (iloopy + i) % 64, r, g, b);
//    fbuf2d.setPixel(data, (unsigned int)cos(i) * (fbuf2D.width - 2) - i, (iloopy + i) % 64, r, g, b);
  }
  return true;
}

int galaga::CGalaga::draw_obj(const rect box, const unsigned char * obj, unsigned char * data)
{
  const unsigned char _w = box.w;
  const unsigned char _h = box.h;
  for (unsigned char y=0; y<_h; y++)
  {
    for (unsigned char x=0; x<_w; x++)
    {
      int _x = box.x + x; // upper ...
      int _y = box.y + y; // ... left
      unsigned char iA = y*_w+x;
      if (obj[iA] > 0)
      {
        vec3 col = cols[obj[iA]];
        fbuf2d.setPixel(data, _x, _y, col.r, col.g, col.b); // bgr
      }
    }
  }
  return true;
}

//        xxx   
//     x  xox  x
//     xxxxxxxxx
int galaga::CGalaga::draw_spacecraft(unsigned char * data)
{
  draw_obj(spacecraft.box,aSpacecraft,data);
  return true;
}

int galaga::CGalaga::draw_spacecraft_turnleft(unsigned char * data)
{
  rect r = {spacecraft.box.x,spacecraft.box.y,5,3};
  draw_obj(r,aSpacecraft_turnleft,data);
  return true;
}

int galaga::CGalaga::draw_spacecraft_turnright(unsigned char * data)
{
  rect r = {spacecraft.box.x+2,spacecraft.box.y,5,3};
  draw_obj(r,aSpacecraft_turnright,data);
  return true;
}

int galaga::CGalaga::draw_spacecraft_explode(unsigned char * data)
{
  int x = spacecraft.box.x + (int)(spacecraft.box.w/2);
  int y = spacecraft.box.y + (int)(spacecraft.box.h/2);
  vec3 col = vec3(255,255,0);
  Bresenham_Circle(&fbuf2d,x-5+(rand()%10),y-5+(rand()%10),rand()%10,col,data); // several circles with differtent pos, radius
  return true;
}

int galaga::CGalaga::draw_gameover(unsigned char * data)
{
  rect r = {10,10,17,9};
  draw_obj(r,aGameover,data);
  return true;
}

int galaga::CGalaga::move_shots()
{
  for (unsigned int i = 0; i < NUM_SHOTS; i++)
  {
    if (a_shots[i].on)
    {
      a_shots[i].box.y--;
      if (a_shots[i].box.y == 0) a_shots[i].on = false;
    }
  }
  return true;
}

int galaga::CGalaga::draw_shots(unsigned char * data)
{
  unsigned char r, g, b;
  g = 255; r = b = 0;
  for (unsigned int i = 0; i < NUM_SHOTS; i++)
  {
    if (a_shots[i].on)
    {
      int x = a_shots[i].box.x;
      int y = a_shots[i].box.y;
      fbuf2d.setPixel(data, x, y, r, g, b);
    }
  }
  return true;
}

int galaga::CGalaga::draw_enemy_explosion(const unsigned int i, unsigned char * data)
{
  int x = a_enemies[i].box.x + (int)(a_enemies[i].box.w/2);
  int y = a_enemies[i].box.y + (int)(a_enemies[i].box.h/2);
  vec3 col = vec3(255,100,0);
  Bresenham_Circle(&fbuf2d,x,y,rand()%10,col,data);
  a_enemies[i].explode_counter--;
  if (a_enemies[i].explode_counter == 0) a_enemies[i].state = enemy::off;
  return true;
}

int galaga::CGalaga::draw_enemies(unsigned char * data)
{
  for (unsigned int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].state == enemy::on)
    {
      rect r = {a_enemies[i].box.x,a_enemies[i].box.y,3,2};
      draw_obj(r,aEnemy,data);
    }
    else if (a_enemies[i].state == enemy::explode)
    {
      draw_enemy_explosion(i,data);
    }
    else ;
  }
  return true;
}

int galaga::CGalaga::move_enemies()
{
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].state == enemy::on)
    {
      if ((iloopy % 2) == 1) a_enemies[i].box.y++;
      if (a_enemies[i].box.y == 64 - 2) a_enemies[i].box.y = 0;
    }
  }
  return true;
}

// https://gamedev.stackexchange.com/questions/586/what-is-the-fastest-way-to-work-out-2d-bounding-box-intersection
/*bool galaga::DoBoxesIntersect(rect a, rect b) // for center + width / height
{
  // translate (top,left,w,h) to (x,y,w,h) <--- optimize !!!
  int ax = a.x + (int)(a.w/2);
  int ay = a.y + (int)(a.h/2);
  int bx = b.x + (int)(b.w/2);
  int by = b.y + (int)(b.h/2);
  return (abs(ax - bx) * 2 < (a.w + b.w)) &&
         (abs(ay - by) * 2 < (a.h + b.h));
}
*/
bool galaga::DoBoxesIntersect(rect a, rect b) // for top, left, "right", "bottom"
{
  return ! (b.x      > (a.x+a.w)
        || (b.x+b.w) <  a.x
        ||  b.y      > (a.y+a.h)
        || (b.y+b.h) <  a.y);
}

int galaga::CGalaga::collisioncheck()
{
  // A) shots vs. enemies
  int iOff = 0;
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].state == enemy::on)
    {
      for (int j = 0; j < NUM_SHOTS; j++)
      {
        if (a_shots[j].on)
        {
          if (DoBoxesIntersect(a_enemies[i].box,a_shots[j].box))
          {
            a_enemies[i].state = enemy::explode;
            a_enemies[i].explode_counter = 50;
            a_shots[j].on = false;
          }
        }
      }
    }
    else
    {
      iOff++;
    }
  }
  if (iOff == NUM_ENEMIES)
  {
    // all shot --> next wave
    for (int i = 0; i < NUM_ENEMIES; i++)
    {
      a_enemies[i].state = enemy::on;
      // comment to get "random" enemy position after 1st wave
      a_enemies[i].box.x = 3 + i * 8;
      a_enemies[i].box.y = i;
      // comment to get "random" enemy position after 1st wave
    }
  }

  // B) enemies vs. ship
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].state == enemy::on)
    {
      if (DoBoxesIntersect(a_enemies[i].box,spacecraft.box))
      {
        spacecraft.state = spacecraft::explode;
        spacecraft.explode_counter = 100;
        a_enemies[i].state = enemy::explode;
        a_enemies[i].explode_counter = 50;
      }
    }
  }

  return true;
}
#endif // __cplusplus
