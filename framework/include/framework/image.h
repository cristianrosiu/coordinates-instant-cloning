#pragma once
// Suppress warnings in third-party code.
#include <framework/disable_all_warnings.h>

#include <filesystem>
#include <vector>
#include <cassert>
#include <exception>
#include <iostream>
#include <string>

DISABLE_WARNINGS_PUSH()
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
DISABLE_WARNINGS_POP()


template <typename T>
class Image {
public:
    Image(const std::filesystem::path& filePath);
    Image(const int new_width, const int new_height);
    Image(const Image&) = default;

    void writeToFile(const std::filesystem::path& filePath, const float scaling_factor = 1.0f);

public:
    int width, height;
    std::vector<T> data;
};

template<typename T> 
inline T stbToType(const stbi_uc* src) { throw std::exception(); };
template <>
float stbToType<float>(const stbi_uc* src);
template <>
glm::vec3 stbToType<glm::vec3>(const stbi_uc* src);

template<typename T>
inline T stbfToType(const float* src) { throw std::exception(); };
template <>
float stbfToType<float>(const float* src);
template <>
glm::vec3 stbfToType<glm::vec3>(const float* src);

template <typename T>
inline void typeToRgbUint8(stbi_uc* dst, const T& value) { throw std::exception(); };
template <>
void typeToRgbUint8(stbi_uc* dst, const float& value);
template <>
void typeToRgbUint8(stbi_uc* dst, const glm::vec3& value);

template <typename T>
Image<T>::Image(const std::filesystem::path& filePath)
{
    if (!std::filesystem::exists(filePath)) {
        std::cerr << "Image file " << filePath << " does not exists!" << std::endl;
        throw std::exception();
    }

    const auto filePathStr = filePath.string(); // Create l-value so c_str() is safe.
    int channels;

    if (stbi_is_hdr(filePathStr.c_str())) {
        stbi_hdr_to_ldr_gamma(1.0f);
        stbi_hdr_to_ldr_scale(1.0f);
        float* stb_data_float = stbi_loadf(filePathStr.c_str(), &width, &height, &channels, 0);

        data.resize(width * height);
        for (size_t i = 0; i < data.size(); i++) {
            data[i] = stbfToType<T>(stb_data_float + i * channels);
        }

        stbi_image_free(stb_data_float);
    }
    else {
        stbi_uc* stb_data = stbi_load(filePathStr.c_str(), &width, &height, &channels, 0);
        if (!stb_data) {
            std::cerr << "Failed to read image " << filePath << " using stb_image.h" << std::endl;
            throw std::exception();
        }

        data.resize(width * height);
        for (size_t i = 0; i < data.size(); i++) {
            data[i] = stbToType<T>(stb_data + i * channels);
        }

        stbi_image_free(stb_data);
    }


}

template <typename T>
Image<T>::Image(const int new_width, const int new_height)
{
    width = new_width;
    height = new_height;
    data.resize(width * height); // Should be full of zeros.
}

template <typename T>
inline void Image<T>::writeToFile(const std::filesystem::path& filePath, const float scaling_factor) {

    // RGB => 3
    const auto channels = 3;

    // Converts floats to uint8 array. 
    // Assumes normalized format, so it is multiplied by 255 (on top of the scaling_factor).
    // If input is single channel, it triples it to get RGB.
    std::vector<stbi_uc> std_data;
    std_data.resize(width * height * channels);
    for (size_t i = 0; i < data.size(); i++) {
        typeToRgbUint8<T>(&std_data[i * channels], data[i] * scaling_factor);
    }

    // Create a folder.
    if (!std::filesystem::is_directory(filePath.parent_path())) {
        std::filesystem::create_directories(filePath.parent_path());
    }

    // Decide JPG (default) or PNG based on extension.
    const auto filePathStr = filePath.string(); // Create l-value so c_str() is safe.
    if (filePath.extension() == ".png") {
        stbi_write_png(filePathStr.c_str(), width, height, channels, std_data.data(), width * channels);
    } else {
        stbi_write_jpg(filePathStr.c_str(), width, height, channels, std_data.data(), 95);
    }
};
