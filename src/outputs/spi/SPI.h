//
// Created by andi on 3/15/17.
//

#ifndef LIGHTSD_I2C_H
#define LIGHTSD_I2C_H

extern "C" {
#include <linux/spi/spidev.h>
};

#include <cstdint>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iostream>
#include <cstring>

struct SPIError : public std::exception {
    const std::string s;

    explicit SPIError(const std::string& s) : s(s) {}

    const char *what() const throw() {
        return s.c_str();
    }
};

class SPI {

    enum {
        BITS_PER_WORD = 8
    };

    const std::string device;
    const uint32_t speed;
    const uint8_t mode;

    int fd;

public:
    SPI(const std::string& device = "/dev/spidev.0", const uint32_t speed = 3200000, const uint8_t mode = 0) :
            device(device), speed(speed), mode(mode), fd(open(device.c_str(), O_RDWR)) {
        init();
    }

    void init() {
        int ret;
        ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
        if (ret == -1) {
            throw SPIError("Can't set write mode");
        }
        uint8_t set_mode;
        ret = ioctl(fd, SPI_IOC_RD_MODE32, &set_mode);
        if (ret == -1) {
            throw SPIError("Can't read mode");
        }
        if (set_mode != mode) {
            throw SPIError("Configured mode isn't the mode I set");
        }

        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1) {
            throw SPIError("Can't set WR Speed");
        }
        uint32_t set_speed;
        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &set_speed);
        if (ret == -1) {
            throw SPIError("Can't read the SPI speed");
        }
        if (set_speed != speed) {
            throw SPIError("Configured speed not set.");
        }

        static const uint8_t bits = BITS_PER_WORD;
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1) {
            throw SPIError("Can't set bits per word");
        }
        uint8_t set_bits;
        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &set_bits);
        if (ret == -1) {
            throw SPIError("Failed to read bits per word");
        }
        if (set_bits != bits) {
            throw SPIError("Bits per word does not match");
        }
    }

    ~SPI() {
        if (fd > 0) {
            close(fd);
        }
    }

    void write(const uint8_t *buffer, const size_t len) {
        if (buffer == nullptr) return;
        if (len == 0) return;

        struct spi_ioc_transfer tr;
        tr.tx_buf = (unsigned long) buffer;
        tr.rx_buf = 0;
        tr.len = len;
        tr.delay_usecs = 0;
        tr.speed_hz = speed;
        tr.bits_per_word = BITS_PER_WORD;

        int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1) {
            std::string msg = "Failed to send message";
            msg += strerror(errno);
            throw SPIError(msg);
        }
    }
};


#endif //LIGHTSD_I2C_H
