//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_OUTPUT_H
#define LIGHTSD_OUTPUT_H

#include "buffer.h"
#include "algorithm.h"

class Output {

public:

    template<typename DataFormat>
    void draw(AbstractBaseBuffer<DataFormat>& buffer) {
        static_assert(always_fail<DataFormat>::value, "Please implement draw fro your type.");
    }
};


#endif //LIGHTSD_OUTPUT_H
