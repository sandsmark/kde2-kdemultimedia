get_target_property(ARTSFLOW_INCLUDE_PATHS kde2::artsflow INTERFACE_INCLUDE_DIRECTORIES)

set(ARTSFLOW_INCLUDES "")
foreach(path ${ARTSFLOW_INCLUDE_PATHS})
    set(ARTSFLOW_INCLUDES -I${path} ${ARTSFLOW_INCLUDES})
endforeach()


add_custom_command(
    OUTPUT artsgui.h artsgui.cc
    COMMAND mcopidl ${ARTSFLOW_INCLUDES} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/common/artsgui.idl
    )
qt2_wrap_cpp(artsgui_MOC
    SOURCES
        kde/kpoti.h
        kde/kpoti_impl.h
        kde/kspinbox_impl.h
        kde/kwidget_impl.h
    )

kde2_library(artsgui
    SOURCES
        artsgui.cc
        kde/kartswidget.cpp
        kde/kbutton_impl.cpp
        kde/khbox_impl.cpp
        kde/klineedit_impl.cpp
        kde/kpoti.cpp
        kde/kpoti_impl.cpp
        kde/kspinbox_impl.cpp
        kde/kvbox_impl.cpp
        kde/kwidget_impl.cpp
        kde/kwidgetrepo.cpp
        ${artsgui_MOC}
    HEADERS
        kde/kartswidget.h
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/kde
    LIBS
        kde2::artsflow
        kde2::kdeui
    )
