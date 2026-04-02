#pragma once

#include <cstdint>

#include "stm32g4xx_ll_exti.h"

#if defined(EXTI_IMR1_IM16)
#define VALLE_PLATFORM_EXTI_HAS_LINE_16 1
#endif /* EXTI_IMR1_IM16 */
#define VALLE_PLATFORM_EXTI_HAS_LINE_17 1
#if defined(EXTI_IMR1_IM18)
#define VALLE_PLATFORM_EXTI_HAS_LINE_18 1
#endif /* EXTI_IMR1_IM18 */
#define VALLE_PLATFORM_EXTI_HAS_LINE_19 1
#if defined(EXTI_IMR1_IM20)
#define VALLE_PLATFORM_EXTI_HAS_LINE_20 1
#endif /* EXTI_IMR1_IM20 */
#if defined(EXTI_IMR1_IM21)
#define VALLE_PLATFORM_EXTI_HAS_LINE_21 1
#endif /* EXTI_IMR1_IM21 */
#if defined(EXTI_IMR1_IM22)
#define VALLE_PLATFORM_EXTI_HAS_LINE_22 1
#endif /* EXTI_IMR1_IM22 */
#define VALLE_PLATFORM_EXTI_HAS_LINE_23 1
#if defined(EXTI_IMR1_IM24)
#define VALLE_PLATFORM_EXTI_HAS_LINE_24 1
#endif /* EXTI_IMR1_IM24 */
#if defined(EXTI_IMR1_IM25)
#define VALLE_PLATFORM_EXTI_HAS_LINE_25 1
#endif /* EXTI_IMR1_IM25 */
#if defined(EXTI_IMR1_IM26)
#define VALLE_PLATFORM_EXTI_HAS_LINE_26 1
#endif /* EXTI_IMR1_IM26 */
#if defined(EXTI_IMR1_IM27)
#define VALLE_PLATFORM_EXTI_HAS_LINE_27 1
#endif /* EXTI_IMR1_IM27 */
#if defined(EXTI_IMR1_IM28)
#define VALLE_PLATFORM_EXTI_HAS_LINE_28 1
#endif /* EXTI_IMR1_IM28 */
#if defined(EXTI_IMR1_IM29)
#define VALLE_PLATFORM_EXTI_HAS_LINE_29 1
#endif /* EXTI_IMR1_IM29 */
#if defined(EXTI_IMR1_IM30)
#define VALLE_PLATFORM_EXTI_HAS_LINE_30 1
#endif /* EXTI_IMR1_IM30 */
#if defined(EXTI_IMR1_IM31)
#define VALLE_PLATFORM_EXTI_HAS_LINE_31 1
#endif /* EXTI_IMR1_IM31 */

#if defined(EXTI_IMR2_IM32)
#define VALLE_PLATFORM_EXTI_HAS_LINE_32 1
#endif /* EXTI_IMR2_IM32 */
#if defined(EXTI_IMR2_IM33)
#define VALLE_PLATFORM_EXTI_HAS_LINE_33 1
#endif /* EXTI_IMR2_IM33 */
#if defined(EXTI_IMR2_IM34)
#define VALLE_PLATFORM_EXTI_HAS_LINE_34 1
#endif /* EXTI_IMR2_IM34 */
#if defined(EXTI_IMR2_IM35)
#define VALLE_PLATFORM_EXTI_HAS_LINE_35 1
#endif /* EXTI_IMR2_IM35 */
#if defined(EXTI_IMR2_IM36)
#define VALLE_PLATFORM_EXTI_HAS_LINE_36 1
#endif /* EXTI_IMR2_IM36 */
#if defined(EXTI_IMR2_IM37)
#define VALLE_PLATFORM_EXTI_HAS_LINE_37 1
#endif /* EXTI_IMR2_IM37 */
#if defined(EXTI_IMR2_IM38)
#define VALLE_PLATFORM_EXTI_HAS_LINE_38 1
#endif /* EXTI_IMR2_IM38 */
#if defined(EXTI_IMR2_IM39)
#define VALLE_PLATFORM_EXTI_HAS_LINE_39 1
#endif /* EXTI_IMR2_IM39 */
#if defined(EXTI_IMR2_IM40)
#define VALLE_PLATFORM_EXTI_HAS_LINE_40 1
#endif /* EXTI_IMR2_IM40 */
#if defined(EXTI_IMR2_IM41)
#define VALLE_PLATFORM_EXTI_HAS_LINE_41 1
#endif /* EXTI_IMR2_IM41 */
#if defined(EXTI_IMR2_IM42)
#define VALLE_PLATFORM_EXTI_HAS_LINE_42 1
#endif /* EXTI_IMR2_IM42 */

namespace valle::platform
{
    enum class ExtiLineId : uint8_t
    {
        kLine0  = 0,
        kLine1  = 1,
        kLine2  = 2,
        kLine3  = 3,
        kLine4  = 4,
        kLine5  = 5,
        kLine6  = 6,
        kLine7  = 7,
        kLine8  = 8,
        kLine9  = 9,
        kLine10 = 10,
        kLine11 = 11,
        kLine12 = 12,
        kLine13 = 13,
        kLine14 = 14,
        kLine15 = 15,
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_16
        kLine16 = 16,
#endif /* EXTI_IMR1_IM16 */
        kLine17 = 17,
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_18
        kLine18 = 18,
#endif /* EXTI_IMR1_IM18 */
        kLine19 = 19,
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_20
        kLine20 = 20,
#endif /* EXTI_IMR1_IM20 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_21
        kLine21 = 21,
#endif /* EXTI_IMR1_IM21 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_22
        kLine22 = 22,
#endif /* EXTI_IMR1_IM22 */
        kLine23 = 23,
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_24
        kLine24 = 24,
#endif /* EXTI_IMR1_IM24 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_25
        kLine25 = 25,
#endif /* EXTI_IMR1_IM25 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_26
        kLine26 = 26,
#endif /* EXTI_IMR1_IM26 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_27
        kLine27 = 27,
#endif /* EXTI_IMR1_IM27 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_28
        kLine28 = 28,
#endif /* EXTI_IMR1_IM28 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_29
        kLine29 = 29,
#endif /* EXTI_IMR1_IM29 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_30
        kLine30 = 30,
#endif /* EXTI_IMR1_IM30 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_31
        kLine31 = 31,
#endif /* EXTI_IMR1_IM31 */

#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_32
        kLine32 = 32,
#endif /* EXTI_IMR2_IM32 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_33
        kLine33 = 33,
#endif /* EXTI_IMR2_IM33 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_34
        kLine34 = 34,
#endif /* EXTI_IMR2_IM34 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_35
        kLine35 = 35,
#endif /* EXTI_IMR2_IM35 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_36
        kLine36 = 36,
#endif /* EXTI_IMR2_IM36 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_37
        kLine37 = 37,
#endif /* EXTI_IMR2_IM37 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_38
        kLine38 = 38,
#endif /* EXTI_IMR2_IM38 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_39
        kLine39 = 39,
#endif /* EXTI_IMR2_IM39 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_40
        kLine40 = 40,
#endif /* EXTI_IMR2_IM40 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_41
        kLine41 = 41,
#endif /* EXTI_IMR2_IM41 */
#ifdef VALLE_PLATFORM_EXTI_HAS_LINE_42
        kLine42 = 42,
#endif /* EXTI_IMR2_IM42 */

    };

}  // namespace valle::platform