#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <string>
#include <vector>

class ImageLoader {
public:
    struct ImageData {
        std::vector<unsigned char> pixels;
        int width;
        int height;
        int channels;
        
        bool hasAlpha() const { return channels == 4; }
        size_t getSizeInBytes() const { return pixels.size(); }
        bool isEmpty() const { return pixels.empty(); }
    };

    ImageLoader();
    ~ImageLoader();
    
    static bool load(const std::string& path, ImageData& image);
    
private:
    ImageLoader(const ImageLoader&) = delete;
    ImageLoader& operator=(const ImageLoader&) = delete;
};

#endif // IMAGE_LOADER_H