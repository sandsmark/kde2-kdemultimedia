########### next target ###############

cmake_minimum_required(VERSION 3.1.0)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
project(kde2-kdemultimedia
    VERSION 2.2.2
    )

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(kde2_configure)

find_package(Qt2 REQUIRED)

find_package(ZLIB REQUIRED)
find_package(kdelibs2 REQUIRED)
find_package(DCOP REQUIRED)
find_package(kdecore REQUIRED)
find_package(kdeui REQUIRED)
find_package(kssl REQUIRED)
find_package(kdesu REQUIRED)
find_package(kio REQUIRED)
find_package(ksycoca REQUIRED)
find_package(mcop REQUIRED)
find_package(qtmcop REQUIRED)
find_package(artsflow REQUIRED)
find_package(kmedia2 REQUIRED)
find_package(kfile REQUIRED)
find_package(artskde REQUIRED)
find_package(PNG REQUIRED)

create_kde2_config_header()

add_subdirectory(arts/gui)
add_subdirectory(noatun/noatun)
