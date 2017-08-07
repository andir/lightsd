#pragma once

#include <cassert>
#include <algorithm>
#include <cmath>
#include "hsv.h"
#include "rgb.h"

namespace {
    template<typename T>
    struct always_fail {
        enum {
            value = false
        };
    };
}

namespace algorithm {

    template<typename ColorType>
    inline constexpr ColorType LinearColor(const float step) {
        static_assert(always_fail<ColorType>::value, "No LinearColor implemented for this type yet.");
        return ColorType{};
    }

    template<>
    inline constexpr HSV LinearColor<HSV>(const float step) {
        return HSV{360.f * step, 1, 1};
    }

    template<typename ColorSystem>
    struct Mask {
        ColorSystem operator()(const float factor, const ColorSystem &color) const {
            static_assert(
                    always_fail<ColorSystem>::value, "Please implement masking for your type");
        }
    };

    template<>
    struct Mask<HSV> {
        inline constexpr HSV operator()(const float factor, const HSV &color) const {
            //assert(factor <= 1.0f);
            return HSV{
                    color.hue,
                    color.saturation,
                    std::min(HSV::value_type(color.value * factor), 1.0f),
            };
        }
    };

    template<>
    struct Mask<RGB> {
        inline constexpr RGB operator()(const float factor, const RGB &color) const {
            assert(factor <= 1.0f);
            return RGB{
                    RGB::value_type(color.red * factor),
                    RGB::value_type(color.green * factor),
                    RGB::value_type(color.blue * factor),
            };
        }
    };


    template<typename BufferType>
    void MaskBuffer(const std::vector<float> &mask, BufferType &data) {
        using ValueType = typename BufferType::value_type;
        assert(mask.size() <= data.count());

        std::transform(mask.begin(), mask.end(), data.begin(), data.begin(), Mask<ValueType>());
    }

    template<typename BufferType>
    void MaskBuffer(const float mask, BufferType &data) {
        using ValueType = typename BufferType::value_type;
        std::transform(data.begin(), data.end(), data.begin(), [mask](auto &e) {
            Mask<ValueType> m;
            return m(mask, e);
        });
    }

    template<typename Iterator>
    void Fade(Iterator& from, Iterator& to) {
        using value_type = typename Iterator::value_type;
        const size_t length = (from > to) ? from - to : to - from;

        if (length == 0)
            return;

        const float step = 1.0f / length;
        size_t i = 0;
        Mask<value_type> m;
        for (auto it = from; it != to; ++it, ++i) {
            const float factor = step * i;
            *it = m(factor, *it);
        }
    };

    template<typename Container>
    void PartialMaskBuffer(const std::vector<float> &mask, Container &data, const size_t offset) {
        if (mask.size() == 0 || data.size() == 0 || mask.size() > data.size()) return;

        using value_type = typename Container::value_type;
        auto data_begin = data.begin() + offset;
        if (data_begin > data.end()) {
            data_begin = data.end();
        }

        auto mask_begin = mask.begin();
        auto mask_end = mask.end();

        // mask is bigger then the remaining part of the
        // data buffer it should be applied to
        if (mask.size() >= (data.size() - offset)) {
            auto new_mask_end = mask_begin + (data.count() - offset);
            mask_end = new_mask_end - 1;
        }

        assert(size_t(mask_end - mask_begin) <= data.count() - offset);

        std::transform(
                mask_begin, mask_end, data_begin, data_begin, Mask<value_type>()
        );
    }

    template<typename Iterator>
    void PartialMaskBuffer(const float mask, Iterator from, Iterator to) {
        using value_type = typename Iterator::value_type;
        std::transform(from, to, from, [mask](auto &e) {
            Mask<value_type> m;
            return m(mask, e);
        });
    }


    template<typename ColorTypeA, typename ColorTypeB>
    ColorTypeB convert(ColorTypeA &a) {
        static_assert(always_fail<ColorTypeA>::value, "not implemented.");
        return ColorTypeB{};
    };

    inline RGB convert(HSV &a) {
        return a.toRGB();
    };

    // conversion from same to same should be a noop
    template<typename T1, typename T2>
    inline T2 &convert(typename std::enable_if<std::is_same<T1, T2>::value, T2>::type &a) {
        return a;
    };


    template<typename T>
    struct cubic_add {
        inline T c(const T t1, const T t2, const T max) const {
            const T tmp = t1 + t2;
            return fmod(tmp, max);
        }
    };


    template<typename T>
    struct cubic_sub {
        inline T c(const T t1, const T t2, const T max) const {
            const T tmp = t1 - t2;
            return fmod(tmp, max);
        }
    };


    template<typename ContainerType>
    void initRainbow(ContainerType &data) {
        using ValueType = typename ContainerType::value_type;
        size_t steps = data.count();
        float stepDelta = 1.0f / steps;
        size_t i = 0;
        std::generate(data.begin(), data.end(), [stepDelta, &i]() {
            return algorithm::LinearColor<ValueType>(stepDelta * i++);
        });

    }

    template<typename ContainerType, typename ColorType>
    void initSolidColor(ContainerType &data, ColorType color) {
        std::fill(data.begin(), data.end(), color);
    };

    template<typename Iterator, typename ColorType>
    void initSolidColor(Iterator start, Iterator stop, ColorType color) {
        std::fill(start, stop, color);
    };
};
