
// CryptoProject2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

const size_t PLAINTEXT_COUNT = 600;
const size_t SAMPLE_COUNT = 55;
const size_t KEY_COUNT = 256;
const unsigned char S[] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

/// <summary>
/// Contains a Matrix smart pointer, generic over the type T.
/// Only allows a single owner of the contents.
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
class Matrix {
private:
    T* contents;
    size_t rows;
    size_t columns;
public:
    Matrix(size_t rows, size_t columns) {
        this->rows = rows;
        this->columns = columns;
        this->contents = new T[rows * columns];
    }

    ~Matrix() {
        if (contents != nullptr) {
            delete[] contents;
        }
    }

    Matrix(Matrix&& o) noexcept : rows(o.rows), columns(o.columns), contents(o.contents) {
        o.contents = nullptr;
        o.rows = 0;
        o.columns = 0;
    }
    //TODO: CONST MOVE CONSTRUCTOR

    T get_value(size_t row, size_t column) {
        assert(contents != nullptr);
        assert(row >= 0);
        assert(row < rows);
        assert(column >= 0);
        assert(column < columns);
        return contents[row * columns + column];
    }

    void set_value(size_t row, size_t column, T value) {
        assert(contents != nullptr);
        assert(row >= 0);
        assert(row < rows);
        assert(column >= 0);
        assert(column < columns);
        contents[row * columns + column] = value;
    }

};

unsigned char hamming_weight(unsigned char byte) {
    unsigned char result = 0;

    while (byte) {
        result++;
        byte &= byte - 1;
    }
    return result;
}

Matrix<double> load_T() {
    std::ifstream tfile;
    std::cout << "Loading T9.dat" << std::endl;

    tfile.open("../data/T9.dat");
    std::string line;
    Matrix<double> result(PLAINTEXT_COUNT, SAMPLE_COUNT);
    int lines = 0;
    int index;
    while (std::getline(tfile, line)) {
        index = 0;
        size_t split = 0;
        std::string number;
        while ((split = line.find(',')) != std::string::npos) {
            number = line.substr(0, split);
            char* end;
            result.set_value(lines, index++, strtod(number.data(), &end));
            line.erase(0, split + 1);
        }

        lines++;

    }
    std::cout << "Lines:" << lines << std::endl;

    tfile.close();

    return result;
}

Matrix<unsigned char> load_plaintexts() {
    std::ifstream tfile;
    std::cout << "Loading inputs9.dat" << std::endl;

    tfile.open("../data/inputs9.dat");
    Matrix<unsigned char> plaintexts(1, PLAINTEXT_COUNT);
    std::string line;
    int index = 0;
    if (std::getline(tfile, line)) {
        size_t split = 0;
        std::string number;
        while ((split = line.find(',')) != std::string::npos) {
            number = line.substr(0, split);
            char* end;
            plaintexts.set_value(0, index++, strtol(number.data(), &end, 10));
            line.erase(0, split + 1);
        }
    }
    else {
        std::cout << "Failed loading input9.dat!" << std::endl;
    }
    std::cout << "indices:" << index << std::endl;

    tfile.close();

    return plaintexts;
}

// Hmatrix is indexed by [plaintext, key]
Matrix<unsigned char> construct_Hmatrix() {
    auto plaintexts = load_plaintexts();

    std::cout << "Constructing Hmatrix" << std::endl;
    Matrix<unsigned char> Hmatrix(PLAINTEXT_COUNT, KEY_COUNT);

    for (unsigned int key = 0; key < KEY_COUNT; key++) {
        std::cout << "Key " << key << std::endl;
        for (size_t plaintext_index = 0; plaintext_index < PLAINTEXT_COUNT; plaintext_index++) {
            Hmatrix.set_value(plaintext_index, key, hamming_weight(S[key ^ plaintexts.get_value(0, plaintext_index)]));
        }
    }
    return Hmatrix;
}

double pearson_correlation(Matrix<unsigned char> Hmatrix, Matrix<double> Tmatrix, size_t h_key_index, size_t t_sample_index) {
    for (int i = 0; i < PLAINTEXT_COUNT; i++) {
        unsigned char hij = Hmatrix.get_value(i, h_key_index);
        double tij = Tmatrix.get_value(i, t_sample_index);
    }
    return 0; //TODO
}

int main()
{
    auto T = load_T();
    Matrix<unsigned char> Hmatrix = construct_Hmatrix();
    std::cout << "Hello World!\n" << hamming_weight(50) << std::endl;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
