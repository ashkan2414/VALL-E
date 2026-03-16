#pragma once

#include <cmath>
#include <cstdint>
#include <numbers>

#include "valle/base/system_build/device.hpp"
#include "valle/math/angle.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
{
    // =========================================================================
    // Encoder Interface
    // =========================================================================

    /**
     * @brief CRTP Base for Encoder Module Interface.
     */
    template <typename TDerived, typename TConfig>
    class AMT10xModuleEncoderInterfaceX
    {
    public:
        using ConfigT = TConfig;

        [[nodiscard]] bool init(const ConfigT& config)
        {
            return static_cast<TDerived*>(this)->init_impl(config);
        }

        void enable()
        {
            static_cast<TDerived*>(this)->enable_impl();
        }

        void disable()
        {
            static_cast<TDerived*>(this)->disable_impl();
        }

        /**
         * @brief Returns raw pulses from the encoder.
         */
        [[nodiscard]] auto get_count() const
        {
            return static_cast<const TDerived*>(this)->get_count_impl();
        }

        /**
         * @brief Returns the scaling factor (X1, X2, X4) from the hardware.
         */
        [[nodiscard]] auto get_counts_per_pulse() const
        {
            return static_cast<const TDerived*>(this)->get_counts_per_pulse_impl();
        }

        /**
         * @brief Reset the encoder count to zero.
         */
        void reset_count()
        {
            static_cast<TDerived*>(this)->reset_count_impl();
        }

        /**
         * @brief Set the encoder count to a specific value (useful for zeroing or homing procedures).
         *
         */
        void set_count(int64_t count)
        {
            static_cast<TDerived*>(this)->set_count_impl(count);
        }

        /**
         * @brief Optional method to read the index channel if available. This can be used for absolute positioning or
         * homing.
         *
         * @return true if the index channel is active, false otherwise.
         */
        [[nodiscard]] bool read_index_channel() const
        {
            return static_cast<const TDerived*>(this)->read_index_channel_impl();
        }
    };

    // =========================================================================
    // Module Definitions
    // =========================================================================

    /**
     * @brief AMT10x Resolution (Pulses Per Revolution) settings.
     * These correspond to the DIP switch settings on the encoder physical unit.
     */
    enum class AMT10xPPR : uint16_t
    {
        k48   = 48,
        k96   = 96,
        k100  = 100,
        k125  = 125,
        k192  = 192,
        k200  = 200,
        k250  = 250,
        k256  = 256,
        k384  = 384,
        k400  = 400,
        k500  = 500,
        k512  = 512,
        k800  = 800,
        k1000 = 1000,
        k1024 = 1024,
        k2048 = 2048,
    };

    // -------------------------------------------------------------------------
    // Module Configuration
    // -------------------------------------------------------------------------

    template <typename TEncoderInterfaceConfig>
    struct AMT10xModuleConfigX
    {
        TEncoderInterfaceConfig encoder_config{};
        float                   home_rad = 0.0F;
        bool                    reverse_direction{false};
    };

    /**
     * @brief AMT10x Encoder Module.
     *
     * @tparam TEncoderInterface Platform specific encoder driver interface.
     * @tparam tkPPR Standard PPR setting for the module instance.
     */
    template <typename TEncoderInterface, AMT10xPPR tkPPR>
    class AMT10xModuleX
    {
    public:
        using EncoderInterfaceT          = TEncoderInterface;
        static constexpr AMT10xPPR skPPR = tkPPR;

        using ConfigT = AMT10xModuleConfigX<typename EncoderInterfaceT::ConfigT>;

        using InjectDevices = typename GetInjectDevices<EncoderInterfaceT>::type;
        using DependDevices = typename GetAdditionalDependDevices<EncoderInterfaceT>::type;

    private:
        EncoderInterfaceT m_encoder{};

        // Dynamic conversion factors based on config
        uint32_t m_cpr           = static_cast<uint32_t>(skPPR) * 4;
        float    m_rad_per_count = (2.0F * std::numbers::pi_v<float>) / static_cast<float>(m_cpr);
        float    m_deg_per_count = rad2deg(m_rad_per_count);
        uint32_t m_home_count    = 0;
        bool     m_reverse       = false;

    public:
        template <typename... TArgs>
        explicit AMT10xModuleX(TArgs&&... args) : m_encoder(std::forward<TArgs>(args)...)
        {
        }

        [[nodiscard]] bool init(const ConfigT& config)
        {
            m_reverse = config.reverse_direction;

            if (!m_encoder.init(config.encoder_config))
            {
                return false;
            }

            m_cpr = static_cast<uint32_t>(skPPR) * m_encoder.get_counts_per_pulse();

            if (m_cpr == 0)
            {
                return false;
            }

            m_rad_per_count = (2.0F * std::numbers::pi_v<float>) / static_cast<float>(m_cpr);
            m_deg_per_count = rad2deg<float>(m_rad_per_count);
            m_home_count    = static_cast<uint32_t>(std::lround(config.home_rad / m_rad_per_count)) % m_cpr;

            return true;
        }

        void enable()
        {
            m_encoder.enable();
        }

        void disable()
        {
            m_encoder.disable();
        }

        /**
         * @brief Returns raw encoder counts (after X1/X2/X4 decoding).
         */
        [[nodiscard]] auto get_count() const
        {
            return m_encoder.get_count();
        }

        [[nodiscard]] int64_t get_pos_count_abs() const
        {
            auto pulses = get_count();

            if (m_reverse)
            {
                // Note: On overflow of uint32 pulses, this remains consistent
                // but absolute pos is limited by the counter wrap-around.
                return -static_cast<int64_t>(pulses);
            }

            return static_cast<int64_t>(pulses);
        }

        [[nodiscard]] auto get_pos_count_rel() const
        {
            auto pulses = get_count() % m_cpr;

            if (m_reverse)
            {
                pulses = m_cpr - pulses;
                if (pulses == m_cpr)
                {
                    pulses = 0;
                }
            }

            return pulses;
        }

        [[nodiscard]] float get_pos_norm_abs() const
        {
            return static_cast<float>(get_pos_count_abs()) / static_cast<float>(m_cpr);
        }

        [[nodiscard]] float get_pos_norm_rel() const
        {
            return static_cast<float>(get_pos_count_rel()) / static_cast<float>(m_cpr);
        }

        /**
         * @brief Returns absolute shaft pos in radians (multi-turn).
         */
        [[nodiscard]] float get_pos_rad_abs() const
        {
            return static_cast<float>(get_pos_count_abs()) * m_rad_per_count;
        }

        /**
         * @brief Returns revolution pos in radians [0, 2pi).
         */
        [[nodiscard]] float get_pos_rad_rel() const
        {
            return static_cast<float>(get_pos_count_rel()) * m_rad_per_count;
        }

        /**
         * @brief Returns absolute shaft pos in degrees (multi-turn).
         */
        [[nodiscard]] float get_pos_deg_abs() const
        {
            return static_cast<float>(get_pos_count_abs()) * m_deg_per_count;
        }

        /**
         * @brief Returns revolution pos in degrees [0, 360).
         */
        [[nodiscard]] float get_pos_deg_rel() const
        {
            return static_cast<float>(get_pos_count_rel()) * m_deg_per_count;
        }

        void reset()
        {
            m_encoder.reset_count();
        }

        void set_as_home()
        {
            m_encoder.set_count(m_home_count);
        }

        [[nodiscard]] bool is_index_active() const
        {
            return m_encoder.read_index_channel();
        }

        [[nodiscard]] bool run_homing_sequence(valle::system::TimeoutMillis timeout)
        {
            enable();
            const bool index_found =
                valle::system::TimingContext::wait_for_with_timeout([&]() { return is_index_active(); }, timeout);

            if (!index_found)
            {
                return false;
            }

            // Once index is found, reset count to zero (or a known offset if needed)
            set_as_home();
            return true;
        }

        [[nodiscard]] EncoderInterfaceT& get_encoder()
        {
            return m_encoder;
        }
    };

}  // namespace valle::app
