//
// Created by andi on 10/4/16.
//

#ifndef LIGHTSD_DUPLICATIONOUTPUT_H
#define LIGHTSD_DUPLICATIONOUTPUT_H

template<typename Output1, typename Output2>
class DuplicationOutput : public Output {
    Output1 &out1;
    Output2 &out2;
public:
    DuplicationOutput(Output1 &o1, Output2 &o2) : out1(o1), out2(o2) {}


    void draw(const std::vector <HSV> &e) {
        _draw(e);
    }

    void draw(const AbstractBaseBuffer <HSV> &e) {
        _draw(e);
    }

private:
    template<typename Container>
    void _draw(const Container &buffer) {
        out1.draw(buffer);
        out2.draw(buffer);
    }

};


#endif //LIGHTSD_DUPLICATIONOUTPUT_H
