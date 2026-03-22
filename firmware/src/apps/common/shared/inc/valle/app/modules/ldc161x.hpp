#pragma once

#include <delegate/delegate.h>

#include <jungles/bitfields.hpp>
#include <numbers>
#include <span>

#include "valle/base/system_build/device.hpp"
#include "valle/math/converters.hpp"
#include "valle/system/timing.hpp"

namespace valle::app
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

    enum class LDC161XChannel : uint8_t
    {
        kChannel0 = 0,
        kChannel1 = 1,
        kChannel2 = 2,
        kChannel3 = 3,
    };

    template <uint8_t tkNumChannels>
    concept CLDC161XValidNumChannels = (tkNumChannels >= 1 && tkNumChannels <= 4);

    template <LDC161XChannel tkChannel, uint8_t tkNumChannels>
    concept CLDC161XValidChannel = (static_cast<uint8_t>(tkChannel) < static_cast<uint8_t>(tkNumChannels));

    inline constexpr uint8_t ldc161x_get_channel_index_from_channel(const LDC161XChannel channel)
    {
        return static_cast<uint8_t>(channel);  // Convert to 0-based index for arrays
    }

    inline constexpr LDC161XChannel ldc161x_get_channel_from_index(const uint8_t index)
    {
        return static_cast<LDC161XChannel>(index);  // Convert back to 0-based channel enum
    }

    template <LDC161XChannel tkChannel>
    constexpr uint8_t kLDC161XChannelIndex = ldc161x_get_channel_index_from_channel(tkChannel);

    template <uint8_t tkChannelIndex>
    constexpr LDC161XChannel kLDC161XChannelFromIndex = ldc161x_get_channel_from_index(tkChannelIndex);

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

        static constexpr uint16_t       skMinRCount                           = 9U;  // > 8
        static constexpr uint16_t       skMinSettleCount                      = 4U;  // > 3
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

        static constexpr uint8_t skClockDividersRegReservedValue = 0;
        static constexpr uint8_t skErrorConfigRegReserved1Value  = 0;
        static constexpr uint8_t skErrorConfigRegReserved2Value  = 0;
        static constexpr uint8_t skConfigRegReserved1Value       = 0;
        static constexpr uint8_t skConfigRegReserved2Value       = 1;
        static constexpr uint8_t skMuxConfigRegReservedValue     = 0b00'0100'0001;
        static constexpr uint8_t skResetRegReservedValue         = 0;
        static constexpr uint8_t skDriveCurrentRegReservedValue  = 0;

        // From Section 8.1.5, table 42
        static constexpr std::array<uint16_t, 32> skIDriveTableUA = {
            16,  18,  20,  23,  28,  32,  40,  46,  52,  59,  72,  82,  95,   110,  127,  146,
            169, 195, 212, 244, 297, 342, 424, 489, 551, 635, 763, 880, 1017, 1173, 1355, 1563};

        template <LDC161XChannel tkChannel>
        static constexpr LDC161XReg skChannelDataMSBReg = static_cast<LDC161XReg>(
            static_cast<uint8_t>(LDC161XReg::kDataChannel0MSB) + (kLDC161XChannelIndex<tkChannel> * 2));

        template <LDC161XChannel tkChannel>
        static constexpr LDC161XReg skChannelDataLSBReg = static_cast<LDC161XReg>(
            static_cast<uint8_t>(LDC161XReg::kDataChannel0LSB) + (kLDC161XChannelIndex<tkChannel> * 2));

        template <LDC161XChannel tkChannel>
        static constexpr LDC161XReg skChannelRCountReg = static_cast<LDC161XReg>(
            static_cast<uint8_t>(LDC161XReg::kRCountChannel0) + kLDC161XChannelIndex<tkChannel>);

        template <LDC161XChannel tkChannel>
        static constexpr LDC161XReg skChannelOffsetReg = static_cast<LDC161XReg>(
            static_cast<uint8_t>(LDC161XReg::kOffsetChannel0) + kLDC161XChannelIndex<tkChannel>);

        template <LDC161XChannel tkChannel>
        static constexpr LDC161XReg skChannelSettleCountReg = static_cast<LDC161XReg>(
            static_cast<uint8_t>(LDC161XReg::kSettleCountChannel0) + kLDC161XChannelIndex<tkChannel>);

        template <LDC161XChannel tkChannel>
        static constexpr LDC161XReg skChannelClockDividerReg = static_cast<LDC161XReg>(
            static_cast<uint8_t>(LDC161XReg::kClockDividerChannel0) + kLDC161XChannelIndex<tkChannel>);

        template <LDC161XChannel tkChannel>
        static constexpr LDC161XReg skChannelDriveCurrentReg = static_cast<LDC161XReg>(
            static_cast<uint8_t>(LDC161XReg::kDriveCurrentChannel0) + kLDC161XChannelIndex<tkChannel>);

        [[nodiscard]] static constexpr LDC161XReg get_channel_data_msb_reg(const LDC161XChannel channel)
        {
            return static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kDataChannel0MSB) +
                                           ((ldc161x_get_channel_index_from_channel(channel) % 4) * 2));
        }

        [[nodiscard]] static constexpr LDC161XReg get_channel_data_lsb_reg(const LDC161XChannel channel)
        {
            return static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kDataChannel0LSB) +
                                           ((ldc161x_get_channel_index_from_channel(channel) % 4) * 2));
        }

        [[nodiscard]] static constexpr LDC161XReg get_channel_r_count_reg(const LDC161XChannel channel)
        {
            return static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kRCountChannel0) +
                                           (ldc161x_get_channel_index_from_channel(channel) % 4));
        }

        [[nodiscard]] static constexpr LDC161XReg get_channel_offset_reg(const LDC161XChannel channel)
        {
            return static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kOffsetChannel0) +
                                           (ldc161x_get_channel_index_from_channel(channel) % 4));
        }

        [[nodiscard]] static constexpr LDC161XReg get_channel_settle_count_reg(const LDC161XChannel channel)
        {
            return static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kSettleCountChannel0) +
                                           (ldc161x_get_channel_index_from_channel(channel) % 4));
        }

        [[nodiscard]] static constexpr LDC161XReg get_channel_clock_divider_reg(const LDC161XChannel channel)
        {
            return static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kClockDividerChannel0) +
                                           (ldc161x_get_channel_index_from_channel(channel) % 4));
        }

        [[nodiscard]] static constexpr LDC161XReg get_channel_drive_current_reg(const LDC161XChannel channel)
        {
            return static_cast<LDC161XReg>(static_cast<uint8_t>(LDC161XReg::kDriveCurrentChannel0) +
                                           (ldc161x_get_channel_index_from_channel(channel) % 4));
        }
    };

    // ==============================================================================
    // I2C Interface
    // ==============================================================================
    enum class LDC161XSensorModuleI2CInterfaceCallbackStatus : uint8_t
    {
        kComplete,
        kAborted,
        kError
    };

    using LDC161XSensorModuleI2CInterfaceCallback =
        delegate::Delegate<void, LDC161XSensorModuleI2CInterfaceCallbackStatus>;

    template <typename TDerived, typename TConfig>
    class LDC161XSensorModuleI2CInterfaceX
    {
    public:
        using ConfigT = TConfig;

        [[nodiscard]] bool init(const ConfigT& config)
        {
            return static_cast<TDerived*>(this)->init_impl(config);
        }

        void set_async_callback(LDC161XSensorModuleI2CInterfaceCallback&& callback)
        {
            static_cast<TDerived*>(this)->set_async_callback_impl(
                std::forward<LDC161XSensorModuleI2CInterfaceCallback>(callback));
        }

        [[nodiscard]] bool transaction_in_progress() const
        {
            return static_cast<const TDerived*>(this)->transaction_in_progress_impl();
        }

        template <uint8_t tkWriteBytes>
        [[nodiscard]] bool register_write_blocking(const std::span<const std::byte, tkWriteBytes>& write_data,
                                                   const system::TimeoutMillis                     timeout_ms)
        {
            return static_cast<TDerived*>(this)->template register_write_blocking_impl<tkWriteBytes>(write_data,
                                                                                                     timeout_ms);
        }

        template <uint8_t tkReadBytes>
        [[nodiscard]] bool register_read_blocking(const LDC161XReg                         reg,
                                                  const std::span<std::byte, tkReadBytes>& data_out,
                                                  const system::TimeoutMillis              timeout_ms)
        {
            static_assert(sizeof(reg) == 1, "Register address must be 1 byte");

            return static_cast<TDerived*>(this)->template register_read_blocking_impl<tkReadBytes>(
                reg, data_out, timeout_ms);
        }

        template <uint8_t tkWriteBytes>
        [[nodiscard]] bool register_write_async(const std::span<const std::byte, tkWriteBytes>& write_data)
        {
            return static_cast<TDerived*>(this)->template register_write_async_impl<tkWriteBytes>(write_data);
        }

        template <uint8_t tkReadBytes>
        [[nodiscard]] bool register_read_async(const LDC161XReg reg)
        {
            static_assert(sizeof(reg) == 1, "Register address must be 1 byte");
            return static_cast<TDerived*>(this)->template register_read_async_impl<tkReadBytes>(reg);
        }

        template <uint8_t tkReadBytes>
        [[nodiscard]] std::optional<std::span<const std::byte>> get_async_read_data() const
        {
            return static_cast<const TDerived*>(this)->template get_async_read_data_impl<tkReadBytes>();
        }

        // ---------------------------------------------------------------------------
        // overloads
        // ---------------------------------------------------------------------------
        [[nodiscard]] bool register_write_blocking(const LDC161XReg            reg,
                                                   const uint16_t&             raw_val,
                                                   const system::TimeoutMillis timeout_ms)
        {
            // Force Big-Endian (MSB first) for transmission
            std::array<std::byte, 1 + sizeof(uint16_t)> write_data = {
                static_cast<std::byte>(reg),                    // Register address
                static_cast<std::byte>((raw_val >> 8) & 0xFF),  // MSB
                static_cast<std::byte>(raw_val & 0xFF)          // LSB
            };

            return register_write_blocking<write_data.size()>(std::span(write_data), timeout_ms);
        }

        [[nodiscard]] std::optional<uint16_t> register_read_blocking(const LDC161XReg            reg,
                                                                     const system::TimeoutMillis timeout_ms)
        {
            std::array<std::byte, sizeof(uint16_t)> buffer{};
            if (!register_read_blocking<sizeof(uint16_t)>(reg, std::span(buffer), timeout_ms))
            {
                return std::nullopt;
            }

            // Reconstruct Big-Endian payload back into STM32 format
            const uint16_t raw_val = (static_cast<uint16_t>(buffer[0]) << 8) | static_cast<uint16_t>(buffer[1]);
            return static_cast<uint16_t>(raw_val);
        }

        [[nodiscard]] bool register_write_async(const LDC161XReg reg, const uint16_t& value)
        {
            // Force Big-Endian (MSB first) for transmission
            const uint16_t                              raw_val    = static_cast<uint16_t>(value);
            std::array<std::byte, 1 + sizeof(uint16_t)> write_data = {
                static_cast<std::byte>(reg),                    // Register address
                static_cast<std::byte>((raw_val >> 8) & 0xFF),  // MSB
                static_cast<std::byte>(raw_val & 0xFF)          // LSB
            };

            return register_write_async<write_data.size()>(std::span(write_data));
        }

        [[nodiscard]] bool register_read_async(const LDC161XReg reg)
        {
            return register_read_async<sizeof(uint16_t)>(reg);
        }

        [[nodiscard]] std::optional<uint16_t> get_async_read_value() const
        {
            const auto buffer =
                static_cast<const TDerived*>(this)->template get_async_read_data_impl<sizeof(uint16_t)>();
            if (!buffer.has_value())
            {
                return std::nullopt;
            }

            expect(buffer->size() == sizeof(uint16_t),
                   "Async read buffer size does not match expected size for uint16_t");

            const uint16_t raw_val = (static_cast<uint16_t>((*buffer)[0]) << 8) | static_cast<uint16_t>((*buffer)[1]);
            return static_cast<uint16_t>(raw_val);
        }
    };

    // ==============================================================================
    // CONFIGURATION
    // ==============================================================================
    // ------------------------------------------------------------------------------
    // CHANNEL CONFIGURATION
    // ------------------------------------------------------------------------------

    struct LDC161XCoilConfig
    {
        float inductance_uh;   // L in microhenries
        float capacitance_pf;  // C in picofarads
        float rp_kohm;         // R_p in kilohms
        float q_factor;        // Unitless

        [[nodiscard]] inline constexpr float calculate_freq_mhz() const
        {
            const double l_henries      = static_cast<double>(inductance_uh) * 1.0e-6;    // Convert uH to H
            const double c_farads       = static_cast<double>(capacitance_pf) * 1.0e-12;  // Convert pF to F
            const double q_factor_local = static_cast<double>(q_factor);

            // Intermediate calculation: sqrt(L * C)
            const double sqrt_lc = std::sqrt(l_henries * c_farads);

            // Part 1: Ideal resonant frequency term [ 1 / (2*pi*sqrt(LC)) ]
            const double ideal_f = 1.0 / (2.0 * std::numbers::pi * sqrt_lc);

            // Part 2: The correction factor terms inside the square root
            // Term A: 1/Q^2
            const double term_a = 1.0 / (q_factor_local * q_factor_local);

            // Term B: (5 * 10^-9) / (Q * sqrt(LC))
            const double term_b = 5.0e-9 / (q_factor_local * sqrt_lc);

            // Combine terms inside the root
            const double correction_term = std::sqrt(1.0 - term_a - term_b);

            return static_cast<float>(ideal_f * correction_term * 1.0e-6);  // Convert to MHz
        }
    };

    // DRIVE CURRENT CONFIGURATION
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

    // OFFSET CONFIG
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

    // CHANNEL CONFIGURATION
    struct LDC161XChannelConfig
    {
        LDC161XCoilConfig    coil_config{};
        LDC161XIDriveCurrent drive_current{};  // Used if RP Override is enabled
        LDC161XOffsetConfig  offset_config{};
    };

    // ------------------------------------------------------------------------------
    // SENSOR CONFIGURATION
    // ------------------------------------------------------------------------------

    // CLOCK SOURCE CONFIGURATION
    struct LDC161XClockSourceInternalOscillator
    {
        static constexpr float skFClkMHz = 43.4F;
    };

    struct LDC161XClockSourceExternalClock
    {
        float fclk_mhz;  // Usually 40MHz
    };
    using ClockSource = std::variant<LDC161XClockSourceInternalOscillator, LDC161XClockSourceExternalClock>;

    // INTERRUPT CONFIG
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

    // ERROR REPORTING CONFIG
    struct LDC161XErrorReportingConfig
    {
        bool report_under_range : 1    = true;
        bool report_over_range : 1     = true;
        bool report_watchdog : 1       = true;
        bool report_amplitude_high : 1 = true;
        bool report_amplitude_low : 1  = true;
    };

    // SENSOR ACTIVATION MODE
    enum class LDC161XSensorActivationMode : uint8_t
    {
        kFullCurrentMode = 0,
        kLowPowerMode    = 1,
    };

    // SENSOR CONFIG
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
    // DATA STRUCTURES
    // ---------------------------------------------------------------------------
    struct LDC161XStatus
    {
        uint8_t err_channel : 1        = false;
        uint8_t err_underrun : 1       = false;
        uint8_t err_overrun : 1        = false;
        uint8_t err_watchdog : 1       = false;
        uint8_t err_amplitude_high : 1 = false;
        uint8_t err_amplitude_low : 1  = false;
        uint8_t err_zero_count : 1     = false;
        uint8_t drdy : 1               = false;
        uint8_t undread_conv0 : 1      = false;
        uint8_t undread_conv1 : 1      = false;
        uint8_t undread_conv2 : 1      = false;
        uint8_t undread_conv3 : 1      = false;
    };

    struct LDC161XDataRaw
    {
        bool err_underrun : 1  = false;
        bool err_overrun : 1   = false;
        bool err_watchdog : 1  = false;
        bool err_amplitude : 1 = false;

        uint32_t value = 0;
    };

    struct LDC161XDataFrequency
    {
        bool err_underrun : 1  = false;
        bool err_overrun : 1   = false;
        bool err_watchdog : 1  = false;
        bool err_amplitude : 1 = false;

        float frequency_mhz = 0.0F;
    };

    struct LDC161XErrorCallbackResult
    {
    };

    struct LDC161XReadStatusCallbackResult
    {
        LDC161XStatus status;
    };

    struct LDC161XReadDataRawCallbackResult
    {
        LDC161XDataRaw data;
    };

    struct LDC161XReadFrequencyCallbackResult
    {
        LDC161XDataFrequency data;
    };

    template <uint8_t tkNumChannels>
    struct LDC161XReadDataRawMultiCallbackResult
    {
        std::array<LDC161XDataRaw, tkNumChannels> data;
    };

    template <uint8_t tkNumChannels>
    struct LDC161XReadFrequencyMultiCallbackResult
    {
        std::array<LDC161XDataFrequency, tkNumChannels> data;
    };

    struct LDC161XReadStatusDataRawCallbackResult
    {
        LDC161XStatus  status;
        LDC161XDataRaw data;
    };

    struct LDC161XReadStatusFrequencyCallbackResult
    {
        LDC161XStatus        status;
        LDC161XDataFrequency data;
    };

    template <uint8_t tkNumChannels>
    struct LDC161XReadStatusDataRawMultiCallbackResult
    {
        LDC161XStatus                             status;
        std::array<LDC161XDataRaw, tkNumChannels> data;
    };

    template <uint8_t tkNumChannels>
    struct LDC161XReadStatusFrequencyMultiCallbackResult
    {
        LDC161XStatus                                   status;
        std::array<LDC161XDataFrequency, tkNumChannels> data;
    };

    template <uint8_t tkNumChannels>
    using LDC161XReadCallbackResult = std::variant<LDC161XErrorCallbackResult,
                                                   LDC161XReadStatusCallbackResult,
                                                   LDC161XReadDataRawCallbackResult,
                                                   LDC161XReadFrequencyCallbackResult,
                                                   LDC161XReadDataRawMultiCallbackResult<tkNumChannels>,
                                                   LDC161XReadFrequencyMultiCallbackResult<tkNumChannels>,
                                                   LDC161XReadStatusDataRawCallbackResult,
                                                   LDC161XReadStatusFrequencyCallbackResult,
                                                   LDC161XReadStatusDataRawMultiCallbackResult<tkNumChannels>,
                                                   LDC161XReadStatusFrequencyMultiCallbackResult<tkNumChannels>>;

    template <uint8_t tkNumChannels>
    using LDC161XReadCallback = delegate::Delegate<void, const LDC161XReadCallbackResult<tkNumChannels>&>;

    // ---------------------------------------------------------------------------
    // MODULE DRIVER
    // ---------------------------------------------------------------------------

    template <typename TI2CInterface, uint8_t tkNumChannels>
        requires(CLDC161XValidNumChannels<tkNumChannels>)
    class LDC161XSensorModuleX
    {
    public:
        using I2CInterfaceT                    = TI2CInterface;
        static constexpr uint8_t skNumChannels = tkNumChannels;

        using ConfigT       = LDC161XSensorModuleConfigX<typename I2CInterfaceT::ConfigT, tkNumChannels>;
        using SensorConfigT = LDC161XSensorConfig<tkNumChannels>;
        static constexpr system::TimeoutMillis skDefaultTimeout = system::TimeoutMillis(50);

        using ErrorCallbackResultT                    = LDC161XErrorCallbackResult;
        using ReadStatusCallbackResultT               = LDC161XReadStatusCallbackResult;
        using ReadDataRawCallbackResultT              = LDC161XReadDataRawCallbackResult;
        using ReadFrequencyCallbackResultT            = LDC161XReadFrequencyCallbackResult;
        using ReadDataRawMultiCallbackResultT         = LDC161XReadDataRawMultiCallbackResult<tkNumChannels>;
        using ReadFrequencyMultiCallbackResultT       = LDC161XReadFrequencyMultiCallbackResult<tkNumChannels>;
        using ReadStatusDataRawCallbackResultT        = LDC161XReadStatusDataRawCallbackResult;
        using ReadStatusFrequencyCallbackResultT      = LDC161XReadStatusFrequencyCallbackResult;
        using ReadStatusDataRawMultiCallbackResultT   = LDC161XReadStatusDataRawMultiCallbackResult<tkNumChannels>;
        using ReadStatusFrequencyMultiCallbackResultT = LDC161XReadStatusFrequencyMultiCallbackResult<tkNumChannels>;

        using ReadCallbackResultT = LDC161XReadCallbackResult<tkNumChannels>;
        using ReadCallbackT       = LDC161XReadCallback<tkNumChannels>;

        using InjectDevices = typename GetInjectDevices<I2CInterfaceT>::type;
        using DependDevices = typename GetAdditionalDependDevices<I2CInterfaceT>::type;

    private:
        static constexpr size_t skNumAsyncChainReadRegsMax = 1 + (tkNumChannels * 2);  // upper limit

        enum AsyncChainReadTag : uint8_t
        {
            kStatus,
            kDataRawSingleChannel,
            kFrequencySingleChannel,
            kDataRawMultiChannel,
            kFrequencyMultiChannel,
            kStatusDataRawSingleChannel,
            kStatusFrequencySingleChannel,
            kStatusDataRawMultiChannel,
            kStatusFrequencyMultiChannel
        };

        struct AsyncChainReadConfig
        {
            AsyncChainReadTag                                  tag = AsyncChainReadTag::kStatus;
            std::array<LDC161XReg, skNumAsyncChainReadRegsMax> regs{};
            uint8_t                                            num_regs = 0;
        };

        struct AsyncChainReadContext
        {
            AsyncChainReadConfig                             config{};
            std::array<uint16_t, skNumAsyncChainReadRegsMax> reg_values{};
            uint32_t                                         current_reg_index = 0;
        };

        static constexpr AsyncChainReadConfig skAsyncChainReadStatusConfig{.regs = {LDC161XReg::kStatus}};

        template <AsyncChainReadTag tkTag>
        static consteval AsyncChainReadConfig get_chain_read_config_for_multi_channel_data_raw()
        {
            std::array<LDC161XReg, skNumChannels * 2> regs{};  // Each channel has MSB and LSB
            for (uint8_t i = 0; i < tkNumChannels; ++i)
            {
                const LDC161XChannel channel = ldc161x_get_channel_from_index(i);
                regs[i * 2]                  = LDC161XTraits::get_channel_data_msb_reg(channel);
                regs[i * 2 + 1]              = LDC161XTraits::get_channel_data_lsb_reg(channel);
            }
            return AsyncChainReadConfig{.tag = tkTag, .regs = regs, .num_regs = static_cast<uint8_t>(regs.size())};
        }

        template <LDC161XChannel tkChannel>
        static constexpr AsyncChainReadConfig skAsyncChainReadDataRawSingleChannelConfig{
            .tag      = AsyncChainReadTag::kDataRawSingleChannel,
            .regs     = {LDC161XTraits::skChannelDataMSBReg<tkChannel>, LDC161XTraits::skChannelDataLSBReg<tkChannel>},
            .num_regs = 2};

        template <LDC161XChannel tkChannel>
        static constexpr AsyncChainReadConfig skAsyncChainReadFrequencySingleChannelConfig{
            .tag      = AsyncChainReadTag::kFrequencySingleChannel,
            .regs     = {LDC161XTraits::skChannelDataMSBReg<tkChannel>, LDC161XTraits::skChannelDataLSBReg<tkChannel>},
            .num_regs = 2};

        static constexpr AsyncChainReadConfig skAsyncChainReadDataRawMultiChannelConfig =
            get_chain_read_config_for_multi_channel_data_raw<AsyncChainReadTag::kDataRawMultiChannel>();
        static constexpr AsyncChainReadConfig skAsyncChainReadFrequencyMultiChannelConfig =
            get_chain_read_config_for_multi_channel_data_raw<AsyncChainReadTag::kFrequencyMultiChannel>();

        template <AsyncChainReadTag tkTag>
        static constexpr AsyncChainReadConfig get_chain_read_config_for_multi_channel_status_data()
        {
            std::array<LDC161XReg, 1 + (tkNumChannels * 2)> regs{};  // Status + (MSB+LSB) for each channel
            regs[0] = LDC161XReg::kStatus;
            for (uint8_t i = 0; i < tkNumChannels; ++i)
            {
                const LDC161XChannel channel = ldc161x_get_channel_from_index(i);
                regs[1 + (i * 2)]            = LDC161XTraits::get_channel_data_msb_reg(channel);
                regs[1 + (i * 2) + 1]        = LDC161XTraits::get_channel_data_lsb_reg(channel);
            }
            return AsyncChainReadConfig{.tag = tkTag, .regs = regs, .num_regs = static_cast<uint8_t>(regs.size())};
        }

        template <LDC161XChannel tkChannel>
        static constexpr auto skAsyncChainReadStatusDataRawSingleChannelConfig =
            AsyncChainReadConfig{.tag      = AsyncChainReadTag::kStatusDataRawSingleChannel,
                                 .regs     = {LDC161XReg::kStatus,
                                              LDC161XTraits::skChannelDataMSBReg<tkChannel>,
                                              LDC161XTraits::skChannelDataLSBReg<tkChannel>},
                                 .num_regs = 3};

        template <LDC161XChannel tkChannel>
        static constexpr auto skAsyncChainReadStatusFrequencySingleChannelConfig =
            AsyncChainReadConfig{.tag      = AsyncChainReadTag::kStatusFrequencySingleChannel,
                                 .regs     = {LDC161XReg::kStatus,
                                              LDC161XTraits::skChannelDataMSBReg<tkChannel>,
                                              LDC161XTraits::skChannelDataLSBReg<tkChannel>},
                                 .num_regs = 3};

        static constexpr auto skAsyncChainReadStatusDataRawMultiChannelConfig =
            get_chain_read_config_for_multi_channel_status_data<AsyncChainReadTag::kStatusDataRawMultiChannel>();

        static constexpr auto skAsyncChainReadStatusFrequencyMultiChannelConfig =
            get_chain_read_config_for_multi_channel_status_data<AsyncChainReadTag::kStatusFrequencyMultiChannel>();

        I2CInterfaceT                                                m_i2c{};
        LDC161XSensorConfigRaw<tkNumChannels>                        m_config{};
        std::array<LinearConverter<uint32_t, double>, tkNumChannels> m_ch_freq_converters{};
        ReadCallbackT                                                m_read_callback{};
        std::optional<AsyncChainReadContext>                         m_async_context{};

    public:
        template <typename... TArgs>
        explicit LDC161XSensorModuleX(TArgs&&... args) : m_i2c(std::forward<TArgs>(args)...)
        {
        }

        [[nodiscard]] bool init(const ConfigT& config)
        {
            // TODO abort transactions

            if (!m_i2c.init(config.i2c_config))
            {
                return false;
            }

            m_i2c.set_async_callback([this](const LDC161XSensorModuleI2CInterfaceCallbackStatus status)
                                     { this->async_transaction_callback(status); });

            m_config             = get_raw_config(config.sensor_config);
            m_ch_freq_converters = get_all_channel_freq_converters(m_config);

            // Reset device to default state
            if (!reset())
            {
                return false;
            }

            // Verify device is connected and responding with correct IDs before configuring
            if (!verify_device_connected())
            {
                return false;
            }

            if (!configure())
            {
                return false;
            }

            return true;
        }

        void set_async_read_callback(ReadCallbackT&& callback)
        {
            m_read_callback = std::forward<ReadCallbackT>(callback);
        }

        [[nodiscard]] bool reset()
        {
            // TODO abort transactions

            LDC161XResetReg reset_reg;
            reset_reg.at<LDC161XResetRegFields::kResetDev>() = 1U;
            reset_reg.at<LDC161XResetRegFields::kReserved>() = LDC161XTraits::skResetRegReservedValue;
            if (!m_i2c.register_write_blocking(LDC161XReg::kDeviceReset, reset_reg.serialize(), skDefaultTimeout))
            {
                return false;
            }

            system::TimingContext::delay_ms(10);  // Allow time for reset to complete
            return true;
        }

        [[nodiscard]] std::optional<uint16_t> read_manufacturer_id(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            if (m_i2c.transaction_in_progress())
            {
                return std::nullopt;  // Avoid starting a new read if a transaction is already in progress
            }

            const auto manufacturer_id = m_i2c.register_read_blocking(LDC161XReg::kReadManufacturerID, timeout_ms);
            if (!manufacturer_id.has_value())
            {
                return std::nullopt;
            }

            return LDC161XManufacturerIDReg(std::move(manufacturer_id.value()))
                .at<LDC161XManufacturerIDRegFields::kManufacturerID>();
        }

        [[nodiscard]] bool verify_manufacturer_id(const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            if (m_i2c.transaction_in_progress())
            {
                return false;  // Avoid starting a new read if a transaction is already in progress
            }

            auto manufacturer_id = read_manufacturer_id(timeout_ms);
            return manufacturer_id.has_value() && manufacturer_id.value() == LDC161XTraits::skManufacturerID;
        }

        [[nodiscard]] std::optional<uint16_t> read_device_id(const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            if (m_i2c.transaction_in_progress())
            {
                return std::nullopt;  // Avoid starting a new read if a transaction is already in progress
            }

            const auto device_id = m_i2c.register_read_blocking(LDC161XReg::kReadDeviceID, timeout_ms);
            if (!device_id.has_value())
            {
                return std::nullopt;
            }
            return LDC161XDeviceIDReg(std::move(device_id.value())).at<LDC161XDeviceIDRegFields::kDeviceID>();
        }

        [[nodiscard]] bool verify_device_id(const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            if (m_i2c.transaction_in_progress())
            {
                return false;  // Avoid starting a new read if a transaction is already in progress
            }

            auto device_id = read_device_id(timeout_ms);
            return device_id.has_value() && device_id.value() == LDC161XTraits::skDeviceID;
        }

        [[nodiscard]] bool verify_device_connected(const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            if (m_i2c.transaction_in_progress())
            {
                return false;  // Avoid starting a new read if a transaction is already in progress
            }

            return verify_manufacturer_id(timeout_ms) && verify_device_id(timeout_ms);
        }

        [[nodiscard]] bool configure()
        {
            if (m_i2c.transaction_in_progress())
            {
                return false;  // Avoid starting a new read if a transaction is already in progress
            }

            // NOTE: THE ORDER OF REGISTER WRITES IS IMPORTANT

            // Put to sleep
            LDC161XConfigReg config_reg;
            config_reg.at<LDC161XConfigRegFields::kActiveChannel>()     = 0U;
            config_reg.at<LDC161XConfigRegFields::kSleepModeEn>()       = 1U;  // Put device to sleep while configuring
            config_reg.at<LDC161XConfigRegFields::kRPOverrideEn>()      = 0U;
            config_reg.at<LDC161XConfigRegFields::kSensorActivateSel>() = 1U;
            config_reg.at<LDC161XConfigRegFields::kAutoAmpDis>()        = 0U;
            config_reg.at<LDC161XConfigRegFields::kRefClkSrc>()         = 0U;
            config_reg.at<LDC161XConfigRegFields::kReserved1>()         = LDC161XTraits::skConfigRegReserved1Value;
            config_reg.at<LDC161XConfigRegFields::kIntBDis>()           = 0U;
            config_reg.at<LDC161XConfigRegFields::kHighCurrentDrv>()    = 0U;
            config_reg.at<LDC161XConfigRegFields::kReserved2>()         = LDC161XTraits::skConfigRegReserved2Value;

            if (!m_i2c.register_write_blocking(LDC161XReg::kConfig, config_reg.serialize(), skDefaultTimeout))
            {
                return false;
            }

            {
                const auto device_config_read = m_i2c.register_read_blocking(LDC161XReg::kConfig, skDefaultTimeout);
                if (!device_config_read.has_value() || device_config_read.value() != config_reg.serialize())
                {
                    return false;
                }
            }

            // Configure channels
            if constexpr (tkNumChannels >= 1)
            {
                if (!configure_channel<LDC161XChannel::kChannel0>())
                {
                    return false;
                }
            }

            if constexpr (tkNumChannels >= 2)
            {
                if (!configure_channel<LDC161XChannel::kChannel1>())
                {
                    return false;
                }
            }

            if constexpr (tkNumChannels >= 3)
            {
                if (!configure_channel<LDC161XChannel::kChannel2>())
                {
                    return false;
                }
            }

            if constexpr (tkNumChannels >= 4)
            {
                if (!configure_channel<LDC161XChannel::kChannel3>())
                {
                    return false;
                }
            }

            // MUX CONFIG
            if (!m_i2c.register_write_blocking(
                    LDC161XReg::kMuxConfig, m_config.mux_config.serialize(), skDefaultTimeout))
            {
                return false;
            }

            const auto mux_config_read = m_i2c.register_read_blocking(LDC161XReg::kMuxConfig, skDefaultTimeout);
            if (!mux_config_read.has_value() || mux_config_read.value() != m_config.mux_config.serialize())
            {
                return false;
            }

            // ERROR CONFIG
            if (!m_i2c.register_write_blocking(
                    LDC161XReg::kErrorConfig, m_config.error_config.serialize(), skDefaultTimeout))
            {
                return false;
            }
            const auto error_config_read = m_i2c.register_read_blocking(LDC161XReg::kErrorConfig, skDefaultTimeout);
            if (!error_config_read.has_value() || error_config_read.value() != m_config.error_config.serialize())
            {
                return false;
            }

            // DEVICE CONFIG (exit sleep mode last)
            if (!m_i2c.register_write_blocking(
                    LDC161XReg::kConfig, m_config.device_config.serialize(), skDefaultTimeout))
            {
                return false;
            }

            const auto device_config_read = m_i2c.register_read_blocking(LDC161XReg::kConfig, skDefaultTimeout);
            if (!device_config_read.has_value() || device_config_read.value() != m_config.device_config.serialize())
            {
                return false;
            }

            return true;
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels>)
        [[nodiscard]] bool configure_channel()
        {
            if (m_i2c.transaction_in_progress())
            {
                return false;  // Avoid starting a new read if a transaction is already in progress
            }

            const auto& chan_config = m_config.channels[kLDC161XChannelIndex<tkChannel>];

            // Write CLOCK DIVIDER
            if (!m_i2c.register_write_blocking(LDC161XTraits::skChannelClockDividerReg<tkChannel>,
                                               chan_config.clock_divider.serialize(),
                                               skDefaultTimeout))
            {
                return false;
            }

            const auto clock_divider_read =
                m_i2c.register_read_blocking(LDC161XTraits::skChannelClockDividerReg<tkChannel>, skDefaultTimeout);
            if (!clock_divider_read.has_value() || clock_divider_read.value() != chan_config.clock_divider.serialize())
            {
                return false;
            }

            // Write RCOUNT
            if (!m_i2c.register_write_blocking(
                    LDC161XTraits::skChannelRCountReg<tkChannel>, chan_config.rcount.serialize(), skDefaultTimeout))
            {
                return false;
            }

            const auto rcount_read =
                m_i2c.register_read_blocking(LDC161XTraits::skChannelRCountReg<tkChannel>, skDefaultTimeout);
            if (!rcount_read.has_value() || rcount_read.value() != chan_config.rcount.serialize())
            {
                return false;
            }

            // Write OFFSET
            if (!m_i2c.register_write_blocking(
                    LDC161XTraits::skChannelOffsetReg<tkChannel>, chan_config.offset.serialize(), skDefaultTimeout))
            {
                return false;
            }

            const auto offset_read =
                m_i2c.register_read_blocking(LDC161XTraits::skChannelOffsetReg<tkChannel>, skDefaultTimeout);
            if (!offset_read.has_value() || offset_read.value() != chan_config.offset.serialize())
            {
                return false;
            }

            // Write SETTLE COUNT
            if (!m_i2c.register_write_blocking(LDC161XTraits::skChannelSettleCountReg<tkChannel>,
                                               chan_config.settle_count.serialize(),
                                               skDefaultTimeout))
            {
                return false;
            }

            const auto settle_count_read =
                m_i2c.register_read_blocking(LDC161XTraits::skChannelSettleCountReg<tkChannel>, skDefaultTimeout);
            if (!settle_count_read.has_value() || settle_count_read.value() != chan_config.settle_count.serialize())
            {
                return false;
            }

            // Write DRIVE CURRENT
            if (!m_i2c.register_write_blocking(LDC161XTraits::skChannelDriveCurrentReg<tkChannel>,
                                               chan_config.idrive.serialize(),
                                               skDefaultTimeout))
            {
                return false;
            }

            const auto idrive_read =
                m_i2c.register_read_blocking(LDC161XTraits::skChannelDriveCurrentReg<tkChannel>, skDefaultTimeout);
            if (!idrive_read.has_value() || idrive_read.value() != chan_config.idrive.serialize())
            {
                return false;
            }

            return true;
        }

        [[nodiscard]] std::optional<LDC161XStatus> read_status(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            if (m_i2c.transaction_in_progress())
            {
                return std::nullopt;  // Avoid starting a new read if a transaction is already in progress
            }

            const auto status_raw = m_i2c.register_read_blocking(LDC161XReg::kStatus, timeout_ms);
            if (!status_raw.has_value())
            {
                return std::nullopt;
            }
            return status_from_reg(std::move(status_raw.value()));
        }

        [[nodiscard]] bool read_status_async()
        {
            return start_async_chain_read(skAsyncChainReadStatusConfig);
        }

        [[nodiscard]] std::optional<LDC161XDataRaw> read_data_raw_blocking(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
            requires(tkNumChannels == 1)
        {
            return read_data_raw_blocking_impl<LDC161XChannel::kChannel0>(timeout_ms);
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels> && tkNumChannels > 1)
        [[nodiscard]] std::optional<LDC161XDataRaw> read_data_raw_blocking(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            return read_data_raw_blocking_impl<tkChannel>(timeout_ms);
        }

        [[nodiscard]] bool read_data_raw_async()
            requires(tkNumChannels == 1)
        {
            return read_data_raw_async_impl<LDC161XChannel::kChannel0>();
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels> && tkNumChannels > 1)
        [[nodiscard]] bool read_data_raw_async()
        {
            return read_data_raw_async_impl<tkChannel>();
        }

        [[nodiscard]] std::optional<std::array<LDC161XDataRaw, skNumChannels>> read_data_raw_blocking_multi(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
            requires(tkNumChannels > 1)
        {
            if (m_i2c.transaction_in_progress())
            {
                return std::nullopt;  // Avoid starting a new read if a transaction is already in progress
            }

            std::array<LDC161XDataRaw, skNumChannels> results{};

            // C++20 Templated Lambda to unpack the index sequence
            auto read_sequence = [&]<size_t... Is>(std::index_sequence<Is...>) -> bool
            {
                // Fold expression over the indices (Is = 0, 1, 2...)
                // The && operator guarantees strict left-to-right evaluation.
                return ((
                            [&]()
                            {
                                constexpr auto channel  = kLDC161XChannelFromIndex<static_cast<uint8_t>(Is)>;
                                const auto     data_opt = read_data_raw_blocking<channel>(timeout_ms);
                                if (!data_opt.has_value())
                                {
                                    return false;  // Early exit on failure
                                }

                                results[Is] = data_opt.value();  // Directly use the compile-time index!
                                return true;
                            }()) &&
                        ...);
            };

            // Call the lambda with a generated sequence (e.g., 0, 1 for skNumChannels = 2)
            if (!read_sequence(std::make_index_sequence<skNumChannels>{}))
            {
                return std::nullopt;
            }

            return results;
        }

        [[nodiscard]] bool read_data_raw_multi_async()
            requires(tkNumChannels > 1)
        {
            return start_async_chain_read(skAsyncChainReadDataRawMultiChannelConfig);
        }

        [[nodiscard]] float frequency_from_raw_data(const LDC161XChannel channel, const uint32_t data_raw) const
        {
            return static_cast<float>(
                m_ch_freq_converters.at(ldc161x_get_channel_index_from_channel(channel)).convert(data_raw));
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels>)
        [[nodiscard]] float frequency_from_raw_data(const uint32_t data_raw) const
        {
            return static_cast<float>(m_ch_freq_converters.at(kLDC161XChannelIndex<tkChannel>).convert(data_raw));
        }

        [[nodiscard]] LDC161XDataFrequency frequency_from_raw_data(const LDC161XChannel  channel,
                                                                   const LDC161XDataRaw& data_raw) const
        {
            return LDC161XDataFrequency{.err_underrun  = data_raw.err_underrun,
                                        .err_overrun   = data_raw.err_overrun,
                                        .err_watchdog  = data_raw.err_watchdog,
                                        .err_amplitude = data_raw.err_amplitude,
                                        .frequency_mhz = frequency_from_raw_data(channel, data_raw.value)};
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels>)
        [[nodiscard]] LDC161XDataFrequency frequency_from_raw_data(const LDC161XDataRaw& data_raw) const
        {
            return LDC161XDataFrequency{.err_underrun  = data_raw.err_underrun,
                                        .err_overrun   = data_raw.err_overrun,
                                        .err_watchdog  = data_raw.err_watchdog,
                                        .err_amplitude = data_raw.err_amplitude,
                                        .frequency_mhz = frequency_from_raw_data<tkChannel>(data_raw.value)};
        }

        [[nodiscard]] std::optional<LDC161XDataFrequency> read_frequency_mhz_blocking(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
            requires(tkNumChannels == 1)
        {
            return read_frequency_mhz_blocking_impl<LDC161XChannel::kChannel0>(timeout_ms);
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels> && tkNumChannels > 1)
        [[nodiscard]] std::optional<LDC161XDataFrequency> read_frequency_mhz_blocking(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            return read_frequency_mhz_blocking_impl<tkChannel>(timeout_ms);
        }

        [[nodiscard]] bool read_frequency_mhz_async()
            requires(tkNumChannels == 1)
        {
            return read_frequency_mhz_async_impl<LDC161XChannel::kChannel0>();
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels> && tkNumChannels > 1)
        [[nodiscard]] bool read_frequency_mhz_async()
        {
            return read_frequency_mhz_async_impl<tkChannel>();
        }

        [[nodiscard]] std::optional<std::array<LDC161XDataFrequency, skNumChannels>> read_frequency_mhz_blocking_multi(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
            requires(tkNumChannels > 1)
        {
            std::array<LDC161XDataFrequency, skNumChannels> results{};

            // C++20 Templated Lambda to unpack the index sequence
            auto read_sequence = [&]<size_t... Is>(std::index_sequence<Is...>) -> bool
            {
                // Fold expression over the indices (Is = 0, 1, 2...)
                // The && operator guarantees strict left-to-right evaluation.
                return ((
                            [&]()
                            {
                                constexpr auto channel  = kLDC161XChannelFromIndex<static_cast<uint8_t>(Is)>;
                                const auto     data_opt = read_frequency_mhz_blocking<channel>(timeout_ms);
                                if (!data_opt.has_value())
                                {
                                    return false;  // Early exit on failure
                                }

                                results[Is] = data_opt.value();  // Directly use the compile-time index!
                                return true;
                            }()) &&
                        ...);
            };

            // Call the lambda with a generated sequence (e.g., 0, 1 for skNumChannels = 2)
            if (!read_sequence(std::make_index_sequence<skNumChannels>{}))
            {
                return std::nullopt;
            }

            return results;
        }

        [[nodiscard]] bool read_frequency_mhz_multi_async()
            requires(tkNumChannels > 1)
        {
            return start_async_chain_read(skAsyncChainReadFrequencyMultiChannelConfig);
        }

        [[nodiscard]] bool read_status_data_raw_multi_async()
            requires(tkNumChannels > 1)
        {
            return start_async_chain_read(skAsyncChainReadStatusDataRawMultiChannelConfig);
        }

        [[nodiscard]] bool read_status_frequency_mhz_multi_async()
            requires(tkNumChannels > 1)
        {
            return start_async_chain_read(skAsyncChainReadStatusFrequencyMultiChannelConfig);
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels> && tkNumChannels > 1)
        [[nodiscard]] std::optional<LDC161XTraits::IDrive> read_auto_calibrate_drive_current(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            return read_auto_calibrate_drive_current_impl<tkChannel>(timeout_ms);
        }

        [[nodiscard]] std::optional<LDC161XTraits::IDrive> read_auto_calibrate_drive_current(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
            requires(tkNumChannels == 1)
        {
            return read_auto_calibrate_drive_current_impl<LDC161XChannel::kChannel0>(timeout_ms);
        }

        [[nodiscard]] I2CInterfaceT& get_i2c()
        {
            return m_i2c;
        }

    private:
        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels>)
        [[nodiscard]] std::optional<LDC161XDataRaw> read_data_raw_blocking_impl(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            if (m_i2c.transaction_in_progress())
            {
                return std::nullopt;  // Avoid starting a new read if a transaction is already in progress
            }

            const auto data_msb =
                m_i2c.register_read_blocking(LDC161XTraits::skChannelDataMSBReg<tkChannel>, timeout_ms);
            if (!data_msb.has_value())
            {
                return std::nullopt;
            }

            const auto data_lsb =
                m_i2c.register_read_blocking(LDC161XTraits::skChannelDataLSBReg<tkChannel>, timeout_ms);
            if (!data_lsb.has_value())
            {
                return std::nullopt;
            }

            return data_from_reg(data_msb.value(), data_lsb.value());
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels>)
        [[nodiscard]] bool read_data_raw_async_impl()
        {
            return start_async_chain_read(skAsyncChainReadDataRawSingleChannelConfig<tkChannel>);
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels> && tkNumChannels > 1)
        [[nodiscard]] std::optional<LDC161XDataFrequency> read_frequency_mhz_blocking_impl(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            if (m_i2c.transaction_in_progress())
            {
                return std::nullopt;  // Avoid starting a new read if a transaction is already in progress
            }

            const auto raw_data_opt = read_data_raw_blocking<tkChannel>(timeout_ms);
            if (!raw_data_opt.has_value())
            {
                return std::nullopt;
            }
            return frequency_from_raw_data<tkChannel>(raw_data_opt.value());
        }

        template <LDC161XChannel tkChannel>
            requires(CLDC161XValidChannel<tkChannel, tkNumChannels>)
        [[nodiscard]] bool read_frequency_mhz_async_impl()
        {
            return start_async_chain_read(skAsyncChainReadFrequencySingleChannelConfig<tkChannel>);
        }

        template <LDC161XChannel tkChannel>
        [[nodiscard]] std::optional<LDC161XTraits::IDrive> read_auto_calibrate_drive_current_impl(
            const system::TimeoutMillis timeout_ms = skDefaultTimeout)
        {
            auto drive_reg_opt =
                m_i2c.register_read_blocking(LDC161XTraits::skChannelDriveCurrentReg<tkChannel>, timeout_ms);
            if (!drive_reg_opt.has_value())
            {
                return std::nullopt;
            }

            return LDC161XDriveCurrentReg(std::move(drive_reg_opt.value()))
                .at<LDC161XDriveCurrentRegFields::kInitIDrive>();
        }

        // ------------------------------------------------------------------------------
        // DATA CONVERSION HELPERS
        // ------------------------------------------------------------------------------
        [[nodiscard]] static LDC161XStatus status_from_reg(const uint32_t status_raw)
        {
            const auto status_reg = LDC161XStatusReg(status_raw);
            return LDC161XStatus{
                .err_channel        = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kErrChannel>() == 1U),
                .err_underrun       = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kErrUR>() == 1U),
                .err_overrun        = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kErrOR>() == 1U),
                .err_watchdog       = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kErrWD>() == 1U),
                .err_amplitude_high = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kErrAH>() == 1U),
                .err_amplitude_low  = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kErrAL>() == 1U),
                .err_zero_count     = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kErrZC>() == 1U),
                .drdy               = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kDRDY>() == 1U),
                .undread_conv0      = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kUndreadConv0>() == 1U),
                .undread_conv1      = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kUndreadConv1>() == 1U),
                .undread_conv2      = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kUndreadConv2>() == 1U),
                .undread_conv3      = static_cast<bool>(status_reg.at<LDC161XStatusRegFields::kUndreadConv3>() == 1U)};
        }

        [[nodiscard]] static LDC161XDataRaw data_from_reg(const uint16_t msb, const uint16_t lsb)
        {
            LDC161XDataReg data_reg((static_cast<uint32_t>(msb) << 16) | lsb);
            return LDC161XDataRaw{
                .err_underrun  = static_cast<bool>(data_reg.at<LDC161XDataRegFields::kErrUR>() == 1U),
                .err_overrun   = static_cast<bool>(data_reg.at<LDC161XDataRegFields::kErrOR>() == 1U),
                .err_watchdog  = static_cast<bool>(data_reg.at<LDC161XDataRegFields::kErrWD>() == 1U),
                .err_amplitude = static_cast<bool>(data_reg.at<LDC161XDataRegFields::kErrAE>() == 1U),
                .value         = data_reg.at<LDC161XDataRegFields::kData>(),
            };
        }

        // ------------------------------------------------------------------------------
        // ASYNC READ HANDLING
        // ------------------------------------------------------------------------------
        [[nodiscard]] bool start_async_chain_read(const AsyncChainReadConfig& config)
        {
            if (m_i2c.transaction_in_progress())
            {
                return false;  // Avoid starting a new read if a transaction is already in progress
            }

            m_async_context = AsyncChainReadContext{
                .config            = config,
                .reg_values        = {},  // Initialize reg_values to default (e.g., zeros)
                .current_reg_index = 0,
            };

            if (!m_i2c.register_read_async(m_async_context->config.regs[0]))
            {
                m_async_context = std::nullopt;
                return false;
            }

            return true;
        }

        void on_async_result_fail()
        {
            if (m_read_callback)
            {
                m_read_callback(LDC161XErrorCallbackResult{});  // Notify error through the read callback
            }
            m_async_context = std::nullopt;  // Reset context on error
        }

        void on_async_result_complete(ReadCallbackResultT&& result)
        {
            if (m_read_callback)
            {
                m_read_callback(std::forward<ReadCallbackResultT>(result));
            }

            m_async_context = std::nullopt;
        }

        static constexpr LDC161XChannel data_reg_to_channel(const LDC161XReg reg)
        {
            switch (reg)
            {
                case LDC161XReg::kDataChannel0LSB:
                case LDC161XReg::kDataChannel0MSB:
                    return LDC161XChannel::kChannel0;
                case LDC161XReg::kDataChannel1LSB:
                case LDC161XReg::kDataChannel1MSB:
                    return LDC161XChannel::kChannel1;
                case LDC161XReg::kDataChannel2MSB:
                case LDC161XReg::kDataChannel2LSB:
                    return LDC161XChannel::kChannel2;
                case LDC161XReg::kDataChannel3MSB:
                case LDC161XReg::kDataChannel3LSB:
                    return LDC161XChannel::kChannel3;
                default:
                    expect(false, "Invalid data register in data_reg_to_channel");
                    return LDC161XChannel::kChannel0;  // Default to channel 0 on error (should never reach here due to
                                                       // expect)
            }
        }

        ReadCallbackResultT parse_async_result_from_context(const AsyncChainReadContext& context) const
        {
            switch (context.config.tag)
            {
                case AsyncChainReadTag::kStatus:
                {
                    return ReadStatusCallbackResultT{.status = status_from_reg(context.reg_values[0])};
                }
                case AsyncChainReadTag::kDataRawSingleChannel:
                {
                    return ReadDataRawCallbackResultT{.data =
                                                          data_from_reg(context.reg_values[0], context.reg_values[1])};
                }
                case AsyncChainReadTag::kDataRawMultiChannel:
                {
                    std::array<LDC161XDataRaw, skNumChannels> data_array{};
                    for (size_t i = 0; i < tkNumChannels; ++i)
                    {
                        const uint16_t msb = context.reg_values[i * 2];
                        const uint16_t lsb = context.reg_values[i * 2 + 1];
                        data_array[i]      = data_from_reg(msb, lsb);
                    }
                    return ReadDataRawMultiCallbackResultT{.data = data_array};
                }
                case AsyncChainReadTag::kFrequencySingleChannel:
                {
                    const auto data_raw = data_from_reg(context.reg_values[0], context.reg_values[1]);
                    return ReadFrequencyCallbackResultT{
                        .data = frequency_from_raw_data(data_reg_to_channel(context.config.regs[0]), data_raw)};
                }
                case AsyncChainReadTag::kFrequencyMultiChannel:
                {
                    std::array<LDC161XDataFrequency, skNumChannels> freq_array{};
                    for (size_t i = 0; i < tkNumChannels; ++i)
                    {
                        const uint16_t msb = context.reg_values[i * 2];
                        const uint16_t lsb = context.reg_values[i * 2 + 1];
                        const auto     data_raw =
                            data_from_reg(msb, lsb);  // Reuse data_from_reg to parse the raw data and error bits
                        freq_array[i] = frequency_from_raw_data(ldc161x_get_channel_from_index(i), data_raw);
                    }
                    return ReadFrequencyMultiCallbackResultT{.data = freq_array};
                }
                case AsyncChainReadTag::kStatusDataRawSingleChannel:
                {
                    return ReadStatusDataRawCallbackResultT{
                        .status = status_from_reg(context.reg_values[0]),
                        .data   = data_from_reg(context.reg_values[1], context.reg_values[2]),
                    };
                }
                case AsyncChainReadTag::kStatusDataRawMultiChannel:
                {
                    const auto status = status_from_reg(context.reg_values[0]);

                    std::array<LDC161XDataRaw, skNumChannels> data_array{};

                    for (size_t i = 0; i < tkNumChannels; ++i)
                    {
                        const uint16_t data_msb = context.reg_values[i * 2 + 1];
                        const uint16_t data_lsb = context.reg_values[i * 2 + 2];

                        data_array[i] = data_from_reg(data_msb, data_lsb);
                    }

                    return ReadStatusDataRawMultiCallbackResultT{
                        .status = status,
                        .data   = data_array,
                    };
                }
                case AsyncChainReadTag::kStatusFrequencySingleChannel:
                {
                    const auto status   = status_from_reg(context.reg_values[0]);
                    const auto data_raw = data_from_reg(context.reg_values[1], context.reg_values[2]);
                    const auto freq_mhz =
                        frequency_from_raw_data(data_reg_to_channel(context.config.regs[0]), data_raw);

                    return ReadStatusFrequencyCallbackResultT{
                        .status = status,
                        .data   = freq_mhz,
                    };
                }
                case AsyncChainReadTag::kStatusFrequencyMultiChannel:
                {
                    const auto status = status_from_reg(context.reg_values[0]);

                    std::array<LDC161XDataFrequency, skNumChannels> freq_array{};

                    for (size_t i = 0; i < tkNumChannels; ++i)
                    {
                        const uint16_t data_msb = context.reg_values[i * 2 + 1];
                        const uint16_t data_lsb = context.reg_values[i * 2 + 2];
                        const auto     data     = data_from_reg(data_msb, data_lsb);
                        freq_array[i]           = frequency_from_raw_data(ldc161x_get_channel_from_index(i), data);
                    }

                    return ReadStatusFrequencyMultiCallbackResultT{
                        .status = status,
                        .data   = freq_array,
                    };
                }
                default:
                    expect(false, "Unhandled AsyncChainReadTag in parse_async_result_from_context");
                    return ReadCallbackResultT{};  // Return default result on unhandled tag (should never reach here
                                                   // due to expect)
            }
        }

        void async_transaction_callback(const LDC161XSensorModuleI2CInterfaceCallbackStatus status)
        {
            if (!m_read_callback)
            {
                m_async_context = std::nullopt;  // Clear context since we have no callback to handle the result
                return;                          // No callback registered, nothing to do
            }

            if (!m_async_context.has_value())
            {
                on_async_result_fail();  // No valid context for this callback, treat as failure
                return;
            }
            AsyncChainReadContext& context = *m_async_context;  // We know context is valid if we're in the callback

            if (status == LDC161XSensorModuleI2CInterfaceCallbackStatus::kAborted ||
                status == LDC161XSensorModuleI2CInterfaceCallbackStatus::kError)
            {
                on_async_result_fail();
                return;
            }

            const auto read_value = m_i2c.get_async_read_value();
            if (!read_value.has_value())
            {
                on_async_result_fail();
                return;
            }

            context.reg_values[context.current_reg_index] = read_value.value();
            context.current_reg_index++;

            if (context.current_reg_index >= context.config.num_regs)
            {
                // All registers for this context have been read, now process the result
                on_async_result_complete(parse_async_result_from_context(context));
            }
            else
            {
                // Initiate read for the next register in the sequence
                if (!m_i2c.register_read_async(context.config.regs[context.current_reg_index]))
                {
                    on_async_result_fail();
                    return;
                }
            }
        }

        // ------------------------------------------------------------------------------
        // CONFIG CALCULATION UTILITIES
        // ------------------------------------------------------------------------------
        [[nodiscard]] static constexpr float calculate_fsensor_mhz(const LDC161XCoilConfig& coil)
        {
            return coil.calculate_freq_mhz();
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

        [[nodiscard]] static constexpr float calculate_channel_switch_time_us(const float fref_mhz)
        {
            return (5.0F / fref_mhz) + 0.692F;
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

            const float sample_period    = 1.0F / static_cast<float>(config.sample_rate_hz);
            const float sample_period_us = sample_period * 1.0e6F;

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
                expect(raw_config.fref_mhz <= max_fref_mhz,
                       "Resulting fref exceeds maximum allowed reference frequency");

                const float                     max_fin_mhz = raw_config.fref_mhz / 4.0F;
                const LDC161XTraits::FinDivider dynamic_min_fin =
                    (fsensor_mhz >= 8.75F) ? 2U : LDC161XTraits::skMinFinDivider;

                const LDC161XTraits::FinDivider fin_divider =
                    std::clamp(static_cast<LDC161XTraits::FinDivider>(std::ceil(fsensor_mhz / max_fin_mhz)),
                               dynamic_min_fin,
                               LDC161XTraits::skMaxFinDivider);

                raw_config.clock_divider.at<LDC161XClockDividerRegFields::kFinDivider>() = fin_divider;
                raw_config.clock_divider.at<LDC161XClockDividerRegFields::kReserved>() =
                    LDC161XTraits::skClockDividersRegReservedValue;

                const float fin_mhz = fsensor_mhz / static_cast<float>(fin_divider);
                expect(fin_mhz < raw_config.fref_mhz / 4.0F, "Resulting fin exceeds maximum allowed input frequency");

                // --- SETTLE COUNT ---
                const LDC161XTraits::SettleCount min_settle_count =
                    calculate_min_settle_count(channel_config.coil_config.q_factor, raw_config.fref_mhz, fsensor_mhz);

                expect(min_settle_count >= LDC161XTraits::skMinSettleCount,
                       "Calculated minimum settle count is less than allowed minimum");
                raw_config.settle_count.at<LDC161XSettleCountRegFields::kSettleCount>() = min_settle_count;

                const float settle_time_us         = calculate_settle_time_us(min_settle_count, raw_config.fref_mhz);
                const float channel_switch_time_us = calculate_channel_switch_time_us(raw_config.fref_mhz);

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
                raw_config.idrive.at<LDC161XDriveCurrentRegFields::kReserved>() =
                    LDC161XTraits::skDriveCurrentRegReservedValue;
            }

            // --- RCOUNT ---
            // Distribute remaining conversion time equally among channels
            const float channel_conversion_time = conversion_time_left_us / static_cast<float>(tkNumChannels);

            for (uint8_t i = 0; i < tkNumChannels; ++i)
            {
                LDC161XChannelConfigRaw&    raw_config = results[i];
                const LDC161XTraits::RCount rcount     = calculate_rcount(channel_conversion_time, raw_config.fref_mhz);
                raw_config.rcount.at<LDC161XRCountRegFields::kRCount>() = rcount;
                expect(rcount >= LDC161XTraits::skMinRCount, "Calculated rcount is less than allowed minimum");
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
            raw_config.mux_config.template at<LDC161XMuxConfigRegFields::kReserved>() =
                LDC161XTraits::skMuxConfigRegReservedValue;
            raw_config.mux_config.template at<LDC161XMuxConfigRegFields::kDeglitchEn>() =
                static_cast<uint8_t>(config.deglitch_bandwidth);

            // --- ERROR CONFIG ---
            // enable all error reporting by default
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kURErr2Out>() = 1;
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kORErr2Out>() = 1;
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kWDErr2Out>() = 1;
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kAHErr2Out>() = 1;
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kALErr2Out>() = 1;
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kReserved1>() =
                LDC161XTraits::skErrorConfigRegReserved1Value;

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
            raw_config.error_config.template at<LDC161XErrorConfigRegFields::kReserved2>() =
                LDC161XTraits::skErrorConfigRegReserved2Value;

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
            raw_config.device_config.template at<LDC161XConfigRegFields::kReserved1>() =
                LDC161XTraits::skConfigRegReserved1Value;
            raw_config.device_config.template at<LDC161XConfigRegFields::kIntBDis>() =
                config.interrupt_config.has_value() ? 0 : 1;

            if constexpr (tkNumChannels == 1)
            {
                raw_config.device_config.template at<LDC161XConfigRegFields::kHighCurrentDrv>() =
                    config.high_current_drive_en ? 1 : 0;
            }
            else
            {
                // High current drive not supported in multi-channel mode
                raw_config.device_config.template at<LDC161XConfigRegFields::kHighCurrentDrv>() = 0;
            }
            raw_config.device_config.template at<LDC161XConfigRegFields::kReserved2>() =
                LDC161XTraits::skConfigRegReserved2Value;

            // --- CHANNEL CONFIG ---
            raw_config.channels = get_channel_config(config);

            return raw_config;
        }
    };
}  // namespace valle::app