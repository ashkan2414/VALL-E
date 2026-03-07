#pragma once

#include <jungles/bitfields.hpp>
#include <numbers>
#include <span>

#include "valle/core/device/device.hpp"
#include "valle/math/converters.hpp"

namespace valle
{
    // ==============================================================================
    // CORE Functionality
    // ==============================================================================

    // ------------------------------------------------------------------------------
    // HARDWARE REGISTERS
    // ------------------------------------------------------------------------------

    enum class LDC161XReg : uint8_t
    {
        kDataChannel0MSB = 0x00,
        kDataChannel0LSB = 0x01,
        kDataChannel1MSB = 0x02,
        kDataChannel1LSB = 0x03,
        kDataChannel2MSB = 0x04,  // LDC1614 Only
        kDataChannel2LSB = 0x05,  // LDC1614 Only
        kDataChannel3MSB = 0x06,  // LDC1614 Only
        kDataChannel3LSB = 0x07,  // LDC1614 Only

        kRCountChannel0 = 0x08,
        kRCountChannel1 = 0x09,
        kRCountChannel2 = 0x0A,  // LDC1614 Only
        kRCountChannel3 = 0x0B,  // LDC1614 Only

        kOffsetChannel0 = 0x0C,
        kOffsetChannel1 = 0x0D,
        kOffsetChannel2 = 0x0E,  // LDC1614 Only
        kOffsetChannel3 = 0x0F,  // LDC1614 Only

        kSettleCountChannel0 = 0x10,
        kSettleCountChannel1 = 0x11,
        kSettleCountChannel2 = 0x12,  // LDC1614 Only
        kSettleCountChannel3 = 0x13,  // LDC1614 Only

        kClockDividerChannel0 = 0x14,
        kClockDividerChannel1 = 0x15,
        kClockDividerChannel2 = 0x16,  // LDC1614 Only
        kClockDividerChannel3 = 0x17,  // LDC1614 Only

        kStatus      = 0x18,
        kErrorConfig = 0x19,
        kConfig      = 0x1A,
        kMuxConfig   = 0x1B,
        kDeviceReset = 0x1C,

        kDriveCurrentChannel0 = 0x1E,
        kDriveCurrentChannel1 = 0x1F,
        kDriveCurrentChannel2 = 0x20,  // LDC1614 Only
        kDriveCurrentChannel3 = 0x21,  // LDC1614 Only

        kReadManufacturerID = 0x7E,
        kReadDeviceID       = 0x7F
    };

    enum class LDC161XDataRegFields : uint8_t
    {
        kErrUR = 0,  // Under-Range error flag, cleared by reading DATAx register
        kErrOR,      // Over-Range error flag, cleared by reading DATAx register
        kErrWD,      // Watchdog Error flag, cleared by reading DATAx register
        kErrAE,      // Amplitude Error flag, cleared by reading DATAx register
        kData,       // Measured data
    };

    using LDC161XDataReg = jungles::Bitfields<uint32_t,
                                              jungles::Field<LDC161XDataRegFields::kErrUR, 1>,
                                              jungles::Field<LDC161XDataRegFields::kErrOR, 1>,
                                              jungles::Field<LDC161XDataRegFields::kErrWD, 1>,
                                              jungles::Field<LDC161XDataRegFields::kErrAE, 1>,
                                              jungles::Field<LDC161XDataRegFields::kData, 28>>;

    enum class LDC161XRCountRegFields : uint8_t
    {
        /**
         * @brief Reference Count conversion interval time.
         * 0x0000 - 0x0004: Reserved
         * 0x0005 - 0xFFFF: Conversion time = (RCOUNT * 16) / f_REF
         */
        kRCount,
    };

    using LDC161XRCountReg = jungles::Bitfields<uint16_t, jungles::Field<LDC161XRCountRegFields::kRCount, 16>>;

    enum class LDC161XOffsetRegFields : uint8_t
    {
        /**
         * @brief Channel conversion offset.
         * f_offset = (OFFSET / 2^16) * f_REF
         */
        kOffset,
    };

    using LDC161XOffsetReg = jungles::Bitfields<uint16_t, jungles::Field<LDC161XOffsetRegFields::kOffset, 16>>;

    enum class LDC161XSettleCountRegFields : uint8_t
    {
        /**
         * @brief Settle Count time.
         * The LDC will use this settling time to allow the LC sensor to
         * stabilize before initiation of a conversion on Channel 0.
         * If the amplitude has not settled prior to the conversion start, an
         * Amplitude error will be generated if reporting of this type of error
         * is enabled.
         * 0x0000: settle time = 32 / f_REF
         * 0x0001: settle time = 32 / f_REF
         * 0x0002 - 0xFFFF: settle time = (SETTLE_COUNT * 16) / f_REF
         */
        kSettleCount,
    };

    using LDC161XSettleCountReg =
        jungles::Bitfields<uint16_t, jungles::Field<LDC161XSettleCountRegFields::kSettleCount, 16>>;

    enum class LDC161XClockDividerRegFields : uint8_t
    {
        /**
         * @brief  Frequency Input Divider
         * Sets the divider for Channel 0 input.
         * @note Must be set to >=2 if the Sensor frequency is >= 8.75MHz.
         * 0x0000: Reserved. Do not use.
         * FIN_DIVIDER >= 0x0001: f_IN = ƒ_SENSOR/FIN_DIVIDER
         */
        kFinDivider,
        kReserved,  // Reserved

        /**
         * @brief Frequency Reference Divider
         *
         * Sets the divider for channel reference.
         * Use this to scale the maximum conversion frequency.
         *
         * 0x000: Reserved. Do not use.
         * FREF_DIVIDER >= 0x001: f_REF = f_CLK / FREF_DIVIDER
         */
        kFrefDivider,
    };

    using LDC161XClockDividerReg = jungles::Bitfields<uint16_t,
                                                      jungles::Field<LDC161XClockDividerRegFields::kFinDivider, 4>,
                                                      jungles::Field<LDC161XClockDividerRegFields::kReserved, 2>,
                                                      jungles::Field<LDC161XClockDividerRegFields::kFrefDivider, 10>>;

