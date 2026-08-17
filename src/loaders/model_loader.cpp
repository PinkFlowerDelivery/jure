#include "model_loader.h"
#include <stdexcept>

jure::loaders::Texture jure::loaders::ModelLoader::ModelLoader::parseTexture(std::string filepath) {
    int32_t w;
    int32_t h;
    int32_t channels;
    uint8_t* img = stbi_load(filepath.c_str(), &w, &h, &channels, STBI_rgb_alpha);
    if (img == nullptr) {
        throw std::runtime_error("Failed to load image");
    }

    size_t imageSize = w * h * 4;
    std::vector<uint8_t> imgBytes(img, img + imageSize);

    stbi_image_free(img);

    VkExtent3D extent;
    extent.depth = 1;
    extent.width = w;
    extent.height = h;

    return {extent, std::move(imgBytes)};
};
