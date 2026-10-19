add_library(engine_systems STATIC generic_systems.cpp render_systems.cpp entity_interpolator.cpp animation_system.cpp client_predictor.cpp)
add_library(rtype::engine_systems ALIAS engine_systems)
target_include_directories(engine_systems PUBLIC ${RTYPE_INCLUDE_DIRS})
target_link_libraries(engine_systems PUBLIC rtype::engine_ecs rtype::engine_ports PRIVATE rtype::hardening)
