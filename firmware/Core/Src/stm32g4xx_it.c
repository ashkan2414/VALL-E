/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"
#include "stm32g4xx_it.h"

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/

/*
  * @brief This function handles Non maskable interrupt.
  */
__attribute__((weak)) void NMI_Handler(void)
{
   while (1)
  {
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
__attribute__((weak)) void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Memory management fault.
  */
__attribute__((weak)) void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
__attribute__((weak)) void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
__attribute__((weak)) void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
__attribute__((weak)) void SVC_Handler(void)
{
}

/**
  * @brief This function handles Debug monitor.
  */
__attribute__((weak)) void DebugMon_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
__attribute__((weak)) void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
__attribute__((weak)) void SysTick_Handler(void)
{
}
