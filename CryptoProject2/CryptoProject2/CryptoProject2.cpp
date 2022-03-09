
// CryptoProject2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

// Number of plaintexts.
const size_t PLAINTEXT_COUNT = 600;
// Number of time samples in the loaded T matrix.
const size_t SAMPLE_COUNT = 55;
// Number of keys tested.
const size_t KEY_COUNT = 256;
// The AES S-box.
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

template<typename T>
// A Matrix class, which is basically a generic unique pointer to a row count*column count length array.
// Indexed by rows zero...row count - 1 and columns zero...column count - 1
class Matrix {
private:
    std::unique_ptr<T[]> contents;
    size_t rows;
    size_t columns;
public:
    // Construct/allocate a Matrix with given number of rows & columns.
    Matrix(size_t rows, size_t columns) {
        this->rows = rows;
        this->columns = columns;
        this->contents = std::unique_ptr<T[]>(new T[rows * columns]);
    }

    // Returns the matrix value in the given row, column position.
    T get_value(size_t row, size_t column) {
        // Make sure the access is within bounds & the matrix is not invalidated.
        assert(contents != nullptr);
        assert(row >= 0);
        assert(row < rows);
        assert(column >= 0);
        assert(column < columns);
        return contents[row * columns + column];
    }

    // Sets the matrix value in the given row, column position.
    void set_value(size_t row, size_t column, T value) {
        // Make sure the access is within bounds & the matrix is not invalidated.
        assert(contents != nullptr);
        assert(row >= 0);
        assert(row < rows);
        assert(column >= 0);
        assert(column < columns);
        contents[row * columns + column] = value;
    }

    // Get the number of rows in the matrix.
    size_t get_rows() {
        // Just make sure contents exist and we're not looking at an invalidated matrix
        assert(contents != nullptr);
        return this->rows;
    }
    // Get the number of columns in the matrix.
    size_t get_columns() {
        // Just make sure contents exist and we're not looking at an invalidated matrix
        assert(contents != nullptr);
        return this->columns;
    }

    // Returns a deep copy of this Matrix.
    Matrix deep_copy() {
        Matrix result(this->rows, this->columns);

        for (int i = 0;i < rows * columns;i++) {
            result.contents[i] = this->contents[i];
        }

        return result;
    }
};

// Returns the hamming weight of a byte.
unsigned char hamming_weight(unsigned char byte) {
    unsigned char result = 0;

    while (byte) {
        result++;
        byte &= byte - 1;
    }
    return result;
}

// Load the T matrix from the file corresponding to our group.
Matrix<double> load_T(const char* filename) {
    std::ifstream tfile;
    std::cout << "Loading " << filename <<  std::endl;

    tfile.open(filename);
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
    if (lines != PLAINTEXT_COUNT) {
        std::cout << "Failed loading T9.dat!" << std::endl;
    }

    tfile.close();

    return result;
}

// Load plaintexts from the file corresponding to our group. This comes out as a row vector (a matrix with 1 row).
Matrix<unsigned char> load_plaintexts(const char* filename) {
    std::ifstream tfile;
    std::cout << "Loading " << filename <<  std::endl;

    tfile.open(filename);
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

    tfile.close();

    return plaintexts;
}

// Construct the H matrix using the plaintexts for our group.
// The H matrix is indexed by [plaintext, key]
Matrix<unsigned char> construct_Hmatrix(const char* plaintext_file) {
    auto plaintexts = load_plaintexts(plaintext_file);

    Matrix<unsigned char> result(PLAINTEXT_COUNT, KEY_COUNT);

    for (unsigned int key = 0; key < KEY_COUNT; key++) {
        for (size_t plaintext_index = 0; plaintext_index < PLAINTEXT_COUNT; plaintext_index++) {
            result.set_value(plaintext_index, key, hamming_weight(S[key ^ plaintexts.get_value(0, plaintext_index)]));
        }
    }
    return result;
}

