//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_OUTPUT_H
#define LIGHTSD_OUTPUT_H

#include "../Buffer.h"
#include "../hsv.h"

class Output {

public:
    virtual ~Output() {};

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer) = 0;

    virtual void draw(const std::vector <HSV> &buffer) = 0;
};


#endif //LIGHTSD_OUTPUT_H
