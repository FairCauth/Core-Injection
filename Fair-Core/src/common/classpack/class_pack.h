#pragma once
#include <jni/jni.h>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <stdexcept>

struct ClassPack {
    std::vector<std::vector<unsigned char>> classes;
};
uint32_t readU32(const unsigned char*& p, const unsigned char* end);
ClassPack loadClassPack(const std::filesystem::path& path);