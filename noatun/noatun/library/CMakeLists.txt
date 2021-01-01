add_subdirectory(noatunarts)

set(noatun_lib_SRCS
    pluginmodule.cpp cmodule.cpp downloader.cpp engine.cpp noatunplaylist.cpp
    noatunpref.cpp player.cpp playlistsaver.cpp noatunapp.cpp pluginloader.cpp
    plugin.cpp noatunstdaction.cpp conversion.cpp noatunui.cpp effectview.cpp
    equalizerview.cpp equalizer.cpp effects.cpp mimetypetree.cpp
    stereobuttonaction.cpp
    )

qt2_wrap_moc(noatun_lib_SRCS
    SOURCES
        plugin.h
        plugin_deps.h
    )
qt2_wrap_cpp(noatun_lib_SRCS
    SOURCES
        cmodule.h
        controls.h
        downloader.h
        effects.h
        effectview.h
        engine.h
        equalizer.h
        equalizerview.h
        mimetypetree.h
        noatunapp.h
        noatunplaylist.h
        noatunpref.h
        noatunstdaction.h
        player.h
        pluginmodule.h
        stereobuttonaction.h
)
kde2_library(libnoatun
    SOURCES
        ${noatun_lib_SRCS}
    LIBS
        kde2::kio
        kde2::kmedia2
        kde2::kfile
        kde2::artskde
        kde2::noatunarts
    )

