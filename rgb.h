//
// Created by andi on 10/2/16.
//

#ifndef LIGHTSD_RGB_H
#define LIGHTSD_RGB_H

#include <sstream>
#include <iomanip>

struct RGB {

    using value_type = uint8_t;

    value_type red;
    value_type green;
    value_type blue;


    void print() const {
        std::cout << "RGB{" << int(this->red) << ", " << int(this->green) << ", " << int(this->blue) << "}"
                  << std::endl;
    }

    std::string asHex() const {
        std::ostringstream os;
        os << '#';
        for (const auto &v : {red, green, blue}) {
            os << std::hex << std::setw(2) << std::setfill('0') << int(v);
        }
        return os.str();
    }


    static inline RGB make_RGB(const value_type R, const value_type G, const value_type B) {
        return RGB{
                R, G, B
        };
    }

    static inline RGB make_RGB(const double fR, const double fG, const double fB) {

        value_type R = value_type(255 * fR), G = value_type(255 * fG), B = value_type(255 * fB);

        return RGB{R, G, B};
    }
};


#endif //LIGHTSD_RGB_H
