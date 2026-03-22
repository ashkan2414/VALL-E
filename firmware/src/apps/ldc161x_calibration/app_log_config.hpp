#pragma once

#include "valle/base/logging/config_base.hpp"

VALLE_DEFINE_LOGGING_CONFIG((valle::LoggingConfig{
    .enabled               = true,
    .max_message_length    = 512,
    .log_level             = LogLevel::kInfo,
    .print_source_location = false,
}));