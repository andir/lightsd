//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_UDPOUTPUT_H
#define LIGHTSD_UDPOUTPUT_H

#include <boost/asio.hpp>
#include "Output.h"
#include "../rgb.h"

using boost::asio::ip::udp;


class UDPOutput : public Output {
    boost::asio::io_service io_service;
    udp::socket socket;
    udp::resolver resolver;
    udp::endpoint endpoint;

public:

    UDPOutput(const std::string& destination, const std::string& port);

    void draw(const std::vector<HSV> &buffer);

    void draw(const AbstractBaseBuffer<HSV> &buffer);

private:
    template<typename Container>
    void _send(const Container &buffer) {
        std::vector<RGB> rgbbuffer(buffer.size());
        std::transform(buffer.begin(), buffer.end(), rgbbuffer.begin(), [](const auto &e) {
            return e.toRGB();
        });

        std::vector<boost::asio::const_buffer> buffers;

        buffers.push_back(boost::asio::buffer((char *) &*rgbbuffer.begin(), rgbbuffer.size() * sizeof(RGB)));

        socket.send_to(buffers, endpoint);
    }

};


#endif //LIGHTSD_UDPOUTPUT_H
