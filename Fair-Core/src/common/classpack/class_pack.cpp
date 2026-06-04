#include "class_pack.h"

uint32_t readU32(const unsigned char*& p, const unsigned char* end) {
    if (p + 4 > end) {
        throw std::runtime_error("Invalid pack: unexpected EOF");
    }

    uint32_t v =
        ((uint32_t)p[0]) |
        ((uint32_t)p[1] << 8) |
        ((uint32_t)p[2] << 16) |
        ((uint32_t)p[3] << 24);

    p += 4;
    return v;
}

ClassPack loadClassPack(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open classes.pack");
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size <= 0) {
        throw std::runtime_error("classes.pack is empty");
    }

    std::vector<unsigned char> data((size_t)size);
    if (!file.read((char*)data.data(), size)) {
        throw std::runtime_error("Failed to read classes.pack");
    }

    const unsigned char* p = data.data();
    const unsigned char* end = data.data() + data.size();

    if (p + 4 > end || p[0] != 'C' || p[1] != 'L' || p[2] != 'P' || p[3] != 'K') {
        throw std::runtime_error("Invalid classes.pack magic");
    }

    p += 4;

    uint32_t version = readU32(p, end);
    if (version != 1) {
        throw std::runtime_error("Unsupported classes.pack version");
    }

    uint32_t classCount = readU32(p, end);

    std::vector<uint32_t> classSizes(classCount);
    uint64_t totalSize = 0;

    for (uint32_t i = 0; i < classCount; i++) {
        classSizes[i] = readU32(p, end);
        totalSize += classSizes[i];
    }

    if ((uint64_t)(end - p) < totalSize) {
        throw std::runtime_error("Invalid classes.pack: class bytes not enough");
    }

    ClassPack pack;
    pack.classes.reserve(classCount);

    for (uint32_t i = 0; i < classCount; i++) {
        uint32_t len = classSizes[i];

        std::vector<unsigned char> bytes(p, p + len);
        p += len;

        pack.classes.push_back(std::move(bytes));
    }

    return pack;
}