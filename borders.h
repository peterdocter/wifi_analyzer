#ifndef BORDERS_H
#define BORDERS_H

#include <math.h>
#include <cfloat>
#include <tiffio.h>
#include <string.h>
#include <stdlib.h>


class Borders{
public:
    int max, min;
    int** border;
    Borders();
    Borders(int new_min, int new_max);
    void set_new_borders(int new_min, int new_max);
};

#endif // BORDERS_H
