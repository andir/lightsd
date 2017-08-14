#pragma once

#include "../Buffer.h"
#include "../hsv.h"

class Output {

public:
    virtual ~Output() {};

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer) = 0;

    virtual void draw(const std::vector<HSV> &buffer) = 0;
};