    enum class LDC161XStatusRegFields : uint8_t
    {
        /**
         * @brief Channel Error Indicator
         * Indicates which channel has generated an error condition.
         * Once flagged, any reported error is latched and maintained until either
         * the STATUS or DATAx register corresponding to the channel is read.
         *
         * 0b00: Channel 0 is the source of the error
         * 0b01: Channel 1 is the source of the error
         * 0b10: Channel 2 is the source of the error (LDC1614 only)
         * 0b11: Channel 3 is the source of the error (LDC1614 only)
         */
        kErrChannel,
        kErrUR,         // Under-Range Error Flag
        kErrOR,         // Over-Range Error Flag
        kErrWD,         // Watchdog Timeout Error Flag
        kErrAH,         // Amplitude High Error Flag
        kErrAL,         // Amplitude Low Error Flag
        kErrZC,         // Zero Count Error Flag
        kReserved1,     // Reserved
        kDRDY,          // Data Ready Flag
        kReserved2,     // Reserved
        kUndreadConv0,  // Underrun Conversion Flag
        kUndreadConv1,  // Underrun Conversion Flag
        kUndreadConv2,  // Underrun Conversion Flag
        kUndreadConv3,  // Underrun Conversion Flag
    };

    using LDC161XStatusReg = jungles::Bitfields<uint16_t,
                                                jungles::Field<LDC161XStatusRegFields::kErrChannel, 2>,
                                                jungles::Field<LDC161XStatusRegFields::kErrUR, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kErrOR, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kErrWD, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kErrAH, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kErrAL, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kErrZC, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kReserved1, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kDRDY, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kReserved2, 2>,
                                                jungles::Field<LDC161XStatusRegFields::kUndreadConv0, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kUndreadConv1, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kUndreadConv2, 1>,
                                                jungles::Field<LDC161XStatusRegFields::kUndreadConv3, 1>>;

    enum class LDC161XErrorConfigRegFields : uint8_t
    {
        kURErr2Out,  // Whether to report Under-Range Error in the DATAx registers
        kORErr2Out,  // Whether to report Over-Range Error in the DATAx registers
        kWDErr2Out,  // Whether to report Watchdog Error in the DATAx registers
        kAHErr2Out,  // Whether to report Amplitude High Error in the DATAx registers
        kALErr2Out,  // Whether to report Amplitude Low Error in the DATAx registers
        kReserved1,  // Reserved
        kURErr2Int,  // Whether to generate interrupt on the INTB pin on Under-Range Error
        kORErr2Int,  // Whether to generate interrupt on the INTB pin on Over-Range Error
        kWDErr2Int,  // Whether to generate interrupt on the INTB pin on Watchdog Error
        kAHErr2Int,  // Whether to generate interrupt on the INTB pin on Amplitude High Error
        kALErr2Int,  // Whether to generate interrupt on the INTB pin on Amplitude Low Error
        kZCErr2Int,  // Whether to generate interrupt on the INTB pin on Zero Count Error
        kReserved2,  // Reserved
        kDRDY2Int,   // Whether to generate interrupt on the INTB pin on Data Ready
    };

    using LDC161XErrorConfigReg = jungles::Bitfields<uint16_t,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kURErr2Out, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kORErr2Out, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kWDErr2Out, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kAHErr2Out, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kALErr2Out, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kReserved1, 3>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kURErr2Int, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kORErr2Int, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kWDErr2Int, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kAHErr2Int, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kALErr2Int, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kZCErr2Int, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kReserved2, 1>,
                                                     jungles::Field<LDC161XErrorConfigRegFields::kDRDY2Int, 1>>;

    enum class LDC161XConfigRegFields : uint8_t
    {
        /**
         * @brief Active Channel Selection
         * Selects which channel is active for conversion when MUX CONFIG AUTOSCAN is disabled.
         */
        kActiveChannel,
        kSleepModeEn,   // Sleep Mode Enable
        kRPOverrideEn,  // Sensor Rp override enable. Control over sensor current drive.

        /**
         * @brief Sensor Activate mode selection
         * 0: Full current activation mode: the LDC will drive maximum sensor current for a shorter sensor activation
         * time. 1: Low Power activation mode: the LDC uses the value in DRIVE_CURRENTx during sensor activation to
         * minimize power consumption.
         */
        kSensorActivateSel,

        /**
         * @brief Automatic Amplitude Control Disable
         * Setting this bit disables automatic amplitude and stop the updating of INIT_IDRIVEx field.
         * 0: Automatic Amplitude Control enabled
         * 1: Automatic Amplitude Control disabled. Recommended for precision applications.
         */
        kAutoAmpDis,
        kRefClkSrc,       // Reference Clock Source Selection. 0 = internal, 1 = CLKIN pin
        kReserved1,       // Reserved
        kIntBDis,         // INTB Pin Disable
        kHighCurrentDrv,  // High Current Drive Enable. 0 = normal (<= 1.5mA), 1 = high current drive mode (> 1.5mA)
        kReserved2,       // Reserved
    };

    using LDC161XConfigReg = jungles::Bitfields<uint16_t,
                                                jungles::Field<LDC161XConfigRegFields::kActiveChannel, 2>,
                                                jungles::Field<LDC161XConfigRegFields::kSleepModeEn, 1>,
                                                jungles::Field<LDC161XConfigRegFields::kRPOverrideEn, 1>,
                                                jungles::Field<LDC161XConfigRegFields::kSensorActivateSel, 1>,
                                                jungles::Field<LDC161XConfigRegFields::kAutoAmpDis, 1>,
                                                jungles::Field<LDC161XConfigRegFields::kRefClkSrc, 1>,
                                                jungles::Field<LDC161XConfigRegFields::kReserved1, 1>,
                                                jungles::Field<LDC161XConfigRegFields::kIntBDis, 1>,
                                                jungles::Field<LDC161XConfigRegFields::kHighCurrentDrv, 1>,
                                                jungles::Field<LDC161XConfigRegFields::kReserved2, 6>>;

    enum class LDC161XRRSequence : uint8_t
    {
        kCh0Ch1       = 0b00,  // Ch0, Ch1
        kCh0Ch1Ch2    = 0b01,  // Ch0, Ch1, Ch2 (LDC1614 only)
        kCh0Ch1Ch2Ch3 = 0b10,  // Ch0, Ch1, Ch2, Ch3 (LDC1614 only)
        kCh0Ch1Repeat = 0b11,  // Ch0, Ch1
    };

