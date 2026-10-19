add_library(engine_foundation STATIC
    vec2.cpp
    result.cpp)
add_library(rtype::engine_foundation ALIAS engine_foundation)
target_include_directories(engine_foundation PUBLIC ${RTYPE_INCLUDE_DIRS})
target_compile_features(engine_foundation PUBLIC cxx_std_20)
target_link_libraries(engine_foundation PRIVATE rtype::hardening)
