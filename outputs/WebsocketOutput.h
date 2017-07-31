//
// Created by andi on 10/4/16.
//

#ifndef LIGHTSD_WEBSOCKETOUTPUT_H_H
#define LIGHTSD_WEBSOCKETOUTPUT_H_H

#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>
#include <set>
#include <sstream>

#include <websocketpp/common/thread.hpp>

#include "../rgb.h"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

/* on_open insert connection_hdl into channel
 * on_close remove connection_hdl from channel
 * on_message queue send to all channels
 */

enum action_type {
    SUBSCRIBE,
    UNSUBSCRIBE,
    MESSAGE,
    QUIT
};

typedef std::string message_type;

struct action {
    action(action_type t, connection_hdl h) : type(t), hdl(h) {}

    action(action_type t, connection_hdl h, message_type m)
            : type(t), hdl(h), msg(m) {}

    action_type type;
    websocketpp::connection_hdl hdl;
    message_type msg;
};

class broadcast_server {
public:
    broadcast_server() : m_server() {
        websocketpp::lib::error_code ec;
        // Initialize Asio Transport
        m_server.init_asio();
        //auto endpoint = m_server.get_local_endpoint(ec);
        //endpoint.set_reuse_addr(true);
        m_server.set_reuse_addr(true);


        // Register handler callbacks
        m_server.set_open_handler(bind(&broadcast_server::on_open, this, ::_1));
        m_server.set_close_handler(bind(&broadcast_server::on_close, this, ::_1));
    }

    ~broadcast_server() {
        stop();
        std::cerr << "Stopped websocket server " << std::endl;
    }

    void run(uint16_t port) {
        // listen on specified port
        m_server.listen(port);

        // Start the server accept loop
        m_server.start_accept();

        const auto log_levels = websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect;
        m_server.set_error_channels(websocketpp::log::elevel::fatal);
        m_server.set_access_channels(log_levels);

        // Start the ASIO io_service run loop
        try {
            m_server.run();
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

    bool has_clients() const {
        return !m_connections.empty();
    }

    void stop() {
        lock_guard<mutex> guard(m_action_lock);
        const connection_hdl hdl = std::shared_ptr<void>(nullptr);

        m_actions.push(action(QUIT, hdl));
        m_action_cond.notify_one();

        if (!m_server.stopped()) {
            m_server.stop();
        }
    }


    void on_open(connection_hdl hdl) {
        {
            lock_guard<mutex> guard(m_action_lock);
            //std::cout << "on_open" << std::endl;
            m_actions.push(action(SUBSCRIBE, hdl));
        }
        m_action_cond.notify_one();
    }

    void on_close(connection_hdl hdl) {
        {
            lock_guard<mutex> guard(m_action_lock);
            //std::cout << "on_close" << std::endl;
            m_actions.push(action(UNSUBSCRIBE, hdl));
        }
        m_action_cond.notify_one();
    }

    void on_message(connection_hdl hdl, server::message_ptr msg) {
//        // queue message up for sending by processing thread
//        {
//            lock_guard<mutex> guard(m_action_lock);
//            //std::cout << "on_message" << std::endl;
//            m_actions.push(action(MESSAGE, hdl, msg));
//        }
//        m_action_cond.notify_one();
    }

    void push_message(std::string msg) {
        {
            lock_guard<mutex> guard(m_action_lock);
            const connection_hdl hdl = std::shared_ptr<void>(nullptr);
            m_actions.push(action(MESSAGE, hdl, msg));
        }
        m_action_cond.notify_one();
    }

    void process_messages() {
        while (1) {
            unique_lock<mutex> lock(m_action_lock);

            while (m_actions.empty()) {
                m_action_cond.wait(lock);
            }

            action a = m_actions.front();
            m_actions.pop();

            if (a.type == QUIT) {
                return;
            }

            lock.unlock();

            if (a.type == SUBSCRIBE) {
                lock_guard<mutex> guard(m_connection_lock);
                m_connections.insert(a.hdl);
            } else if (a.type == UNSUBSCRIBE) {
                lock_guard<mutex> guard(m_connection_lock);
                m_connections.erase(a.hdl);
            } else if (a.type == MESSAGE) {
                lock_guard<mutex> guard(m_connection_lock);

                con_list::iterator it;
                websocketpp::lib::error_code ec;

                for (it = m_connections.begin(); it != m_connections.end(); ++it) {
                    m_server.send(*it, a.msg, websocketpp::frame::opcode::text, ec);
                    if (ec) {
                        // yeah yeah.. lets move on...
                    }
                }
            } else {
                // undefined.
            }
        }
    }

private:
    typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

    server m_server;
    con_list m_connections;
    std::queue<action> m_actions;

    mutex m_action_lock;
    mutex m_connection_lock;
    condition_variable m_action_cond;
};


class WebsocketOutput : public Output {
    broadcast_server server;
    thread process_thread;
    thread accept_thread;
public:

    WebsocketOutput(int port) :
            process_thread(bind(&broadcast_server::process_messages, &server)),
            accept_thread(bind(&broadcast_server::run, &server, port)) {
    }


    ~WebsocketOutput() {
        server.stop();
        process_thread.join();
        accept_thread.join();

        std::cerr << "Stoped WebsocketOutput" << std::endl;
    }


    void draw(const AbstractBaseBuffer<HSV> &buffer) {
        _draw(buffer);
    }

    void draw(const std::vector<HSV> &buffer) {
        _draw(buffer);
    }


private:
    template<typename Container>
    void _draw(Container &buffer) {


        if (!server.has_clients())
            return;

        std::stringstream ss;


        ss << "{\"values\":[";


        size_t count = 0;
        for (const auto &e: buffer) {
            RGB v = e.toRGB();
            ss << "[" << int(v.red) << "," << int(v.green) << "," << int(v.blue) << "]";

            if (++count < buffer.size())
                ss << ",";
        }

        ss << "]}\n";

        server.push_message(ss.str());
    }


};


#endif //LIGHTSD_WEBSOCKETOUTPUT_H_H