    /**
     * @brief Deglitch Filter Bandwidth Settings
     * Select the lowest setting that exceeds the maximum sensor oscillation frequency.
     */
    enum class LDC161XDeglitchBandwidth : uint8_t
    {
        kBand1MHz   = 0b001,
        kBand3p3MHz = 0b100,
        kBand10MHz  = 0b101,
        kBand33MHz  = 0b111,
    };

    enum class LDC161XMuxConfigRegFields : uint8_t
    {
        /**
         * @brief Auto Scan Enable
         * 0: Continuous conversion on the channel selected by ACTIVE CHANNEL field in CONFIG register.
         * 1: Auto scan conversions as selected by RR SEQUENCE field.
         */
        kAutoScanEn,

        /**
         * @brief Auto-scan sequence configuration
         * Configure multiplexing channel sequence. The LDC will perform a single conversion on each channel in the
         * sequence selected, and then restart the sequence continuously. 0b00: Ch0, Ch1 0b01: Ch0, Ch1, Ch2 (LDC1614
         * only) 0b10: Ch0, Ch1, Ch2, Ch3 (LDC1614 only) 0b11: Ch0, Ch1
         */
        kRRSequence,  // Auto scan sequence configuration
        kReserved,    // Reserved

        /**
         * @brief Input deglitch filter bandwidth
         * Select the lowest setting that exceeds the maximum sensor oscillation frequency.
         * 0b001: 1 MHz
         * 0b100: 3.3 MHz
         * 0b101: 10 Mhz
         * 0b111: 33 MHz
         */
        kDeglitchEn,
    };

    using LDC161XMuxConfigReg = jungles::Bitfields<uint16_t,
                                                   jungles::Field<LDC161XMuxConfigRegFields::kAutoScanEn, 1>,
                                                   jungles::Field<LDC161XMuxConfigRegFields::kRRSequence, 2>,
                                                   jungles::Field<LDC161XMuxConfigRegFields::kReserved, 10>,
                                                   jungles::Field<LDC161XMuxConfigRegFields::kDeglitchEn, 3>>;
    enum class LDC161XResetRegFields : uint8_t
    {
        kResetDev,  // Device Reset. Write 1 to reset device. Will always read 0.
        kReserved,  // Reserved
    };

    using LDC161XResetReg = jungles::Bitfields<uint16_t,
                                               jungles::Field<LDC161XResetRegFields::kResetDev, 1>,
                                               jungles::Field<LDC161XResetRegFields::kReserved, 15>>;

    enum class LDC161XDriveCurrentRegFields : uint8_t
    {
        kIDrive,  // Drive Current Setting used during the settling and conversion time of the sensor. RP OVERRIDE must
                  // be 1.

        /**
         * @brief Initial Drive Current Setting used during calibration (readonly).
         * This field stores the Initial Drive Current measured during the initial Amplitude Calibration phase.
         * It is updated after each amplitude Correction phase of the sensor conversion if AUTO_AMP_DIS=0.
         * When writing to DRIVE CURRENTx register, set this field to 0b00000.
         */
        kInitIDrive,
        kReserved,  // Reserved
    };

    using LDC161XDriveCurrentReg = jungles::Bitfields<uint16_t,
                                                      jungles::Field<LDC161XDriveCurrentRegFields::kIDrive, 5>,
                                                      jungles::Field<LDC161XDriveCurrentRegFields::kInitIDrive, 5>,
                                                      jungles::Field<LDC161XDriveCurrentRegFields::kReserved, 6>>;

    enum class LDC161XManufacturerIDRegFields : uint8_t
    {
        kManufacturerID,  // Manufacturer ID, must be 0x5449
    };

    using LDC161XManufacturerIDReg =
        jungles::Bitfields<uint16_t, jungles::Field<LDC161XManufacturerIDRegFields::kManufacturerID, 16>>;

    enum class LDC161XDeviceIDRegFields : uint8_t
    {
        kDeviceID,  // Device ID, must be 0x3055
    };

    using LDC161XDeviceIDReg = jungles::Bitfields<uint16_t, jungles::Field<LDC161XDeviceIDRegFields::kDeviceID, 16>>;

    struct LDC161XTraits
    {
        using RCount         = uint16_t;  // 16 bits
        using Offset         = uint16_t;  // 16 bits
        using SettleCount    = uint16_t;  // 16 bits
        using FinDivider     = uint8_t;   // 4 bits
        using FrefDivider    = uint16_t;  // 10 bits
        using IDrive         = uint8_t;   // 5 bits
        using ManufacturerID = uint16_t;  // 16 bits
        using DeviceID       = uint16_t;  // 16 bits

        static constexpr uint16_t       skMinRCount                           = 8U;
        static constexpr uint16_t       skMinSettleCount                      = 3U;
        static constexpr FinDivider     skMinFinDivider                       = 1U;
        static constexpr FinDivider     skMaxFinDivider                       = 15U;
        static constexpr FrefDivider    skMinFrefDivider                      = 1U;
        static constexpr FrefDivider    skMaxFrefDivider                      = 1023U;
        static constexpr float          skMaxFrefMhzSingleChannel             = 35.0F;
        static constexpr float          skMaxFrefMhzMultiChannelInternalClock = 55.0F;
        static constexpr float          skMaxFrefMhzMultiChannelExternalClock = 40.0F;
        static constexpr IDrive         skMaxIDrive                           = 0x1FU;  // 5 bits, 31
        static constexpr ManufacturerID skManufacturerID                      = 0x5449;
        static constexpr DeviceID       skDeviceID                            = 0x3055;

        // From Section 8.1.5, table 42
        static constexpr std::array<uint16_t, 32> skIDriveTableUA = {
            16,  18,  20,  23,  28,  32,  40,  46,  52,  59,  72,  82,  95,   110,  127,  146,
            169, 195, 212, 244, 297, 342, 424, 489, 551, 635, 763, 880, 1017, 1173, 1355, 1563};

