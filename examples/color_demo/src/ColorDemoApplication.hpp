#pragma once

#include "engine/Application.hpp"

// Concrete Application for this demo. All it does is decide, in onInit(),
// which windows exist and what each one renders with -- it has no per-frame
// or per-event behavior of its own, so onEvent()/onTick() are left at the
// engine's no-op defaults.
class ColorDemoApplication final : public Application {
protected:
    void onInit() override;
};
