add_library(engine_networking_codec STATIC
    ack_window.cpp
    byte_order.cpp
    byte_reader.cpp
    byte_writer.cpp
    sequence_number.cpp)
add_library(rtype::engine_networking_codec ALIAS engine_networking_codec)
target_include_directories(engine_networking_codec PUBLIC ${RTYPE_INCLUDE_DIRS})
target_compile_features(engine_networking_codec PUBLIC cxx_std_20)
target_link_libraries(engine_networking_codec PUBLIC rtype::engine_foundation PRIVATE rtype::hardening)
