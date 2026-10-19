option(RTYPE_WERROR "Promouvoir les warnings en erreurs" ON)
set(RTYPE_SANITIZE "" CACHE STRING "Sanitizers : liste séparée par des virgules (address,undefined | thread)")

add_library(rtype_hardening INTERFACE)
add_library(rtype::hardening ALIAS rtype_hardening)

if(MSVC)
  target_compile_options(rtype_hardening INTERFACE /W4 /permissive- /GS /guard:cf)
  target_link_options(rtype_hardening INTERFACE /NXCOMPAT /DYNAMICBASE /GUARD:CF)
  if(RTYPE_WERROR)
    target_compile_options(rtype_hardening INTERFACE /WX)
  endif()
else()
  target_compile_options(rtype_hardening INTERFACE
    -Wall -Wextra -Wpedantic -pedantic-errors
    -Wconversion -Wsign-conversion -Wshadow -Wold-style-cast -Wcast-align
    -Wnull-dereference -Wdouble-promotion -Wformat=2 -Wuninitialized
    -fstack-protector-strong
    $<$<CONFIG:Release>:-D_FORTIFY_SOURCE=2>)
  target_link_options(rtype_hardening INTERFACE
    -Wl,-z,relro -Wl,-z,now
    $<$<CONFIG:Release>:-s>)
  if(RTYPE_WERROR)
    target_compile_options(rtype_hardening INTERFACE -Werror)
  endif()
endif()

if(RTYPE_SANITIZE)
  if(MSVC)
    message(FATAL_ERROR "RTYPE_SANITIZE n'est pas supporté sous MSVC")
  endif()
  if(RTYPE_SANITIZE MATCHES "thread" AND RTYPE_SANITIZE MATCHES "address")
    message(FATAL_ERROR "RTYPE_SANITIZE : thread et address sont incompatibles")
  endif()
  target_compile_options(rtype_hardening INTERFACE -fsanitize=${RTYPE_SANITIZE} -fno-omit-frame-pointer)
  target_link_options(rtype_hardening INTERFACE -fsanitize=${RTYPE_SANITIZE})
  message(STATUS "Sanitizers : ${RTYPE_SANITIZE}")
endif()
