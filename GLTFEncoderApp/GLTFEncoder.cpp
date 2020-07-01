// GLTF_Draco.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <Compressonator.h>
#include <Common.h>
#include <chrono>
#include <tiny_gltf2.h>




int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Please provide Input and file" << std::endl;
        return 1;
    }
    
    std::string         src_file = argv[1];       //input source  
    std::string         dst_file = argv[2];  //output destination glTF file
    std::string         err;                            //error messages
    tinygltf2::Model    model;
    tinygltf2::TinyGLTF loader;
    tinygltf2::TinyGLTF saver;

    //std::cout << "Input File->" << src_file << endl;
    //std::cout << "Output File->" << dst_file << endl;


    bool perform_mesh_compression = true;  //flag to turn on/off compression
    bool is_src_file_draco = false;  //flag to indicate source file is compressed or not-
                                           //can be replaced with helper function provided below to check for glTF compressed file

    auto start = std::chrono::high_resolution_clock::now();
    bool ret = loader.LoadASCIIFromFile(&model, &err, src_file, perform_mesh_compression);
    auto end1 = std::chrono::high_resolution_clock::now();
	double time_taken =
		chrono::duration_cast<chrono::milliseconds>(end1 - start).count();
    
    std::cout << "Read + Draco Mesh Generation time: " << time_taken << " ms" << std::endl << std::flush;

    if (ret)
    {
        std::cout << "read success" << std::endl << std::flush;
    }
    else
    {
        std::cout << "read fail: " << err << std::endl << std::flush;
        return 1;
    }

    err.clear();

    int level = -1;
    if (argc > 3)
        level = std::stoi(argv[3]);

    CMP_CompressOptions CompressOptions;
    // it is recommended to use only default settings, other settings may result in corrupt in resource like texture.
    CompressOptions.iCmpLevel = CMP_MESH_COMP_LEVEL;    //setting: compression level (range 0-10: higher mean more compressed) - default 7
    CompressOptions.iPosBits = CMP_MESH_POS_BITS;      //setting: quantization bits for position - default 14
    CompressOptions.iTexCBits = CMP_MESH_TEXC_BITS;     //setting: quantization bits for texture coordinates - default 12
    CompressOptions.iNormalBits = CMP_MESH_NORMAL_BITS;   //setting: quantization bits for normal - default 10
    CompressOptions.iGenericBits = CMP_MESH_GENERIC_BITS;  //setting: quantization bits for generic - default 8
    if (level != -1)
        CompressOptions.iCmpLevel = level;
    std::cout << "Level->" << CompressOptions.iCmpLevel << std::endl << std::flush;
    ret = saver.WriteGltfSceneToFile(&model, &err, dst_file, CompressOptions, is_src_file_draco, perform_mesh_compression);
	auto end2 = std::chrono::high_resolution_clock::now();
	time_taken =
		chrono::duration_cast<chrono::milliseconds>(end2 - end1).count();

    std::cout << "Encode + write time: " << time_taken << " ms" << std::endl << std::flush;

    if (ret)
    {
        std::cout << "write success" << std::endl << std::flush;
    }
    else
    {
        std::cout << "write fail: " << err << std::endl << std::flush;
        return 1;
    }
    return 0;
}
