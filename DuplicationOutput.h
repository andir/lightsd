//
// Created by andi on 10/4/16.
//

#ifndef LIGHTSD_DUPLICATIONOUTPUT_H
#define LIGHTSD_DUPLICATIONOUTPUT_H

template<typename Output1, typename Output2>
class DuplicationOutput {
    Output1& out1;
    Output2& out2;
public:
    DuplicationOutput(Output1& o1, Output2& o2) : out1(o1), out2(o2) {}


    template<typename Collection>
            void draw(const Collection& e) {
        out1.draw(e);
        out2.draw(e);
    }

};


#endif //LIGHTSD_DUPLICATIONOUTPUT_H
