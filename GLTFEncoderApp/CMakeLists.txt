cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

add_executable(GLTFEncoderApp "")

target_sources(GLTFEncoderApp
               PRIVATE
               GLTFEncoder.cpp
               ../Compressonator/Applications/_Plugins/Common/gltf/tiny_gltf2_utils.cpp
               )

target_include_directories(GLTFEncoderApp
                           PRIVATE
                           ../Compressonator/Applications/_Plugins/Common
                           ../Compressonator/Applications/_Plugins/Common/json
                           ../Compressonator/Applications/_Plugins/Common/gltf
                           ../Compressonator/Applications/_Libs/CMP_MeshCompressor/Draco/src
                           ../Compressonator/CMP_CompressonatorLib
                           ../Compressonator/CMP_Framework/Common/half)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)

target_link_libraries(GLTFEncoderApp 
	debug		${PROJECT_BINARY_DIR}/Compressonator/Applications/_Libs/CMP_MeshCompressor/Debug/CMP_MeshCompressor.lib
	optimized	${PROJECT_BINARY_DIR}/Compressonator/Applications/_Libs/CMP_MeshCompressor/Release/CMP_MeshCompressor.lib
 )
