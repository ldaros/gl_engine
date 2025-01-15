#include "image_loader.h"

#include "stb_image.h"
#include <iostream>

ImageLoader::ImageLoader() {}
ImageLoader::~ImageLoader() {}

bool ImageLoader::load(const std::string& path, ImageData& image) {
    // Clear any existing data
    image.pixels.clear();
    image.width = 0;
    image.height = 0;
    image.channels = 0;

    // Load the image using stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);  // Flip images for OpenGL
    
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load image: " << path << "\n";
        std::cerr << "STB Error: " << stbi_failure_reason() << "\n";
        return false;
    }

    // Set the image properties
    image.width = width;
    image.height = height;
    image.channels = channels;

    // Copy the pixel data
    size_t dataSize = width * height * channels;
    image.pixels.resize(dataSize);
    std::memcpy(image.pixels.data(), data, dataSize);

    // Free the stb_image data
    stbi_image_free(data);

    return true;
}
