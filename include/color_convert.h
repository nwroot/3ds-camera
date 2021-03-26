#pragma once

#include <stdint.h>
#include <stddef.h>

void rgb565_to_rgb888(uint8_t *src, uint8_t *dst, size_t width, size_t height);
void rgb565_to_bgr888(uint8_t *src, uint8_t *dst, size_t width, size_t height);
void rgb888_to_bgr888(uint8_t *src, uint8_t *dst, size_t width, size_t height);
