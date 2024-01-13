#pragma once

#include <cstdint>

namespace ht {
    namespace color {
        template <uint8_t R = 0, uint8_t G = 0, uint8_t B = 0, uint8_t A = 0>
        struct rgba_color {
            uint8_t red { R };
            uint8_t green { G };
            uint8_t blue { B };
            uint8_t alpha { A };
        };

        template <uint8_t R = 0, uint8_t G = 0, uint8_t B = 0>
        using rgb_color = rgba_color<R, G, B, 255>;

    } // namespace color

    namespace color {
        using red = rgb_color<255, 0, 0>;
        using green = rgb_color<0, 255, 0>;
        using blue = rgb_color<0, 255, 0>;

        using black = rgb_color<0, 0, 0>;
        using withe = rgb_color<255, 255, 255>;

    } // namespace color

    namespace color {
        namespace cn {};

        namespace jp {}
    } // namespace color

} // namespace ht