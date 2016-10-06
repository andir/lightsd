//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_DEBUGOUTPUT_H
#define LIGHTSD_DEBUGOUTPUT_H

#include "Output.h"

class DebugOutput : public Output {
public:
    //typename ColorFormat

    template<typename Container>
    void draw(Container &buffer) {

        std::ofstream os("/tmp/test.html");

        for (const auto &e : buffer) {
            //e.print();
            e.hue;
            _drawRGB(os, e);
        }
    }

private:
    template<typename ColorModel>
    void _draw(std::ofstream &os, ColorModel &e) {
        os << "<div style=\" background-color: hsl(" << e.hue << " ,"
           << e.saturation * 100 << "%, " << e.value * 100 << "%)"
           << "; width: 100%; height: 10pt;\">" <<
           e.toRGB().asHex()
           << "</div>" << std::endl;
    }

    template<typename ColorModel>
    void _drawRGB(std::ofstream &os, ColorModel &e) {
        RGB r = e.toRGB();

//        const std::string line = [&r]() {
//            using namespace std::string_literals;
//
//            const std::string s = "<div style=\" background-color: rgba("s +
//            std::to_string(r.red) +
//            " ,"s +
//            std::to_string(r.green) +
//            ", "s +
//            std::to_string(r.blue) +
//            ",1) ; width: 100%; height: 2px;\">"s + r.asHex() + "</div>\n"s;
//            return s;
//        }();
//
//        os << line;

        os << "<div style=\" background-color: rgba(" << int(r.red) << " ,"
           << int(r.green) << ", " << int(r.blue) << ", 1)"
           << "; width: 100%; height: 2px;\">" <<
           r.asHex()
           << "</div>\n";
    }

};


#endif //LIGHTSD_DEBUGOUTPUT_H
