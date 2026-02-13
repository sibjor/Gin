# Install script for directory: C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Project")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Projekt/Datorspel/Gin/build/vendored/SDL_mixer/external/ogg-build/ogg.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Projekt/Datorspel/Gin/build/Debug/ogg-0.dll")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Projekt/Datorspel/Gin/build/vendored/SDL_mixer/external/ogg-build/ogg.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/SDL3_mixer/html" TYPE FILE FILES
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/framing.html"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/index.html"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/oggstream.html"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/ogg-multiplex.html"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/fish_xiph_org.png"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/multiplex1.png"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/packets.png"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/pages.png"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/stream.png"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/vorbisword2.png"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/white-ogg.png"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/white-xifish.png"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/rfc3533.txt"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/rfc5334.txt"
    "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/skeleton.html"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/SDL3_mixer/html" TYPE DIRECTORY FILES "C:/Projekt/Datorspel/Gin/vendored/SDL_mixer/external/ogg/doc/libogg")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Projekt/Datorspel/Gin/build/vendored/SDL_mixer/external/ogg-build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
