#pragma once

#include <cstdint>

#include "valle/base/system_build/device.hpp"

namespace valle::app
{
    // =============================================================================
    // ADC Interface
    // =============================================================================
    template <typename TDerived, typename TConfig>
    class ACS724ModuleADCChannelInterfaceX
    {
    public:
        using ConfigT = TConfig;

        /**
         * @brief Initialize.
         *
         * @param config Configuration structure.
         */
        [[nodiscard]] bool init(const ConfigT& config)
        {
            return static_cast<TDerived*>(this)->init_impl(config);
        }

        /**
         * @brief Read the converted current value.
         *
         * @return float Current in Amps.
         */
        [[nodiscard]] float read_amps() const
        {
            return static_cast<const TDerived*>(this)->read_amps_impl();
        }
    };

    // =============================================================================
    // Module
    // =============================================================================

    /**
     * @brief ACS724 models.
     *
     */
    enum class ACS724Model : uint8_t
    {
        k2P5ABi = 0,  // 2P5AB-S ±2.5
        k05AUni,      // 05AU-S 5
        k05ABi,       // 05AB-S ±5
        k10AUni,      // 10AU-S 10
        k10ABi,       // 10AB-S ±10
        k20AUni,      // 20AU-S 20
        k20ABi,       // 20AB-S ±20
        k30AUni,      // 30AU-S 30
        k30ABi,       // 30AB-S ±30
        k50ABi,       // 50AB-S ±50
    };

    // -----------------------------------------------------------------------------
    // Model Traits
    // -----------------------------------------------------------------------------

    template <ACS724Model tkModel>
    struct ACS724Traits;

    template <>
    struct ACS724Traits<ACS724Model::k2P5ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 5.0F;
        static constexpr float skMinAmps       = -2.5F;
        static constexpr float skMaxAmps       = 2.5F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k05AUni>
    {
        static constexpr bool  skBiDirectional = false;
        static constexpr float skRangeAmps     = 5.0F;
        static constexpr float skMinAmps       = 0.0F;
        static constexpr float skMaxAmps       = 5.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k05ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 10.0F;
        static constexpr float skMinAmps       = -5.0F;
        static constexpr float skMaxAmps       = 5.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k10AUni>
    {
        static constexpr bool  skBiDirectional = false;
        static constexpr float skRangeAmps     = 10.0F;
        static constexpr float skMinAmps       = 0.0F;
        static constexpr float skMaxAmps       = 10.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k10ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 20.0F;
        static constexpr float skMinAmps       = -10.0F;
        static constexpr float skMaxAmps       = 10.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k20AUni>
    {
        static constexpr bool  skBiDirectional = false;
        static constexpr float skRangeAmps     = 20.0F;
        static constexpr float skMinAmps       = 0.0F;
        static constexpr float skMaxAmps       = 20.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k20ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 40.0F;
        static constexpr float skMinAmps       = -20.0F;
        static constexpr float skMaxAmps       = 20.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k30AUni>
    {
        static constexpr bool  skBiDirectional = false;
        static constexpr float skRangeAmps     = 30.0F;
        static constexpr float skMinAmps       = 0.0F;
        static constexpr float skMaxAmps       = 30.0F;
    };

    template <>
    struct ACS724Traits<ACS724Model::k30ABi>
    {
        static constexpr bool  skBiDirectional = true;
        static constexpr float skRangeAmps     = 60.0F;
        static constexpr float skMinAmps       = -30.0F;
        static constexpr float skMaxAmps       = 30.0F;
    };

    // -----------------------------------------------------------------------------
    // Module Drivers
    // -----------------------------------------------------------------------------

    template <typename TADCChannelInterfaceConfig>
    struct ACS724ModuleConfigX
    {
        TADCChannelInterfaceConfig channel_config{};  /// ADC Channel Configuration
    };

    /**
     * @brief ACS724 Current Sensor Driver.
     *
     * @tparam TADCChannel ADC Channel Device type.
     * @tparam tkModel   ACS724 Model type.
     */
    template <typename TADCChannelInterface, ACS724Model tkModel>
    class ACS724ModuleX
    {
    public:
        using ADCChannelInterfaceT           = TADCChannelInterface;
        static constexpr ACS724Model skModel = tkModel;

        using ConfigT = ACS724ModuleConfigX<typename ADCChannelInterfaceT::ConfigT>;

        using InjectDevices = typename GetInjectDevices<ADCChannelInterfaceT>::type;
        using DependDevices = typename GetAdditionalDependDevices<ADCChannelInterfaceT>::type;

        using ModelTraits = ACS724Traits<tkModel>;

    private:
        ADCChannelInterfaceT m_adc;  /// ADC Channel Interface driver

    public:
        template <typename... TArgs>
        explicit ACS724ModuleX(TArgs&&... args) : m_adc(std::forward<TArgs>(args)...)
        {
        }

        // ------------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------------
        /**
         * @brief Initialize ACS724 Module.
         *
         * @param config Configuration structure.
         */
        [[nodiscard]] bool init(const ConfigT& config)
        {
            return m_adc.init(config.channel_config);
        }

        // ------------------------------------------------------------------------
        // Control Interface
        // ------------------------------------------------------------------------

        /**
         * @brief Read the converted current value.
         *
         * @return float Current in Amps.
         */
        [[nodiscard]] float read_amps() const
        {
            return m_adc.read_amps();
        }

        [[nodiscard]] ADCChannelInterfaceT& get_adc()
        {
            return m_adc;
        }
    };

}  // namespace valle::app