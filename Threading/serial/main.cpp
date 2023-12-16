#include <fstream>
#include <iostream>

#include <vector>
#include <chrono>

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

struct Pixel{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

using Image = std::vector<std::vector<Pixel>>;
using Kernel = std::vector<std::vector<int>>;


#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)

int rows;
int cols;

bool fill_and_allocate(char*& buffer, const char* file_name, int& rows, int& cols, int& buffer_size) {
    std::ifstream file(file_name);
    if (!file) {
        std::cout << "File" << file_name << " doesn't exist!" << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    buffer_size = file_header->bfSize;
    return true;
}

void get_pixels_from_bmp24(int end, int rows, int cols, char* file_read_buffer, Image&image) {
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        std::vector<Pixel> row;
        for (int j = cols - 1; j >= 0; j--) {
            Pixel pixel;
            for (int k = 0; k < 3; k++){
            switch (k){
                case 0:
                    pixel.red = static_cast<unsigned char>(file_read_buffer[end - count]);
                    break;
                case 1:
                    pixel.green = static_cast<unsigned char>(file_read_buffer[end - count]);
                    break;
                case 2:
                    pixel.blue =  static_cast<unsigned char>(file_read_buffer[end - count]);
                    break;
                }
                count++;
            }
            row.push_back(pixel);
        }
        image.push_back(row);
    }
}

void write_out_bmp24(char* file_buffer, const char* name_of_file, int buffer_size, Image& image) {
    std::ofstream write(name_of_file);
    if (!write) {
        std::cout << "Failed to write " << name_of_file << std::endl;
        return;
    }

    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++){
                count++; // check 
                switch (k)
                {
                case 0:
                    file_buffer[buffer_size - count] = static_cast<char>(image[i][j].green);
                    break;
                case 1:
                    file_buffer[buffer_size - count] = static_cast<char>(image[i][j].blue);
                    break;
                case 2:
                    file_buffer[buffer_size - count] = static_cast<char>(image[i][j].red);
                    break;
                }
            } 
        }
    }
    write.write(file_buffer, buffer_size);
}

void v_reverse(Image& image) {
    int num_rows = image.size();
    int num_cols = image[0].size();

    for (int i = 0; i < num_rows / 2; i++) {
        for (int j = 0; j < num_cols; j++) {
            Pixel temp_pixel = image[i][j];
            image[i][j] = image[num_rows - i - 1][j];
            image[num_rows - i - 1][j] = temp_pixel;
        }
    }
}

void perform_kernel_on_pixel(int row, int col, Image& image, const Image& temp_image, Kernel& kernel){
    int kernel_rows = kernel.size();
    int kernel_cols = kernel[0].size();

    int red = 0, green = 0, blue = 0;
    for (int i = 0; i < kernel_rows; i++){
        for (int j = 0; j < kernel_cols; j++){
            int image_row = row - kernel_rows / 2 + i;
            int image_col = col - kernel_cols / 2 + j;
            if (image_row < 0 || image_row >= image.size())
                continue;
            if (image_col < 0 || image_col >= image[0].size())
                continue;
                
            red += temp_image[image_row][image_col].red * kernel[i][j] / 16;
            green += temp_image[image_row][image_col].green * kernel[i][j] / 16;
            blue += temp_image[image_row][image_col].blue * kernel[i][j] / 16;
            

        }
    }
    green = std::min(255, green);
    green = std::max(0, green);
    blue = std::min(255, blue);
    blue = std::max(0, blue);
    red = std::min(255, red);
    red = std::max(0, red);


    image[row][col].red = (unsigned)red;
    image[row][col].green = (unsigned)green;
    image[row][col].blue = (unsigned)blue;



}

void filter_kernel(Image& image, Kernel& kernel){
    int num_rows = image.size();
    int num_cols = image[0].size();
    Image temp_image = image;

    for (int i = 0; i < num_rows; i++){
        for (int j = 0; j < num_cols; j++){
            perform_kernel_on_pixel(i, j, image, temp_image, kernel);
        }
    }
}

void purple_haze(Image& image){
    int num_rows = image.size();
    int num_cols = image[0].size();

    for (int i = 0; i < num_rows; i++){
        for (int j = 0; j < num_cols; j++){
            int blue = image[i][j].red * 0.16f + image[i][j].green * 0.5f + image[i][j].blue * 0.16f;
            int red = image[i][j].red * 0.6f + image[i][j].green * 0.2f + image[i][j].blue * 0.8f;
            int green = image[i][j].red * 0.5f + image[i][j].green * 0.3f + image[i][j].blue * 0.5f;
            red = std::max(red, 0);
            green = std::max(green, 0);
            blue = std::max(blue, 0);
            red = std::min(red, 255);
            green = std::min(green, 255);
            blue = std::min(blue, 255);
    
            image[i][j].red = (unsigned)red;
            image[i][j].blue =(unsigned)blue;
            image[i][j].green = (unsigned)green;
        }
    }
}

void add_hatching(Image& image){
    int num_rows = image.size();
    int num_cols = image[0].size();

    float m1 = (float)num_rows / num_cols;

    for (int i = 0; i < num_cols; i++){
        image[(int)(i * m1)][i] = {255, 255, 255};
    }
    for (int i = num_cols / 2; i < num_cols; i++){
        image[(int)(i * m1) - num_rows / 2][i] = {255, 255, 255};
    }
    for (int i = 0; i < num_cols / 2; i++){
        image[(int)(i * m1) + (num_rows / 2)][i] = {255, 255, 255};
    }
}

int main(int argc, char* argv[]) {
    char* file_buffer;
    int buffer_size;
    char* file_name = argv[1];

    if (!fill_and_allocate(file_buffer, file_name, rows, cols, buffer_size)) {
        std::cout << "File read error" << std::endl;
        return 1;
    }
    Image image;
    auto t1 = std::chrono::high_resolution_clock::now();
    get_pixels_from_bmp24(buffer_size - 1, rows, cols, file_buffer, image);
    auto t2 = std::chrono::high_resolution_clock::now();
    v_reverse(image);
    auto t3 = std::chrono::high_resolution_clock::now();
    Kernel blur_filter = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    Kernel test_filter = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
    filter_kernel(image, blur_filter);
    auto t4 = std::chrono::high_resolution_clock::now();
    purple_haze(image);
    auto t5 = std::chrono::high_resolution_clock::now();
    add_hatching(image);
    auto t6 = std::chrono::high_resolution_clock::now();
    write_out_bmp24(file_buffer, argv[2], buffer_size - 1, image);\
    auto t7 = std::chrono::high_resolution_clock::now();

    std::cout << "Time to read: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "ms" << std::endl;
    std::cout << "Time to reverse: " << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count() << "ms" << std::endl;
    std::cout << "Time to blur: " << std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count() << "ms" << std::endl;
    std::cout << "Time to purple haze: " << std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count() <<  "ms" <<std::endl;
    std::cout << "Time to add hatching: " << std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5).count() << "ms" << std::endl;
    std::cout << "Time to write: " << std::chrono::duration_cast<std::chrono::milliseconds>(t7 - t6).count() << "ms" << std::endl;
    std::cout << "Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t7 - t1).count() << "ms" << std::endl;

    return 0;
}