        template <uint8_t tkChannel>
            requires(tkChannel < 4 && tkChannel >= 0)
        static constexpr LDC161XReg skChannelDataMSBReg =
            static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kDataChannel0MSB) + (tkChannel * 2));

        template <uint8_t tkChannel>
            requires(tkChannel < 4 && tkChannel >= 0)
        static constexpr LDC161XReg skChannelDataLSBReg =
            static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kDataChannel0LSB) + (tkChannel * 2));

        template <uint8_t tkChannel>
            requires(tkChannel < 4 && tkChannel >= 0)
        static constexpr LDC161XReg skChannelRCountReg =
            static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kRCountChannel0) + tkChannel);

        template <uint8_t tkChannel>
            requires(tkChannel < 4 && tkChannel >= 0)
        static constexpr LDC161XReg skChannelOffsetReg =
            static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kOffsetChannel0) + tkChannel);

        template <uint8_t tkChannel>
            requires(tkChannel < 4 && tkChannel >= 0)
        static constexpr LDC161XReg skChannelSettleCountReg =
            static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kSettleCountChannel0) + tkChannel);

        template <uint8_t tkChannel>
            requires(tkChannel < 4 && tkChannel >= 0)
        static constexpr LDC161XReg skChannelClockDividerReg =
            static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kClockDividerChannel0) + tkChannel);

        template <uint8_t tkChannel>
            requires(tkChannel < 4 && tkChannel >= 0)
        static constexpr LDC161XReg skChannelDriveCurrentReg =
            static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kDriveCurrentChannel0) + tkChannel);
    };

    // ------------------------------------------------------------------------------
    // USER API
    // ------------------------------------------------------------------------------
    template <uint8_t tkNumChannels>
    concept CLDC161XValidNumChannels = (tkNumChannels >= 1 && tkNumChannels <= 4);

    // ------------------------------------------------------------------------------
    // CHANNEL CONFIGURATION
    // ------------------------------------------------------------------------------

    struct LDC161XCoilConfig
    {
        float inductance_uh;   // L in microhenries
        float capacitance_pf;  // C in picofarads
        float rp_kohm;         // R_p in kilohms
        float q_factor;        // Unitless
    };

    // ----------------------------------------------------------------------------
    // DRIVE CURRENT CONFIGURATION
    // ----------------------------------------------------------------------------
    struct LDC161XIDriveCurrent
    {
        LDC161XTraits::IDrive value = 0;

        [[nodiscard]] static constexpr LDC161XIDriveCurrent from_raw(const LDC161XTraits::IDrive raw_value)
        {
            return LDC161XIDriveCurrent{
                .value = static_cast<LDC161XTraits::IDrive>(raw_value & LDC161XTraits::skMaxIDrive)};  // 5 bits
        }

        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [[nodiscard]] static constexpr LDC161XIDriveCurrent from_coil_rp(const float coil_rp_kohm,
                                                                         const float target_v = 1.5F)
        {
            // Equation 12: I_drive = (pi * Vp) / (4 * Rp)
            // Simplified: I_uA = (PI * target_v * 250) / rp_kohm
            const float target_i_ua = (std::numbers::pi_v<float> * target_v * 250.0F) / coil_rp_kohm;

            // Find the first current in the table that is EQUAL to or GREATER than
            // our target. This ensures we have enough current to maintain oscillation.
            const auto lower_bound_it = std::lower_bound(
                LDC161XTraits::skIDriveTableUA.begin(), LDC161XTraits::skIDriveTableUA.end(), target_i_ua);

            if (lower_bound_it == LDC161XTraits::skIDriveTableUA.end())
            {
                return LDC161XIDriveCurrent::from_raw(
                    LDC161XTraits::skMaxIDrive);  // Calculated current exceeds standard IDRIVE range
            }

            return LDC161XIDriveCurrent::from_raw(
                static_cast<uint8_t>(std::distance(LDC161XTraits::skIDriveTableUA.begin(), lower_bound_it)));
        }
    };

    // ----------------------------------------------------------------------------
    // OFFSET CONFIG
    // ----------------------------------------------------------------------------
    struct LDC161XOffsetConfigRaw
    {
        uint16_t value = 0;

        // Factory: From an absolute frequency
        static constexpr LDC161XOffsetConfigRaw from_frequency_mhz(float foffset_mhz, float fref_mhz)
        {
            // Rearranged formula
            constexpr uint32_t two_to_16 = 1U << 16U;
            return LDC161XOffsetConfigRaw{.value = static_cast<uint16_t>((foffset_mhz / fref_mhz) * two_to_16)};
        }
    };

    struct LDC161XOffsetConfigFrequency
    {
        float offset_mhz = 0.0F;
    };

    using LDC161XOffsetConfig = std::variant<LDC161XOffsetConfigRaw, LDC161XOffsetConfigFrequency>;

    // ----------------------------------------------------------------------------
    // CHANNEL CONFIGURATION
    // ----------------------------------------------------------------------------
    struct LDC161XChannelConfig
    {
        LDC161XCoilConfig    coil_config{};
        LDC161XIDriveCurrent drive_current{};  // Used if RP Override is enabled
        LDC161XOffsetConfig  offset_config{};
    };

    // ===========================================================================
    // SENSOR CONFIGURATION
    // ===========================================================================

    // ----------------------------------------------------------------------------
    // CLOCK SOURCE CONFIGURATION
    // ----------------------------------------------------------------------------
    struct LDC161XClockSourceInternalOscillator
    {
        static constexpr float skFClkMHz = 43.4F;
    };

    struct LDC161XClockSourceExternalClock
    {
        float fclk_mhz;  // Usually 40MHz
    };
    using ClockSource = std::variant<LDC161XClockSourceInternalOscillator, LDC161XClockSourceExternalClock>;

    // ----------------------------------------------------------------------------
    // INTERRUPT CONFIG
    // ----------------------------------------------------------------------------
    struct LDC161XInterruptConfig
    {
        bool int_on_under_range : 1    = false;
        bool int_on_over_range : 1     = false;
        bool int_on_watchdog : 1       = false;
        bool int_on_amplitude_high : 1 = false;
        bool int_on_amplitude_low : 1  = false;
        bool int_on_zero_count : 1     = false;
        bool int_on_data_ready : 1     = false;
    };

    // ---------------------------------------------------------------------------
    // ERROR REPORTING CONFIG
    // ---------------------------------------------------------------------------
    struct LDC161XErrorReportingConfig
    {
        bool report_under_range : 1    = true;
        bool report_over_range : 1     = true;
        bool report_watchdog : 1       = true;
        bool report_amplitude_high : 1 = true;
        bool report_amplitude_low : 1  = true;
    };

    // ---------------------------------------------------------------------------
    // SENSOR ACTIVATION MODE
    // ---------------------------------------------------------------------------
    enum class LDC161XSensorActivationMode : uint8_t
    {
        kFullCurrentMode = 0,
        kLowPowerMode    = 1,
    };

    // ---------------------------------------------------------------------------
    // SENSOR CONFIG
    // ---------------------------------------------------------------------------
    template <uint8_t tkNumChannels>
        requires(CLDC161XValidNumChannels<tkNumChannels>)
    struct LDC161XSensorConfig
    {
        ClockSource                           clock_source       = LDC161XClockSourceExternalClock{.fclk_mhz = 40.0F};
        uint16_t                              sample_rate_hz     = 200U;
        LDC161XDeglitchBandwidth              deglitch_bandwidth = LDC161XDeglitchBandwidth::kBand10MHz;
        std::optional<LDC161XInterruptConfig> interrupt_config   = std::nullopt;
        LDC161XSensorActivationMode           sensor_activation_mode = LDC161XSensorActivationMode::kFullCurrentMode;

        bool enable_rp_override = true;
        bool auto_amplitude_en  = false;

        std::conditional_t<tkNumChannels == 1, bool, std::monostate> high_current_drive_en{};

        std::array<LDC161XChannelConfig, tkNumChannels> channels{};
    };

    // ==============================================================================
    // I2C Interface
    // ==============================================================================
    template <typename TDerived, typename TConfig>
    class LDC161XSensorModuleI2CInterfaceX
    {
    public:
        using ConfigT = TConfig;

        [[nodiscard]] bool init(const ConfigT& config)
        {
            return static_cast<TDerived*>(this)->init_impl(config);
        }

        template <uint8_t tkWriteBytes>
        [[nodiscard]] bool register_write_blocking(const std::span<const std::byte, tkWriteBytes>& write_data,
                                                   TimeoutMillis                                   timeout_ms)
        {
            return static_cast<TDerived*>(this)->template register_write_blocking_impl<tkWriteBytes>(write_data,
                                                                                                     timeout_ms);
        }

        template <uint8_t tkReadBytes>
        [[nodiscard]] bool register_read_blocking(LDC161XReg                               reg,
                                                  const std::span<std::byte, tkReadBytes>& data_out,
                                                  TimeoutMillis                            timeout_ms)
        {
            static_assert(sizeof(reg) == 1, "Register address must be 1 byte");

            return static_cast<TDerived*>(this)->template register_read_blocking_impl<tkReadBytes>(
                reg, data_out, timeout_ms);
        }

        // ---------------------------------------------------------------------------
        // overloads
        // ---------------------------------------------------------------------------
        template <typename TValue>
        [[nodiscard]] bool register_write_blocking(LDC161XReg reg, const TValue& value, TimeoutMillis timeout_ms)
        {
            return register_write_blocking<sizeof(TValue)>(
                reg,
                std::span<const std::byte, sizeof(TValue)>(reinterpret_cast<const std::byte*>(&value), sizeof(TValue)),
                timeout_ms);
        }

        template <uint8_t tkWriteBytes>
        [[nodiscard]] bool register_write_blocking(LDC161XReg                                      reg,
                                                   const std::span<const std::byte, tkWriteBytes>& data,
                                                   TimeoutMillis                                   timeout_ms)
        {
            // Prepare data in the stable member buffer
            std::array<std::byte, tkWriteBytes + 1> write_data;

            write_data[0] = static_cast<std::byte>(reg);
            std::copy(data.begin(), data.end(), write_data.begin() + 1);

            return register_write_blocking<tkWriteBytes + 1>(std::span(write_data), timeout_ms);
        }

        template <typename TValue>
        [[nodiscard]] bool register_read_blocking(LDC161XReg reg, TValue& value_out, TimeoutMillis timeout_ms)
        {
            return register_read_blocking<sizeof(TValue)>(
                reg,
                std::span<std::byte, sizeof(TValue)>(reinterpret_cast<std::byte*>(&value_out), sizeof(TValue)),
                timeout_ms);
        }
    };

    // ==============================================================================
    // Module Driver
    // ==============================================================================

    // ---------------------------------------------------------------------------
    // CONFIG
    // ---------------------------------------------------------------------------
    template <typename TI2CConfig, uint8_t tkNumChannels>
        requires(CLDC161XValidNumChannels<tkNumChannels>)
    struct LDC161XSensorModuleConfigX
    {
        TI2CConfig                         i2c_config{};
        LDC161XSensorConfig<tkNumChannels> sensor_config{};
    };

    struct LDC161XChannelConfigRaw
    {
        float                  fref_mhz      = 0.0F;
        LDC161XRCountReg       rcount        = 0;
        LDC161XOffsetReg       offset        = 0;
        LDC161XSettleCountReg  settle_count  = 0;
        LDC161XClockDividerReg clock_divider = 0;
        LDC161XDriveCurrentReg idrive        = 0;
    };

    template <uint8_t tkNumChannels>
        requires(CLDC161XValidNumChannels<tkNumChannels>)
    struct LDC161XSensorConfigRaw
    {
        LDC161XMuxConfigReg   mux_config    = 0;
        LDC161XErrorConfigReg error_config  = 0;
        LDC161XConfigReg      device_config = 0;

        std::array<LDC161XChannelConfigRaw, tkNumChannels> channels{};
    };

    // ---------------------------------------------------------------------------
    // MODULE
    // ---------------------------------------------------------------------------

    template <typename TI2CInterface, uint8_t tkNumChannels>
        requires(CLDC161XValidNumChannels<tkNumChannels>)
    class LDC161XSensorModuleX
    {
    public:
        using I2CInterfaceT = TI2CInterface;
        using ConfigT       = LDC161XSensorModuleConfigX<typename I2CInterfaceT::ConfigT, tkNumChannels>;
        using SensorConfigT = LDC161XSensorConfig<tkNumChannels>;
        static constexpr TimeoutMillis skDefaultTimeout = TimeoutMillis(50);

        using InjectDevices = typename GetInjectDevices<I2CInterfaceT>::type;
        using DependDevices = typename GetAdditionalDependDevices<I2CInterfaceT>::type;

    private:
        I2CInterfaceT                                                m_i2c{};
        LDC161XSensorConfigRaw<tkNumChannels>                        m_config{};
        std::array<LinearConverter<uint32_t, double>, tkNumChannels> m_ch_freq_converters{};

    public:
        template <typename... TArgs>
        explicit LDC161XSensorModuleX(TArgs&&... args) : m_i2c(std::forward<TArgs>(args)...)
        {
        }

        [[nodiscard]] bool init(const ConfigT& config)
        {
            if (!m_i2c.init(config.i2c_config))
            {
                return false;
            }

            m_config             = get_raw_config(config.sensor_config);
            m_ch_freq_converters = get_all_channel_freq_converters(m_config);

            // Reset device to default state
            reset();

            // Verify device is connected and responding with correct IDs before configuring
            if (!verify_device_connected())
            {
                return false;
            }

            configure();
            return true;
        }

        void reset()
        {
            LDC161XResetReg reset_reg;
            reset_reg.at<LDC161XResetRegFields::kResetDev>() = 1U;
            m_i2c.register_write_blocking(LDC161XReg::kDeviceReset, reset_reg.serialize(), skDefaultTimeout);
            delay_ms(10);  // Allow time for reset to complete
        }

        [[nodiscard]] uint16_t read_manufacturer_id(const TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            uint16_t manufacturer_id;
            m_i2c.register_read_blocking(LDC161XReg::kReadManufacturerID, manufacturer_id, timeout_ms);

            return LDC161XManufacturerIDReg(std::move(manufacturer_id))
                .at<LDC161XManufacturerIDRegFields::kManufacturerID>();
        }

        [[nodiscard]] bool verify_manufacturer_id(const TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            return read_manufacturer_id(timeout_ms) == LDC161XTraits::skManufacturerID;
        }

        [[nodiscard]] uint16_t read_device_id(const TimeoutMillis = skDefaultTimeout)
        {
            uint16_t device_id;
            m_i2c.register_read_blocking(LDC161XReg::kReadDeviceID, device_id, timeout_ms);
            return LDC161XDeviceIDReg(std::move(device_id)).at<LDC161XDeviceIDRegFields::kDeviceID>();
        }

        [[nodiscard]] bool verify_device_id(const TimeoutMillis = skDefaultTimeout)
        {
            return read_device_id(timeout_ms) == LDC161XTraits::skDeviceID;
        }

        [[nodiscard]] bool verify_device_connected(const TimeoutMillis
        {
            return verify_manufacturer_id(timeout_ms) && verify_device_id(timeout_ms);
        }

        void configure()
        {
            // NOTE: THE ORDER OF REGISTER WRITES IS IMPORTANT

            // Configure channels
            if constexpr (tkNumChannels >= 1)
            {
                configure_channel<0>();
            }

            if constexpr (tkNumChannels >= 2)
            {
                configure_channel<1>();
            }

            if constexpr (tkNumChannels >= 3)
            {
                configure_channel<2>();
            }

            if constexpr (tkNumChannels >= 4)
            {
                configure_channel<3>();
            }

            // MUX CONFIG
            m_i2c.register_write_blocking(LDC161XReg::kMuxConfig, m_config.mux_config.serialize(), skDefaultTimeout);

            // ERROR CONFIG
            m_i2c.register_write_blocking(
                LDC161XReg::kErrorConfig, m_config.error_config.serialize(), skDefaultTimeout);

            // DEVICE CONFIG (exit sleep mode last)
            m_i2c.register_write_blocking(LDC161XReg::kConfig, m_config.device_config.serialize(), skDefaultTimeout);
        }

        template <uint8_t tkChannel>
            requires(tkChannel >= 0 && tkChannel < tkNumChannels)
        void configure_channel()
        {
            const auto& chan_config = m_config.channels[tkChannel];

            // Write CLOCK DIVIDER
            m_i2c.register_write_blocking(LDC161XTraits::skChannelClockDividerReg<tkChannel>,
                                          chan_config.clock_divider.serialize(),
                                          skDefaultTimeout);

            // Write RCOUNT
            m_i2c.register_write_blocking(
                LDC161XTraits::skChannelRCountReg<tkChannel>, chan_config.rcount.serialize(), skDefaultTimeout);

            // Write OFFSET
            m_i2c.register_write_blocking(
                LDC161XTraits::skChannelOffsetReg<tkChannel>, chan_config.offset.serialize(), skDefaultTimeout);

            // Write SETTLE COUNT
            m_i2c.register_write_blocking(LDC161XTraits::skChannelSettleCountReg<tkChannel>,
                                          chan_config.settle_count.serialize(),
                                          skDefaultTimeout);

            // Write DRIVE CURRENT
            m_i2c.register_write_blocking(
                LDC161XTraits::skChannelDriveCurrentReg<tkChannel>, chan_config.idrive.serialize(), skDefaultTimeout);
        }

        [[nodiscard]] LDC161XStatusReg read_status(const DurationMillistTimeout)
        {
            uint16_t status_raw;
            m_i2c.register_read_blocking(LDC161XReg::kStatus, status_raw, timeout_ms);
            return LDC161XStatusReg(std::move(status_raw));
        }

        template <uint8_t tkChannel>
            requires(tkChannel >= 0 && tkChannel < tkNumChannels)
        [[nodiscard]] uint32_t read_data_raw(const DurationMillistTimeout)
        {
            static_assert(tkChannel < tkNumChannels, "Channel index out of range");

            uint16_t data_msb = 0, data_lsb = 0;
            m_i2c.register_read_blocking(LDC161XTraits::skChannelDataMSBReg<tkChannel>, data_msb, timeout_ms);
            m_i2c.register_read_blocking(LDC161XTraits::skChannelDataLSBReg<tkChannel>, data_lsb, timeout_ms);

            LDC161XDataReg data_reg((static_cast<uint32_t>(data_msb) << 16) | data_lsb);

            return data_reg.at<LDC161XDataRegFields::kData>();
        }

        template <uint8_t tkChannel>
            requires(tkChannel >= 0 && tkChannel < tkNumChannels)
        [[nodiscard]] double read_frequency(const TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            const uint32_t raw_data = read_data_raw<tkChannel>(timeout_ms);
            return m_ch_freq_converters[tkChannel].convert(raw_data);
        }

        [[nodiscard]] I2CInterfaceT& get_i2c()
        {
            return m_i2c;
        }

    private:
        // ------------------------------------------------------------------------------
        // CONFIG CALCULATION UTILITIES
        // ------------------------------------------------------------------------------
        [[nodiscard]] static constexpr float calculate_fsensor_mhz(const LDC161XCoilConfig& coil)
        {
            const double l_henries = static_cast<double>(coil.inductance_uh) * 1.0e-6;    // Convert uH to H
            const double c_farads  = static_cast<double>(coil.capacitance_pf) * 1.0e-12;  // Convert pF to F
            const double q_factor  = static_cast<double>(coil.q_factor);

            // Intermediate calculation: sqrt(L * C)
            const double sqrt_lc = std::sqrt(l_henries * c_farads);

            // Part 1: Ideal resonant frequency term [ 1 / (2*pi*sqrt(LC)) ]
            const double ideal_f = 1.0 / (2.0 * std::numbers::pi * sqrt_lc);

            // Part 2: The correction factor terms inside the square root
            // Term A: 1/Q^2
            const double term_a = 1.0 / (q_factor * q_factor);

            // Term B: (5 * 10^-9) / (Q * sqrt(LC))
            const double term_b = 5.0e-9 / (q_factor * sqrt_lc);

            // Combine terms inside the root
            const double correction_term = std::sqrt(1.0 - term_a - term_b);

            return static_cast<float>(ideal_f * correction_term * 1.0e-6);  // Convert to MHz
        }

        [[nodiscard]] static constexpr uint16_t calculate_min_settle_count(const float coil_q_factor,
                                                                           const float fref_mhz,
                                                                           const float fsensor_mhz)
        {
            return static_cast<uint16_t>((coil_q_factor * fref_mhz) / (16.0F * fsensor_mhz)) + 1U;
        }

        [[nodiscard]] static constexpr float calculate_settle_time_us(uint16_t settle_count, float fref_mhz)
        {
            return (static_cast<float>(settle_count) * 16.0F) / fref_mhz;
        }

        [[nodiscard]] static constexpr uint16_t calculate_rcount(float conversion_time_us, float fref_mhz)
        {
            return static_cast<uint16_t>((conversion_time_us * fref_mhz) / 16.0F);
        }

        [[nodiscard]] static constexpr float calculate_channel_switch_time_us(const float frez_mhz)
        {
            return (5.0F / frez_mhz) + 0.692F;
        }

        [[nodiscard]] static constexpr LinearConverter<uint32_t, double> get_channel_freq_converter(
            const LDC161XChannelConfigRaw& config)
        {
            constexpr uint32_t two_to_28 = 1U << 28;
            constexpr uint32_t two_to_16 = 1U << 16;

            // Common product used in both factors
            const uint16_t fin_divider    = config.clock_divider.at<LDC161XClockDividerRegFields::kFinDivider>();
            const double   clock_product  = static_cast<double>(fin_divider) * static_cast<double>(config.fref_mhz);
            const double   offset_reg_val = config.offset.at<LDC161XOffsetRegFields::kOffset>();

            const double slope  = clock_product / two_to_28;
            const double offset = (clock_product * static_cast<double>(offset_reg_val)) / two_to_16;
            return LinearConverter<uint32_t, double>(slope, offset);
        }

        [[nodiscard]] static constexpr std::array<LinearConverter<uint32_t, double>, tkNumChannels>
        get_all_channel_freq_converters(const LDC161XSensorConfigRaw<tkNumChannels>& raw_config)
        {
            std::array<LinearConverter<uint32_t, double>, tkNumChannels> converters;
            for (uint8_t i = 0; i < tkNumChannels; ++i)
            {
                converters[i] = get_channel_freq_converter(raw_config.channels[i]);
            }
            return converters;
        }

        [[nodiscard]] static constexpr std::array<LDC161XChannelConfigRaw, tkNumChannels> get_channel_config(
            const SensorConfigT& config)
        {
            std::array<LDC161XChannelConfigRaw, tkNumChannels> results;

            const auto clk_source_info = std::visit(
                Overloaded{[](const LDC161XClockSourceInternalOscillator& int_clk)
                           {
                               return std::make_pair(int_clk.skFClkMHz,
                                                     (tkNumChannels == 1)
                                                         ? LDC161XTraits::skMaxFrefMhzSingleChannel
                                                         : LDC161XTraits::skMaxFrefMhzMultiChannelInternalClock);
                           },
                           [](const LDC161XClockSourceExternalClock& ext_clk)
                           {
                               return std::make_pair(ext_clk.fclk_mhz,
                                                     (tkNumChannels == 1)
                                                         ? LDC161XTraits::skMaxFrefMhzSingleChannel
                                                         : LDC161XTraits::skMaxFrefMhzMultiChannelExternalClock);
                           }},
                config.clock_source);

            const auto fclk_mhz     = std::move(clk_source_info.first);
            const auto max_fref_mhz = std::move(clk_source_info.second);

            const float sample_period    = 1.0f / static_cast<float>(config.sample_rate_hz);
            const float sample_period_us = sample_period * 1.0e6f;

            float conversion_time_left_us = sample_period_us;
            for (uint8_t i = 0; i < tkNumChannels; ++i)
            {
                const LDC161XChannelConfig& channel_config = config.channels[i];
                LDC161XChannelConfigRaw&    raw_config     = results[i];

                // --- CLOCK DIVIDERS ---
                const float fsensor_mhz = calculate_fsensor_mhz(channel_config.coil_config);

                // Determine the maximum allowable fref
                const LDC161XTraits::FrefDivider fref_divider =
                    std::clamp(static_cast<uint16_t>(std::ceil(fclk_mhz / max_fref_mhz)),
                               LDC161XTraits::skMinFrefDivider,
                               LDC161XTraits::skMaxFrefDivider);
                raw_config.clock_divider.at<LDC161XClockDividerRegFields::kFrefDivider>() = fref_divider;

                raw_config.fref_mhz = fclk_mhz / static_cast<float>(fref_divider);
                assert(raw_config.fref_mhz <= max_fref_mhz &&
                       "Resulting fref exceeds maximum allowed reference frequency");

                const float                     max_fin_mhz = raw_config.fref_mhz / 4.0f;
                const LDC161XTraits::FinDivider fin_divider =
                    std::clamp(static_cast<LDC161XTraits::FinDivider>(std::ceil(fsensor_mhz / max_fin_mhz)),
                               LDC161XTraits::skMinFinDivider,
                               LDC161XTraits::skMaxFinDivider);

                raw_config.clock_divider.at<LDC161XClockDividerRegFields::kFinDivider>() = fin_divider;

                const float fin_mhz = fsensor_mhz / static_cast<float>(fin_divider);
                assert(fin_mhz < raw_config.fref_mhz / 4.0f && "Resulting fin exceeds maximum allowed input frequency");

                // --- SETTLE COUNT ---
                const LDC161XTraits::SettleCount min_settle_count =
                    calculate_min_settle_count(channel_config.coil_config.q_factor, raw_config.fref_mhz, fsensor_mhz);

                assert(min_settle_count >= LDC161XTraits::skMinSettleCount &&
                       "Calculated minimum settle count is less than allowed minimum");
                raw_config.settle_count.at<LDC161XSettleCountRegFields::kSettleCount>() = min_settle_count;

                const float settle_time_us         = calculate_settle_time_us(min_settle_count, raw_config.fref_mhz);
                const float channel_switch_time_us = calculate_channel_switch_time_us(fsensor_mhz);

                conversion_time_left_us -= settle_time_us + channel_switch_time_us;

                // --- OFFSET ---
                const LDC161XTraits::Offset offset =
                    std::visit(Overloaded{[&](const LDC161XOffsetConfigRaw& raw_offset) { return raw_offset.value; },
                                          [&](const LDC161XOffsetConfigFrequency& freq_offset)
                                          {
                                              return LDC161XOffsetConfigRaw::from_frequency_mhz(freq_offset.offset_mhz,
                                                                                                raw_config.fref_mhz)
                                                  .value;
                                          }},
                               channel_config.offset_config);

                raw_config.offset.at<LDC161XOffsetRegFields::kOffset>() = offset;

                // --- IDRIVE ---
                raw_config.idrive.at<LDC161XDriveCurrentRegFields::kIDrive>() = channel_config.drive_current.value;
            }

            // --- RCOUNT ---
            // Distribute remaining conversion time equally among channels
            const float channel_conversion_time = conversion_time_left_us / static_cast<float>(tkNumChannels);

            for (uint8_t i = 0; i < tkNumChannels; ++i)
            {
                LDC161XChannelConfigRaw&    raw_config = results[i];
                const LDC161XTraits::RCount rcount     = calculate_rcount(channel_conversion_time, raw_config.fref_mhz);
                raw_config.rcount.at<LDC161XRCountRegFields::kRCount>() = rcount;
                assert(rcount >= LDC161XTraits::skMinRCount && "Calculated rcount is less than allowed minimum");
            }

            return results;
        }

        [[nodiscard]] static constexpr LDC161XSensorConfigRaw<tkNumChannels> get_raw_config(const SensorConfigT& config)
        {
            LDC161XSensorConfigRaw<tkNumChannels> raw_config;

            // --- MUX CONFIG ---
            raw_config.mux_config.template at<LDC161XMuxConfigRegFields::kAutoScanEn>() = (tkNumChannels > 1) ? 1 : 0;
            switch (tkNumChannels)
            {
                case 1:
                case 2:
                    raw_config.mux_config.template at<LDC161XMuxConfigRegFields::kRRSequence>() =
                        static_cast<uint8_t>(LDC161XRRSequence::kCh0Ch1);
                    break;
                case 3:
                    raw_config.mux_config.template at<LDC161XMuxConfigRegFields::kRRSequence>() =
                        static_cast<uint8_t>(LDC161XRRSequence::kCh0Ch1Ch2);
                    break;
                case 4:
                    raw_config.mux_config.template at<LDC161XMuxConfigRegFields::kRRSequence>() =
                        static_cast<uint8_t>(LDC161XRRSequence::kCh0Ch1Ch2Ch3);
                    break;
                default:
                    break;
            }
            raw_config.mux_config.template at<LDC161XMuxConfigRegFields::kDeglitchEn>() =
                static_cast<uint8_t>(config.deglitch_bandwidth);

            // --- ERROR CONFIG ---
            // enable all error reporting by default
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kURErr2Out>() = 1;
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kORErr2Out>() = 1;
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kWDErr2Out>() = 1;
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kAHErr2Out>() = 1;
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kALErr2Out>() = 1;

            if (config.interrupt_config.has_value())
            {
                const LDC161XInterruptConfig& int_config = config.interrupt_config.value();

                raw_config.error_config.template at<LDC161XErrorConfigRegFields::kURErr2Int>() =
                    int_config.int_on_under_range ? 1 : 0;
                raw_config.error_config.template at<LDC161XErrorConfigRegFields::kORErr2Int>() =
                    int_config.int_on_over_range ? 1 : 0;
                raw_config.error_config.template at<LDC161XErrorConfigRegFields::kWDErr2Int>() =
                    int_config.int_on_watchdog ? 1 : 0;
                raw_config.error_config.template at<LDC161XErrorConfigRegFields::kAHErr2Int>() =
                    int_config.int_on_amplitude_high ? 1 : 0;
                raw_config.error_config.template at<LDC161XErrorConfigRegFields::kALErr2Int>() =
                    int_config.int_on_amplitude_low ? 1 : 0;
                raw_config.error_config.template at<LDC161XErrorConfigRegFields::kZCErr2Int>() =
                    int_config.int_on_zero_count ? 1 : 0;
                raw_config.error_config.template at<LDC161XErrorConfigRegFields::kDRDY2Int>() =
                    int_config.int_on_data_ready ? 1 : 0;
            }

            // --- CONFIG ---
            raw_config.device_config.template at<LDC161XConfigRegFields::kActiveChannel>() = 0;  // Start with channel 0
            raw_config.device_config.template at<LDC161XConfigRegFields::kSleepModeEn>()   = 0;  // Disable sleep mode
            raw_config.device_config.template at<LDC161XConfigRegFields::kRPOverrideEn>() =
                config.enable_rp_override ? 1 : 0;
            raw_config.device_config.template at<LDC161XConfigRegFields::kSensorActivateSel>() =
                static_cast<uint8_t>(config.sensor_activation_mode);
            raw_config.device_config.template at<LDC161XConfigRegFields::kAutoAmpDis>() =
                config.auto_amplitude_en ? 0 : 1;
            raw_config.device_config.template at<LDC161XConfigRegFields::kRefClkSrc>() =
                std::holds_alternative<LDC161XClockSourceExternalClock>(config.clock_source) ? 1 : 0;
            raw_config.device_config.template at<LDC161XConfigRegFields::kIntBDis>() =
                config.interrupt_config.has_value() ? 0 : 1;
            raw_config.device_config.template at<LDC161XConfigRegFields::kHighCurrentDrv>() =
                (tkNumChannels == 1 && config.high_current_drive_en) ? 1 : 0;

            // --- CHANNEL CONFIG ---
            raw_config.channels = get_channel_config(config);

            return raw_config;
        }
    };
}  // namespace valle