#include "color.h"
#include "stdio.h"

Color::Color(){
    red=0;
    green=0;
    blue=0;
    }

Color::Color(uint8_t r, uint8_t g, uint8_t b){
    red=r;
    green=g;
    blue=b;
}

void Color::set_color(uint8_t r, uint8_t g, uint8_t b){
    red=r;
    green=g;
    blue=b;
}

