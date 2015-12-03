#include "point.h"
#include "color.h"
#include "math.h"

Point::Point(int xx, int yy){
    x=xx;
    y=yy;
    sig = -MAX_ABS_SIG;
    color.set_color(255,0,0);
}
Point::Point(int xx, int yy, int cur_sig){
    sig = cur_sig;
    x=xx;
    y=yy;
    color.set_color(255,0,0);
}
Point::Point(){
    x=0;
    y=0;
    sig = -MAX_ABS_SIG;
    color.set_color(0,0,0);
}
Point::Point(int xx, int yy, uint8_t r, uint8_t g, uint8_t b){
    x=xx;
    y=yy;
    sig = -MAX_ABS_SIG;
    color.set_color(r,g,b);
}

Point::Point(const Point *p){
    x=p->x;
    y=p->y;
    sig = p->sig;
    color.set_color(p->color.red,
                  p->color.green,
                  p->color.blue);
}
void Point::set_color(uint8_t r, uint8_t g, uint8_t b){
    color.set_color(r,g,b);
}

void Point::set_coords(int x, int y){
    this->x = x;
    this->y = y;
}

void Point::set_sig(int signal){
    sig = signal;
}

int Point::operator !=(int a){
    if(sqrt(x*x+y*y)-a != 0) return 1;
    return 0;
}
int Point::operator !=(Point a){
    if(a.x!=this->x || a.y!=this->y) return 1;
    return 0;
}
int Point::operator ==(Point b){
    if (this->x== b.x && this->y == b.y) return 1;
    return 0;
}

float Point::dist(Point b){
    return sqrt((this->x - b.x)*(this->x - b.x) + (this->y - b.y)*(this->y - b.y));
}

