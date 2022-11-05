#include <iostream>
#include <lodepng.h>
#include <vector>
#include <cstdint>
#include <string>
#include "Vertice_Direction.h"

void decodeFromPng(std::vector<unsigned char>& image, const char* filename, unsigned height, unsigned width) {
    /*
     * Функция для чтения png файла
     */
    std::vector<unsigned char> png;
    //load and decode
    unsigned error = lodepng::load_file(png, filename);
    if(!error) error = lodepng::decode(image, width, height, png);

    //if there's an error, display it
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
}

void getBlueChannel(const std::vector<unsigned char>& image, int** arr, int rows, int cols) {
    /*
     * Функция, которая выделяет синий канал из одномерного вектора и
     * заполняет им двумерный массив
     */
    for (int i = 2, j = 0, k = 0; i < image.size(); i += 4, ++k) {
        if (k == cols) {
            k = 0; // обнуляем номер столбца
            ++j; // увеличиваем номер строки
        }
        arr[j][k] = int(image[i]);
    }
}
void printArray(int** arr, int rows, int cols) {
    /*
     * Функция для вывода массива в консоль
     */
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j)
            std::cout << arr[i][j] << " ";
        std::cout <<std:: endl;
    }
}

void drawPath(const std::vector<int>& path, std::vector<unsigned char>& image, int rows, int cols, const char* output_file) {
    int i = 0, j = -1;
    int previousPoint = -1;
    for (auto& e : path) {
        if (previousPoint == e)
            ++i;
        else
            j = e;
        previousPoint = e;
        // draw red path R = 255, G = 0, B = 0
        int index = (i * cols + j) * 4;
        image[index] = (unsigned char)255;
        image[++index] = (unsigned char)0;
        image[++index] = (unsigned char)0;
    }

    //Encode the image
    unsigned error = lodepng::encode(output_file, image, cols, rows);

    //if there's an error, display it
    if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

void findOptimalPath(int** arr, std::vector<int>& path, int x_i, int x_f, int rows, int cols) {
    //--------------------------------------------------------------------------------
    // Напишите ваш код здесь

    Vertice** matrix_array = new Vertice * [rows];
    for (int i = 0; i < rows; ++i)
        matrix_array[i] = new Vertice[cols];

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            matrix_array[i][j].visited = false;
            matrix_array[i][j].path_weight = std::numeric_limits<int>::max();
            matrix_array[i][j].own_weight = arr[i][j];
            matrix_array[i][j].previous = position::no;
            }
    }
    matrix_array[0][x_i].path_weight = 0;

    bool no_more = true;
    while (no_more) {
        // find position to check
        int min_weight = std::numeric_limits<int>::max();
        int min_i = -1;
        int min_j = -1;
        for (int p = 0; p < rows; ++p) {
            for (int q = 0; q < cols; ++q) {
                if (!(matrix_array[p][q].visited) && (matrix_array[p][q].path_weight < min_weight)) {
                    min_weight = matrix_array[p][q].path_weight;
                    min_i = p;
                    min_j = q;
                    // std::cout << min_i << " " << min_j << " " << arr[min_i][min_j].own_weight << std::endl;
                }
            }
        }

        // determine the paths_weight to the connected points
        if (min_i != -1) {

            // down (с проверкой, что не выходит за границы, не посещен и меньше предыдущего значения)
            if ((min_i + 1 < rows) && !matrix_array[min_i + 1][min_j].visited) {
                int new_down_weight = matrix_array[min_i][min_j].path_weight + matrix_array[min_i][min_j].own_weight;
                if (new_down_weight < matrix_array[min_i + 1][min_j].path_weight) {
                    matrix_array[min_i + 1][min_j].path_weight = new_down_weight;
                    matrix_array[min_i + 1][min_j].previous = position::down;
                }
            }

            //right
            if ((min_j + 1 < cols) && !matrix_array[min_i][min_j + 1].visited) {
                int new_right_weight = matrix_array[min_i][min_j].path_weight + matrix_array[min_i][min_j].own_weight;
                if (new_right_weight < matrix_array[min_i][min_j + 1].path_weight) {
                    matrix_array[min_i][min_j + 1].path_weight = new_right_weight;
                    matrix_array[min_i][min_j + 1].previous = position::r;
                }
            }
            //left
            if ((min_j - 1 < cols) && !matrix_array[min_i][min_j - 1].visited) {
                int new_left_weight = matrix_array[min_i][min_j].path_weight + matrix_array[min_i][min_j].own_weight;
                if (new_left_weight < matrix_array[min_i][min_j - 1].path_weight) {
                    matrix_array[min_i][min_j - 1].path_weight = new_left_weight;
                    matrix_array[min_i][min_j - 1].previous = position::l;
                }
            }
            matrix_array[min_i][min_j].visited = true;
        } else {
            no_more = false;
        }
    }

    int i = cols - 1;
    int j = x_f;
    std::vector<int> path_X;
    std::vector<int> path_Y;
    while (!(i == 0 and j == x_i)){
        //path_X.push_back(i);
        path.push_back(j);
        switch (matrix_array[i][j].previous) {
            case down:
                i -= 1;
                break;
            case r:
                j -= 1;
                break;
            case l:
                j += 1;
                break;
            case no:
                break;
        }
    }
    //std::cout << "Finish2" << std::endl;
    path.push_back(j);

    std::reverse(path.begin(), path.end());

    for (int i = 0; i < rows; ++i)
        delete[] matrix_array[i];
    delete matrix_array;

    //--------------------------------------------------------------------------------
}

int main(int argc, char* argv[]) {

    if (argc != 7) {
        std::cerr << "Wrong input arguments" << std::endl;
        return -1;
    }
    int rows = std::stoi(argv[2]), cols = std::stoi(argv[3]); // определение строк и столбцов как у матрицы в математике
    int x_i = std::stoi(argv[4]), x_f = std::stoi(argv[5]);
    char *input_file = argv[1], *output_file = argv[6];
    // читаем png изображение
    std::vector<unsigned char> image; // vector to store value of pixels with all 4 channels RGBA
    decodeFromPng(image, input_file, rows, cols); // read png file

    // способ 3 стр. 115 создания двумерного массива
    // выделяем память

    int** arr = new int*[rows];
    for (int row = 0; row < rows; ++row )
        arr[row] = new int[cols];

    // инициализируем выделенный массив arr синим каналом нашего изображения
    getBlueChannel(image, arr, rows, cols);

    // можно опционально выводить массив в консоль для отладки
    //printArray(arr, rows, cols);

    //функцию ниже вам нужно реализовать
    //-----------------------------------------------------------------------------
    std::vector<int> path;  // вектор к которому нужно делать push_back для добавления точек пути
    findOptimalPath(arr, path, x_i, x_f, rows, cols);
    //------------------------------------------------------------------------------

    // далее можно отрисовать path используя фукцию ниже
    drawPath(path, image, rows, cols, output_file);


    // очищаем память
    for (int row = 0; row < rows; ++row )
        delete[] arr[row];
    delete[] arr;
    return 0;
}

