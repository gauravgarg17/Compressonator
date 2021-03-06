cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

add_executable(CompressonatorCLI-bin "")

target_sources(CompressonatorCLI-bin
               PRIVATE
               Source/CompressonatorCLI.cpp
               Source/CompressonatorCLI_Documentation.h
               ../_Plugins/Common/ATIFormats.h
               ../_Plugins/Common/ATIFormats.cpp
               ../_Plugins/Common/cmdline.h
               ../_Plugins/Common/cmdline.cpp
               ../_Plugins/Common/CMP_FileIO.h
               ../_Plugins/Common/CMP_FileIO.cpp
               ../_Plugins/Common/ModelData.h
               ../_Plugins/Common/ModelData.cpp
               ../_Plugins/Common/PluginInterface.h
               ../_Plugins/Common/PluginManager.h
               ../_Plugins/Common/PluginManager.cpp
               ../_Plugins/Common/query_timer.h
               ../_Plugins/Common/query_timer.cpp
               ../_Plugins/Common/TextureIO.h
               ../_Plugins/Common/TextureIO.cpp
               ../_Plugins/Common/gltf/tiny_gltf2.h
               ../_Plugins/Common/gltf/tiny_gltf2_utils.h
               ../_Plugins/Common/gltf/tiny_gltf2_utils.cpp
               )

target_include_directories(CompressonatorCLI-bin
                           PRIVATE
                           Source
                           ../_Plugins/Common
                           ../_Plugins/Common/json
                           ../_Plugins/Common/gltf
                           ../_Plugins/CMesh/mesh_optimizer
                           ../_Libs/GPU_Decode
                           ../_Libs/CMP_MeshCompressor/Draco/src
                           ../_Libs/CMP_MeshOptimizer
                           ../_Libs/CMP_Math
                           ../../CMP_CompressonatorLib
                           ../../CMP_Framework/
                           ../../CMP_Framework/Common/half
                           ../../CMP_Core/shaders
                           ../../CMP_Core/source
                           /usr/include/OpenEXR)

if(APPLE)
target_include_directories(CompressonatorCLI-bin
                           PRIVATE
                           /usr/local/include/OpenEXR/)
endif()

# Qt5 include path - users install required
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5Gui)
if(Qt5Gui_FOUND)
target_include_directories(CompressonatorCLI-bin
                           PRIVATE 
                           "${Qt5Gui_INCLUDE_DIRS}")
else()
message(FATAL_ERROR "Package Qt5 (Qt5Gui) are required, but not found. In Unix, run initsetup_unix.sh or sudo apt-get install qtdeclarative5-dev. If is window, please make sure install qt and add the bin path (i.e. C:\Qt\5.7\msvc2015_64\bin) to environment PATH.")
endif()

if (UNIX)
target_compile_definitions(CompressonatorCLI-bin PRIVATE _LINUX)
endif()

set (CMP_LIBS "")
if (UNIX)
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../CMP_CompressonatorLib/libCompressonator.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../CMP_Framework/libCMP_Framework.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../Applications/_Libs/CMP_MeshCompressor/libCMP_MeshCompressor.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../Applications/_Plugins/CImage/ASTC/libASTC.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../Applications/_Plugins/CImage/EXR/libEXR.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../Applications/_Plugins/CImage/KTX/libKTX.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../Applications/_Plugins/CImage/TGA/libTGA.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../Applications/_Plugins/CAnalysis/Analysis/libAnalysis.a")
    if(NOT APPLE)
        list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libpthread.so")
        list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libHalf.so")
        list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libImath.so")
        list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libIlmThread.so")
        list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libIex.so")
        list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libIlmImf.so")
    else()
        list(APPEND CMP_LIBS "/usr/lib/libz.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libHalf.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libImath.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libIlmThread.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libIex.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libIlmImf.dylib")
    endif()
endif()

find_package(Boost COMPONENTS system filesystem) 
if(Boost_FOUND)
    list(APPEND CMP_LIBS ${Boost_LIBRARIES})
endif()
find_package(OpenCV) 
if (OpenCV_FOUND)
    list(APPEND CMP_LIBS ${OpenCV_LIBRARIES})
else()
    message(FATAL_ERROR "Package OpenCV are required, but not found. In Unix, run initsetup_unix.sh or sudo apt-get install libopencv-dev to install the libs.")
endif()

target_link_libraries(CompressonatorCLI-bin
                      ${CMP_LIBS}
                      Qt5::Gui
                      )