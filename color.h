#ifndef COLOR_H
#define COLOR_H
#include <stdint.h>

class Color{
public:
    Color();
    Color(uint8_t r, uint8_t g, uint8_t b);

    uint8_t red, green, blue;

    void set_color(uint8_t r, uint8_t g, uint8_t b);
};
#endif // COLOR_H
