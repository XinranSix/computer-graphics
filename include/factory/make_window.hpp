#pragma once

#include "log/log.h"
#include <memory>

namespace ht {
    template <typename WINDOW, typename IMGUIRENDERER>
    std::unique_ptr<WINDOW> make_window(int width, int height,
                                        std::string title) {
        ht::Log::init();
        std::unique_ptr<WINDOW> window { std::make_unique<WINDOW>(width, height,
                                                                  title) };
        window->setImGuiRender(new IMGUIRENDERER { window.get() });
        window->init();
        return window;
    }
} // namespace ht
