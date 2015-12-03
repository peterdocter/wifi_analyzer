#ifndef POINT_H
#define POINT_H
#include <color.h>
#define uint32 uint32_t

#define MAX_ABS_SIG 99

class Point{
public:
    int x,y;
    Color color;
    int sig;
    Point(int xx, int yy);
    Point(int xx, int yy, int cur_sig);
    Point(int xx, int yy, uint8_t r, uint8_t g, uint8_t b);
    Point();
    Point(const Point *p);
    void set_color(uint8_t r, uint8_t g, uint8_t b);
    void set_sig(int signal);
    void set_coords(int x, int y);
    int operator !=(int a);
    int operator !=(Point b);
    int operator ==(Point b);
    float dist(Point b);
};


#endif // POINT_H
