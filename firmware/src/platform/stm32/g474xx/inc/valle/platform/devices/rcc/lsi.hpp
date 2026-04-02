#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/hardware/rcc/lsi.hpp"

namespace valle::platform
{
    // =============================================================================
    // CONFIGURATION
    // =============================================================================
    struct LsiOscillatorConfig
    {
        bool enabled = true;

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return enabled ? LsiOscillatorInterface::skFrequencyHz : 0U;
        }
    };

    // =============================================================================
    // DEVICE
    // =============================================================================
    template <typename T = void>
    class LsiOscillator
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using HdiT          = LsiOscillatorHdi<T>;
        using InjectDevices = TypeList<HdiT>;

    private:
        [[no_unique_address]] DeviceRef<HdiT> m_hw;

    public:
        explicit LsiOscillator(DeviceRef<HdiT>&& hardware_key) : m_hw(std::move(hardware_key))
        {
        }

        [[nodiscard]] inline bool init(const LsiOscillatorConfig& config)
        {
            if (config.enabled)
            {
                m_hw->enable();

                expect(wait_for_ready(HdiT::skDefaultEnableTimeoutCount), "LSI failed to become ready within timeout");

                return true;
            }

            m_hw->disable();

            expect(wait_for_not_ready(HdiT::skDefaultDisableTimeoutCount), "LSI failed to disable within timeout");

            return true;
        }

    private:
        [[nodiscard]] static bool wait_for_ready(const uint32_t timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown([]() { return m_hw->is_ready(); }, timeout_count);
        }

        [[nodiscard]] static bool wait_for_not_ready(const uint32_t timeout_count)
        {
            return TimingContext::wait_for_with_timeout_countdown([]() -> bool { return !m_hw->is_ready(); },
                                                                  timeout_count);
        }
    };

}  // namespace valle::platform