# FastLED
# https://github.com/FastLED/FastLED
# MIT License

cmake_minimum_required(VERSION 3.5)

set(FastLED_SRCS
  src/colorpalettes.cpp
  src/colorutils.cpp
  src/FastLED.cpp
  src/hsv2rgb.cpp
  src/lib8tion.cpp
  src/noise.cpp
  )

idf_component_register(SRCS ${FastLED_SRCS}
                      INCLUDE_DIRS "src"
                      REQUIRES esp_timer)

project(FastLED)