// Calculate the pearson correlation between one column of the H matrix and one column of the T matrix.
// This is the correlation between the expected power output for the given key and the observed power output at time t across all plaintexts.
double pearson_correlation(Matrix<unsigned char> & H, Matrix<double> & T, size_t h_key_index, size_t t_sample_index) {
    double hmean = 0;
    double tmean = 0;
    for (int i = 0; i < PLAINTEXT_COUNT; i++) {
        hmean += ((double)H.get_value(i, h_key_index))/PLAINTEXT_COUNT;
        tmean += T.get_value(i, t_sample_index)/PLAINTEXT_COUNT;
    }
    double lower_h_sum = 0;
    double lower_t_sum = 0;
    double upper_sum = 0;
    for (int i = 0; i < PLAINTEXT_COUNT; i++) {
        double hij = H.get_value(i, h_key_index);
        double til = T.get_value(i, t_sample_index);

        upper_sum += (hij - hmean) * (til - tmean);
        lower_h_sum += (hij - hmean) * (hij - hmean);
        lower_t_sum += (til - tmean) * (til - tmean);
    }
    return upper_sum / sqrt(lower_h_sum * lower_t_sum);
}

// Creates the pearson correlation matrix from the H matrix and the T matrix.
// Rows are keys, columns are time indices.
Matrix<double> construct_pearson_correlation_matrix(Matrix<unsigned char> & H, Matrix<double> & T) {
    Matrix<double> result(KEY_COUNT, SAMPLE_COUNT);

    for (int key = 0; key < KEY_COUNT; key++) {
        for (int sample = 0; sample < SAMPLE_COUNT; sample++) {
            result.set_value(key, sample, pearson_correlation(H, T, key, sample));
        }
    }
    return result;
}

// Outputs a matrix to a .csv file.
void output_matrix(Matrix<double>& matrix, const char* filename) {

    std::ofstream file;
    file.open(filename, std::ios::trunc);

    for (size_t i = 0; i < matrix.get_rows(); i++) {
        for (size_t j = 0; j < matrix.get_columns(); j++) {
            file << matrix.get_value(i, j);
            if (j + 1 < matrix.get_columns()) {
                file << ",";
            }
            else if(i + 1 < matrix.get_rows()) {
                file << "\n";
            }
        }
    }

    file.close();
}

// Adds white noise to a matrix with given amplitude. (amplitude = max effect of the noise here)
// So every value in matrix has a number in the range [-amplitude, amplitude] added with uniform distribution.
void add_noise(Matrix<double>& matrix, double amplitude) {
    for (int i = 0; i < matrix.get_rows(); i++) {
        for (int j = 0; j < matrix.get_columns(); j++) {
            matrix.set_value(i, j, matrix.get_value(i, j) + (rand() / ((double)RAND_MAX) * 2 - 1) * amplitude);
        }
    }
}



void generate_noise_series(int min_noise, int max_noise, int step, Matrix<double> &T, Matrix<unsigned char> &H) {
    for (int noise_amplitude = min_noise; noise_amplitude < max_noise; noise_amplitude += step) {
        std::cout << "Generating noise with amplitude " << noise_amplitude << std::endl;
        auto noise_T = T.deep_copy();
        add_noise(noise_T, noise_amplitude);
        std::ostringstream ss;
        ss << "../output/correlation_matrix_noise" << noise_amplitude << ".csv";
        auto pearson_corr = construct_pearson_correlation_matrix(H, noise_T);
        output_matrix(pearson_corr, ss.str().c_str());
    }
}

int main()
{
    const char* T_file = "../data/T9.dat";
    const char* input_file = "../data/inputs9.dat";
    const char* output_file = "../output/correlation_matrix.csv";

    // Load matrices
    std::cout << "Loading matrices" << std::endl;
    auto T = load_T(T_file);
    auto H = construct_Hmatrix(input_file);

    // Construct pearson correlation matrix
    //std::cout << "Constructing pearson correlation matrix" << std::endl;
    //auto pearson_correlation_matrix = construct_pearson_correlation_matrix(H, T);

    // Output to a .csv file for further analysis in python
    //std::cout << "Outputting correlation matrix to .csv file" << std::endl;
    //output_matrix(pearson_correlation_matrix, output_file);

    std::cout << "Generating noise series" << std::endl;

    generate_noise_series(10, 300, 5, T, H);

    std::cout << "Done!" << std::endl;
}
