// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "common/common_funcs.h"
#include "common/common_types.h"
#include "core/hid/hid_types.h"
#include "core/hle/service/hid/controllers/controller_base.h"
#include "core/hle/service/hid/ring_lifo.h"

namespace Core::HID {
class EmulatedConsole;
} // namespace Core::HID

namespace Service::HID {
class TouchScreen final : public ControllerBase {
public:
    explicit TouchScreen(Core::HID::HIDCore& hid_core_, u8* raw_shared_memory_);
    ~TouchScreen() override;

    // Called when the controller is initialized
    void OnInit() override;

    // When the controller is released
    void OnRelease() override;

    // When the controller is requesting an update for the shared memory
    void OnUpdate(const Core::Timing::CoreTiming& core_timing) override;

    void SetTouchscreenDimensions(u32 width, u32 height);

private:
    static constexpr std::size_t MAX_FINGERS = 16;

    // This is nn::hid::TouchScreenState
    struct TouchScreenState {
        s64 sampling_number{};
        s32 entry_count{};
        INSERT_PADDING_BYTES(4); // Reserved
        std::array<Core::HID::TouchState, MAX_FINGERS> states{};
    };
    static_assert(sizeof(TouchScreenState) == 0x290, "TouchScreenState is an invalid size");

    struct TouchSharedMemory {
        // This is nn::hid::detail::TouchScreenLifo
        Lifo<TouchScreenState, hid_entry_count> touch_screen_lifo{};
        static_assert(sizeof(touch_screen_lifo) == 0x2C38, "touch_screen_lifo is an invalid size");
        INSERT_PADDING_WORDS(0xF2);
    };
    static_assert(sizeof(TouchSharedMemory) == 0x3000, "TouchSharedMemory is an invalid size");

    TouchScreenState next_state{};
    TouchSharedMemory* shared_memory = nullptr;
    Core::HID::EmulatedConsole* console = nullptr;

    std::array<Core::HID::TouchFinger, MAX_FINGERS> fingers{};
    u32 touchscreen_width;
    u32 touchscreen_height;
};
} // namespace Service::HID
