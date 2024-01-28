#include <FreeImage.h>
#include <iostream>
#include <vector>
#include <fstream>

// C64 Color Palette (16 colors)
const RGBQUAD c64_palette[16] = {
    {0, 0, 0, 0}, {255, 255, 255, 0}, {136, 0, 0, 0}, {170, 255, 238, 0},
    {204, 68, 204, 0}, {0, 204, 85, 0}, {0, 0, 170, 0}, {238, 238, 119, 0},
    {221, 136, 85, 0}, {102, 68, 0, 0}, {255, 119, 119, 0}, {51, 51, 51, 0},
    {119, 119, 119, 0}, {170, 255, 102, 0}, {0, 136, 255, 0}, {187, 187, 187, 0}
};

// Function to find closest C64 color
int getClosestC64Color(const RGBQUAD& color) {
    int minDistance = INT_MAX;
    int closestColorIndex = 0;
    for (int i = 0; i < 16; ++i) {
        int distance = (color.rgbRed - c64_palette[i].rgbRed) * (color.rgbRed - c64_palette[i].rgbRed) +
                       (color.rgbGreen - c64_palette[i].rgbGreen) * (color.rgbGreen - c64_palette[i].rgbGreen) +
                       (color.rgbBlue - c64_palette[i].rgbBlue) * (color.rgbBlue - c64_palette[i].rgbBlue);
        if (distance < minDistance) {
            minDistance = distance;
            closestColorIndex = i;
        }
    }
    return closestColorIndex;
}

std::vector<uint8_t> convertImageToC64Sprite(const char* imagePath) {
    // Initialize FreeImage
    FreeImage_Initialise();

    // Load the image
    FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, imagePath, 0);
    if (!bitmap) {
        std::cerr << "Image loading failed." << std::endl;
        exit(1);
    }

    // Resize image to 24x21 (C64 sprite size)
    FIBITMAP* resizedBitmap = FreeImage_Rescale(bitmap, 24, 21, FILTER_BOX);
    FreeImage_Unload(bitmap);

    std::vector<uint8_t> spriteData;

    // Convert each row of pixels to bytes
    for (unsigned y = 0; y < 21; ++y) {
        for (unsigned x = 0; x < 24; x += 8) {
            uint8_t byte = 0;
            for (unsigned bit = 0; bit < 8; ++bit) {
                if (x + bit < 24) {
                    RGBQUAD color;
                    FreeImage_GetPixelColor(resizedBitmap, x + bit, y, &color);
                    int colorIndex = getClosestC64Color(color);
                    byte |= (colorIndex != 0) << (7 - bit); // Assuming index 0 is transparent
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

