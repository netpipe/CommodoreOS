#include <FreeImage.h>
#include <iostream>
#include <vector>
#include <fstream>

std::vector<uint8_t> convertImageToC64Sprite(const char* imagePath) {
    // Initialize FreeImage
    FreeImage_Initialise();

    // Load the image
    FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, imagePath, 0);
    if (!bitmap) {
        std::cerr << "Image loading failed." << std::endl;
        exit(1);
    }

    // Convert to greyscale
    FIBITMAP* greyBitmap = FreeImage_ConvertToGreyscale(bitmap);
    FreeImage_Unload(bitmap);

    // Resize image to 24x21 (C64 sprite size)
    FIBITMAP* resizedBitmap = FreeImage_Rescale(greyBitmap, 24, 21, FILTER_BOX);
    FreeImage_Unload(greyBitmap);

    std::vector<uint8_t> spriteData;

    // Convert each row of pixels to bytes
    for (unsigned y = 0; y < 21; ++y) {
        for (unsigned x = 0; x < 24; x += 8) {
            uint8_t byte = 0;
            for (unsigned bit = 0; bit < 8; ++bit) {
                if (x + bit < 24) {
                    RGBQUAD color;
                    FreeImage_GetPixelColor(resizedBitmap, x + bit, y, &color);
                    if (color.rgbReserved) {  // Assuming non-zero alpha means a visible pixel
                        byte |= (1 << (7 - bit));
                    }
                }
            }
            spriteData.push_back(byte);
        }
    }

    FreeImage_Unload(resizedBitmap);
    FreeImage_DeInitialise();

    return spriteData;
}

void writeSpriteToFile(const std::vector<uint8_t>& spriteData, const std::string& outputFilePath) {
    std::ofstream outputFile(outputFilePath, std::ios::out | std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening output file" << std::endl;
        return;
    }

    for (uint8_t byte : spriteData) {
        outputFile << std::hex << static_cast<int>(byte) << " ";
    }
}

int main() {
    std::string imagePath = "path/to/your/image.png";
    std::string outputFilePath = "output.txt";

    auto spriteData = convertImageToC64Sprite(imagePath.c_str());
    writeSpriteToFile(spriteData, outputFilePath);

    return 0;
}

