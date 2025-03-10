function(create_kde2_config_header)
    include(CheckIncludeFiles)
    include(CheckFunctionExists)
    include(CheckStructHasMember)
    include(CheckCSourceCompiles)
    include(CheckSymbolExists)

    check_include_files(malloc.h HAVE_MALLOC_H)
    check_include_files(string.h HAVE_STRING_H)
    check_include_files(unistd.h HAVE_UNISTD_H)
    check_include_files(sys/stat.h HAVE_SYS_STAT_H)
    check_include_files(sys/stat.h HAVE_S_ISSOCK)
    check_include_files(sys/param.h HAVE_SYS_PARAM_H)
    check_include_files(sys/select.h HAVE_SYS_SELECT_H)
    check_include_files(sys/param.h HAVE_SYS_PARAM_H)
    check_include_files(sys/mount.h HAVE_SYS_MOUNT_H)
    check_include_files(float.h HAVE_FLOAT_H)
    check_include_files(mntent.h HAVE_MNTENT_H)
    check_include_files(limits.h HAVE_LIMITS_H)
    check_include_files(sys/acl.h ACL_H)
    check_include_files(sys/time.h HAVE_SYS_TIME_H)
    check_include_files(time.h HAVE_TIME_H)
    check_include_files(memory.h HAVE_MEMORY_H)
    check_include_files(alloca.h HAVE_ALLOCA_H)
    check_include_files(argz.h HAVE_ARGZ_H)
    check_include_files(crypt.h HAVE_CRYPT)
    check_include_files(sys/mman.h HAVE_SYS_MMAN_H)
    check_include_files(cups/cups.h HAVE_CUPS)
    check_include_files(dirent.h HAVE_DIRENT_H)
    check_include_files(dlfcn.h HAVE_DLFCN_H)
    check_include_files(fnmatch.h HAVE_FNMATCH_H)
    check_include_files(fstab.h HAVE_FSTAB_H)
    check_include_files(inttypes.h HAVE_INTTYPES_H)
    check_include_files(libutil.h HAVE_LIBUTIL_H)
    check_include_files(linux/socket.h HAVE_LINUX_SOCKET_H)
    check_include_files(locale.h HAVE_LOCALE_H)
    check_include_files(sys/soundcard.h HAVE_SYS_SOUNDCARD_H)
    check_include_files(sys/socket.h HAVE_SYS_SOCKET_H)
    check_include_files(paths.h HAVE_PATHS_H)
    check_include_files(pty.h HAVE_PTY_H)
    check_include_files(stdint.h HAVE_STDINT_H)
    check_include_files(strings.h HAVE_STRINGS_H)

    check_function_exists("getdomainname" HAVE_GETDOMAINNAME_PROTO)
    check_function_exists("gethostname" HAVE_GETHOSTNAME_PROTO)
    check_function_exists("setenv" HAVE_SETENV)
    check_function_exists("unsetenv" HAVE_UNSETENV)
    check_function_exists("stpcpy" HAVE_STPCPY)
    check_function_exists("getaddrinfo" HAVE_GETADDRINFO)
    check_function_exists("random" HAVE_RANDOM)
    check_function_exists("seteuid" HAVE_SETEUID)
    check_function_exists("vsnprintf" HAVE_VSNPRINTF)
    check_function_exists("gettimeofday" HAVE_SYS_TIME_H)
    check_function_exists("isinf" HAVE_FUNC_ISINF)
    check_function_exists("isnan" HAVE_FUNC_ISNAN)
    check_function_exists("finite" HAVE_FUNC_FINITE)
    check_function_exists("setmntent" HAVE_SETMNTENT)
    list(APPEND CMAKE_REQUIRED_LIBRARIES acl)
    check_function_exists("acl_get_tag_type" HAVE_ACL_GET_TAG_TYPE)
    check_function_exists("mkstemp" HAVE_MKSTEMP)
    check_function_exists("mkstemps" HAVE_MKSTEMPS)
    check_function_exists("munmap" HAVE_MUNMAP)
    check_function_exists("fork" HAVE_FORK)
    check_function_exists("freeaddrinfo" HAVE_FREEADDRINFO)
    check_function_exists("gai_strerror" HAVE_GAI_STRERROR)
    check_function_exists("getcwd" HAVE_GETCWD)
    check_function_exists("getdomainname" HAVE_GETDOMAINNAME)
    check_function_exists("getgroups" HAVE_GETGROUPS)
    check_function_exists("gethostbyname2" HAVE_GETHOSTBYNAME2)
    check_function_exists("gethostbyname2_r" HAVE_GETHOSTBYNAME2_R)
    check_function_exists("gethostbyname_r" HAVE_GETHOSTBYNAME_R)
    check_function_exists("getnameinfo" HAVE_GETNAMEINFO)
    check_function_exists("getpagesize" HAVE_GETPAGESIZE)
    check_function_exists("getpeername" HAVE_GETPEERNAME)
    check_function_exists("getpt" HAVE_GETPT)
    check_function_exists("getsockname" HAVE_GETSOCKNAME)
    check_function_exists("getsockopt" HAVE_GETSOCKOPT)
    check_function_exists("grantpt" HAVE_GRANTPT)
    check_function_exists("inet_ntop" HAVE_INET_NTOP)
    check_function_exists("inet_pton" HAVE_INET_PTON)
    check_function_exists("initgroups" HAVE_INITGROUPS)
    check_function_exists("putenv" HAVE_PUTENV)
    #check_function_exists("res_init" HAVE_RES_INIT)
    check_function_exists("unlockpt" HAVE_UNLOCKPT)
    check_function_exists("usleep" HAVE_USLEEP)

    check_symbol_exists("res_init" "resolv.h" HAVE_ALLOCA)
    check_symbol_exists("mmap" "sys/mman.h" HAVE_MMAP)
    if(HAVE_INITGROUPS)
        set(HAVE_INITGROUPS_PROTO ON)
    endif()

    if (HAVE_ALLOCA_H)
        check_symbol_exists("alloca" "alloca.h" HAVE_ALLOCA)
    endif()

    if(ALSA_FOUND)
        check_include_files(alsa/asoundlib.h HAVE_ALSA_ASOUNDLIB_H)
        check_symbol_exists(snd_pcm_resume "alsa/pcm.h" HAVE_SND_PCM_RESUME)
        find_library(HAVE_LIBASOUND2 libasound.so.2)
        find_library(HAVE_LIBASOUND libasound.so.1)
    endif()

    check_symbol_exists("bzCompress" "bzlib.h" BZ2_NO_PREFIX)
    if(NOT BZ2_NO_PREFIX)
        set(NEED_BZ2_PREFIX ON)
    endif()

    check_symbol_exists("LC_MESSAGES" "locale.h" HAVE_LC_MESSAGES)

    check_struct_has_member("struct addrinfo" ai_addrlen "netdb.h" HAVE_STRUCT_ADDRINFO LANGUAGE C)
    check_struct_has_member("struct sockaddr_in6" sin6_port "netinet/in.h" HAVE_SOCKADDR_IN6 LANGUAGE C)
    check_struct_has_member("struct sockaddr_in6" sin6_scope_id "netinet/in.h" HAVE_SOCKADDR_IN6_SCOPE_ID LANGUAGE C)
    check_struct_has_member("struct sockaddr" sa_len "sys/socket.h" HAVE_SOCKADDR_SA_LEN LANGUAGE C)
    check_struct_has_member("struct tm" tm_sec "time.h;sys/time.h" TIME_WITH_SYS_TIME LANGUAGE C)

    # Couldn't get the more automatic things to work
    check_cxx_source_compiles("#include <sys/socket.h>
                                int main(int argc, char *argv[]) { struct ucred cred; }"
                                HAVE_STRUCT_UCRED)

    if(ACL_H AND HAVE_ACL_GET_TAG_TYPE)
        set(USE_POSIX_ACL TRUE CACHE BOOL "Using posix ACL")
    endif()

    if(HAVE_SYS_TIME_H AND HAVE_TIME_H)
        set(TIME_WITH_SYS_TIME TRUE CACHE BOOL "Define if you can safely include both <sys/time.h> and <time.h>")
    endif()

    if (Threads_FOUND)
        check_include_files(pthread.h HAVE_LIBPTHREAD)
    endif()

    set(KDE_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION})
    set(KDE_COMPILING_OS ${CMAKE_HOST_SYSTEM_NAME})
    set(KDE_DISTRIBUTION_TEXT "Restoration Project")

    # TODO: check this properly
    set(HAVE_IOCTL_INT_ULONGINT_DOTS ON)

    if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86$)|(X86$)")
        # IA32 only
        set(HAVE_X86_SSE ON) # In 2020 for gods sake
    endif()
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
        set(HAVE_X86_FLOAT_INT ON)
    endif()

    configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)
    include_directories(${PROJECT_BINARY_DIR})
    add_definitions(-DHAVE_CONFIG_H)
endfunction()

