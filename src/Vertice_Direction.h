#pragma once
#include <iostream>
#include <lodepng.h>
#include <vector>
#include <cstdint>
#include <string>
#include<limits>
#include <algorithm>

enum position
{
    l= 0,
    r = 1,
    down = 2,
    no = 3,
};

struct Vertice
    {
    bool visited;
    int path_weight;
    int own_weight;
    position previous;
};