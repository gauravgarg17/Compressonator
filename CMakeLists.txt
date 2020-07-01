cmake_minimum_required(VERSION 3.10)

project(DracoGLTF)

add_subdirectory(Compressonator/Applications/_Libs/CMP_MeshCompressor)
add_subdirectory(GLTFEncoderApp)

add_dependencies(GLTFEncoderApp CMP_MeshCompressor)