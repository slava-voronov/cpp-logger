set(HEADER_LIST logger.h)

add_library(Logger INTERFACE)

target_sources(Logger PUBLIC "logger.h" "consumer.h" "consumer.cpp" "producer.h" "loggerimpl.cpp" "logger.cpp" "safequeue.h" "formatter.h")

target_include_directories(Logger INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})