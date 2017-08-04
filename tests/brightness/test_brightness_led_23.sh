#!/bin/bash
# Test setting LED brightness to 0x0023

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0023

$X52CLI bri LED 0x0023

verify_output
