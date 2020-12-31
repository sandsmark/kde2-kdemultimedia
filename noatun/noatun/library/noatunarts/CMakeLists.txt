get_target_property(ARTSFLOW_INCLUDE_PATHS kde2::artsflow INTERFACE_INCLUDE_DIRECTORIES)
#separate_arguments(ARTSFLOW_INCLUDE_PATHS)

set(ARTSFLOW_INCLUDES "")
foreach(path ${ARTSFLOW_INCLUDE_PATHS})
    set(ARTSFLOW_INCLUDES -I${path} ${ARTSFLOW_INCLUDES})
endforeach()

add_custom_command(
    OUTPUT noatunarts.h noatunarts.cc
    COMMAND mcopidl ${ARTSFLOW_INCLUDES} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/noatunarts.idl
    )

set(noatun_libarts_SRCS
    noatunarts.cc fft.c Equalizer_impl.cpp 
    FFTScopes.cpp StereoEffectStack_impl.cpp 
    StereoVolumeControl_impl.cpp Session_impl.cpp
    )

kde2_library(noatunarts
    SOURCES
        ${noatun_libarts_SRCS}
    LIBS
        kde2::artskde
        kde2::artsgui
        PNG::PNG
    )
