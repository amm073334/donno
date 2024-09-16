#include <iostream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void error(std::string err) {
    std::cout << err << std::endl;
    exit(1);
}

std::string get_filename(int idx) {
    static const std::string ext = ".png";
    if      (idx < 10)  return "00" + std::to_string(idx) + ext;
    else if (idx < 100) return "0"  + std::to_string(idx) + ext;
    else                return        std::to_string(idx) + ext;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        error("usage: " + (std::string)argv[0] + " <dir>");
    }
    std::string dir = argv[1];
    if (dir.back() != '/' && dir.back() != '\\') dir += '\\'; // insert a trailing backslash to file dir if necessary

    int w = 0, h = 0;
    const int ch = 4;
    std::vector<unsigned char*> imgs;
    for (int frame_idx = 0;; frame_idx++) {
        std::string file = dir + get_filename(frame_idx);
        int x, y, n;

        unsigned char* img = stbi_load(file.c_str(), &x, &y, &n, ch);
        if (!img) break;

        if (frame_idx == 0) { w = x; h = y; } // on the first frame loaded, set the image dimensions
        if (x != w || y != h) error("image dimensions of '" + file + "' aren't consistent");
        
        imgs.push_back(img);
    }

    const int frame_bytes = w * h * ch;
    if (frame_bytes <= 0) error("could not load any frames");
    unsigned char* out = (unsigned char*)malloc(imgs.size() * frame_bytes);
    if (!out) error("output alloc failed");
    
    unsigned char* copy_dst = out;
    for (auto& img : imgs) {
        for (size_t i = 0; i < frame_bytes; i++)
            copy_dst[i] = img[i];
        stbi_image_free(img);
        copy_dst += frame_bytes;
    }

    const char* outfile = "out.png";
    stbi_write_png(outfile, w, h * imgs.size(), ch, out, w*ch);
    free(out);
    return 0;
}