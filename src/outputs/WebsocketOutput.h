#pragma once

#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>
#include <set>
#include <sstream>
#include <variant>
#include <utility>
#include <memory>

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
    MQTT_MESSAGE,
    QUIT
};

typedef typename std::variant<std::vector<uint8_t>, std::pair<std::string, std::string> > message_type;

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
        m_server.set_http_handler(bind(&broadcast_server::on_http, this, ::_1));
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

    void on_http(connection_hdl hdl) {
            server::connection_ptr con = m_server.get_con_from_hdl(hdl);
            const std::string filename = con->get_resource();
            m_server.get_alog().write(websocketpp::log::alevel::app, "http request1:" + filename);
            if (filename == "/") {
               std::string s = R"raw(<!DOCTYPE html>
<html>
<head>
</head>
<body>
<canvas id="canvas" width="256" height="256"></canvas>
</table>
<script>
  (function () {
    var ele = document.getElementById('canvas');
    var ctx = ele.getContext('2d');

    var ws = new WebSocket("ws://" + location.host + "/stream");
    ws.binaryType = 'arraybuffer';
    
    ws.onopen = function () {
      console.log('Opened');
    }

    ws.onmessage = function (event) {
      // Interpret data as RGB-pixel array
      var data = new Uint8Array(event.data);
      var pixels = event.data.byteLength / 3;
      if (ele.width != pixels) {
        ele.width = pixels;
      }
      // Shifting down
      ctx.putImageData(ctx.getImageData(0, 0, pixels, pixels -1), 0, 1);
      // Drawing next line
      img = ctx.createImageData(pixels, 1);
      for (i = 0; i <= pixels; i++) {
        img.data[i * 4 + 0] = data[i * 3 + 0];
        img.data[i * 4 + 1] = data[i * 3 + 1];
        img.data[i * 4 + 2] = data[i * 3 + 2];
        img.data[i * 4 + 3] = 255;
      }

      ctx.putImageData(img, 0, 0);
    }
  })();
</script>
</body>
</html>)raw";
               con->set_body(s);
               con->set_status(websocketpp::http::status_code::ok);
            } else {
               const std::string s = "<h1>Error 404</h1>";
               con->set_body(s);
               con->set_status(websocketpp::http::status_code::not_found);
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

    void push_message(std::vector<uint8_t>& msg) {
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
            } else if (a.type == MQTT_MESSAGE) {
                lock_guard<mutex> guard(m_connection_lock);

                auto [key, value] = std::get<std::pair<std::string, std::string> >(a.msg);

                auto message = "{\"key\":\""+ key + "\",\"value\":\"" + value + "\"}";

                for (con_list::iterator it = m_connections.begin(); it != m_connections.end(); ++it) {
                    m_server.send(*it, message, websocketpp::frame::opcode::text);
                }

            } else if (a.type == MESSAGE) {
                lock_guard<mutex> guard(m_connection_lock);

                con_list::iterator it;
                websocketpp::lib::error_code ec;

                auto bytes = std::get<std::vector<uint8_t> >(a.msg);
                for (it = m_connections.begin(); it != m_connections.end(); ++it) {
                    m_server.send(*it, static_cast<void*>(bytes.data()), bytes.size(), websocketpp::frame::opcode::binary, ec);
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

    explicit WebsocketOutput(int port) :
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

        std::vector<uint8_t> packet(buffer.size() * 3);
        size_t i = 0;
        for (const auto &e: buffer) {
            RGB v = e.toRGB();
            packet[i*3+0] = v.red;
            packet[i*3+1] = v.green;
            packet[i*3+2] = v.blue;
            ++i;
        }

        server.push_message(packet);
    }


};
