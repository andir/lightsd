//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_UDPOUTPUT_H
#define LIGHTSD_UDPOUTPUT_H

#include <boost/asio.hpp>

using boost::asio::ip::udp;


class UDPOutput : public Output {
    boost::asio::io_service io_service;
    udp::socket socket;
    udp::resolver resolver;
    udp::endpoint endpoint;


    struct dmx_header {
        char ident[8];
        unsigned int opcode;

    };

public:

    UDPOutput(const std::string destination, const std::string port) :
            socket(io_service, udp::endpoint(udp::v4(), 0)),
            resolver(io_service) {
        endpoint = *resolver.resolve({udp::v4(), destination, port});
    }

    template<typename Container>
    void draw(Container &buffer) {

        _send<Container>(buffer);
    }

private:
//    template<typename Container>
//    void _send(Container& foo) {
//        static_assert(always_fail<Container>::value, "Not implmeneted.");
//    }
//
//typename std::enable_if<std::is_base_of<HSV, typename Container::value_type>::value,
    template<typename Container>
            void _send(Container& buffer) {
        std::vector<RGB> rgbbuffer(buffer.size());
        std::transform(buffer.begin(), buffer.end(), rgbbuffer.begin(), [](const auto& e){
            return e.toRGB();
        });

        std::vector<boost::asio::const_buffer> buffers;

        buffers.push_back(boost::asio::buffer((char*)&*rgbbuffer.begin(), rgbbuffer.size() * sizeof(RGB)));

        socket.send_to(buffers, endpoint);
    }

};


#endif //LIGHTSD_UDPOUTPUT_H
