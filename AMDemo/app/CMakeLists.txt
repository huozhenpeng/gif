
add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp
             src/main/cpp/dgif_lib.c
             src/main/cpp/egif_lib.c
             src/main/cpp/gif_err.c
             src/main/cpp/gif_hash.c
             src/main/cpp/gifalloc.c
             src/main/cpp/quantize.c
              )

find_library( # Sets the name of the path variable.
                jnigraphics-lib

                # Specifies the name of the NDK library that
                # you want CMake to locate.
                jnigraphics )



find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )



target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       ${jnigraphics-lib}
                       )