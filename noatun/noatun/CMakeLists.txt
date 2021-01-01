add_subdirectory(library)

set(noatun_main_SRCS
    main.cpp joinhandler.cpp joinhandler_skel.cpp
)
kde2_kidl(noatun_main_SRCS
    joinhandler.h
    )

kde2_kinit_executable(noatun
    SOURCES
        ${noatun_main_SRCS}
    LIBS
        kde2::noatun
    )

install(FILES noatun.desktop DESTINATION "${KDE2_APPSDIR}")

