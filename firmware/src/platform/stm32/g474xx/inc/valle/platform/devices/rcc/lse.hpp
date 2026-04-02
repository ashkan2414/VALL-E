#pragma once

#include "valle/platform/core.hpp"
#include "valle/platform/devices/power.hpp"
#include "valle/platform/hardware/rcc/lse.hpp"

namespace valle::platform
{
    // =============================================================================
    // LSE OSCILLATOR DEVICE
    // =============================================================================

    // --------------------------------------------------------------------------------
    // CONFIGURATION
    // --------------------------------------------------------------------------------
    struct LseOscillatorConfig
    {
        bool                         enabled = true;
        LseOscillatorMode            mode    = LseOscillatorMode::kCrystal;
        LseOscillatorDriveCapability drive   = LseOscillatorDriveCapability::kLow;

        [[nodiscard]] constexpr uint32_t get_frequency_hz() const
        {
            return enabled ? LseOscillatorInterface::skFrequencyHz : 0U;
        }
    };

    // --------------------------------------------------------------------------------
    // DEVICE
    // --------------------------------------------------------------------------------
    template <typename T = void>
    class LseOscillator
    {
    public:
        struct Descriptor : public UniqueDeviceDescriptor
        {
        };

        using HdiT          = LseOscillatorHdi<T>;
        using DependDevices = TypeList<PowerController<>>;
        using InjectDevices = TypeList<HdiT>;

    private:
        [[no_unique_address]] DeviceRef<HdiT> m_hw;

    public:
        explicit LseOscillator(DeviceRef<HdiT>&& hardware_key) : m_hw(std::move(hardware_key))
        {
        }

        [[nodiscard]] bool init(const LseOscillatorConfig& config)
        {
            // LL_PWR_EnableBkUpAccess() must be called before enabling LSE, otherwise LSE won't start.
            // This is a quirk of the STM32G4 hardware.
            if (config.enabled)
            {
                if (m_hw->is_ready())
                {
                    // If LSE is already enabled, just update the drive capability
                    m_hw->set_drive_capability(config.drive);
                    return true;
                }

                // Set initial drive capability to high to ensure reliable startup,
                // then reduce it to the desired level after the oscillator is stable.
                m_hw->set_drive_capability(LseOscillatorDriveCapability::kHigh);
                m_hw->enable(config.mode);

                expect(wait_for_ready(HdiT::skDefaultEnableTimeoutCount), "LSE failed to become ready within timeout");

                m_hw->set_drive_capability(config.drive);

                return true;
            }

            m_hw->disable();
            expect(wait_for_not_ready(HdiT::skDefaultDisableTimeoutCount), "LSE failed to disable within timeout");

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