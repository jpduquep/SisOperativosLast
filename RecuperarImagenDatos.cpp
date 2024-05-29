#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <zlib.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;

const int MATRIX_SIZE = 2;
const int MOD = 256;

// Función para calcular el determinante de una matriz 2x2
int determinant(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}

// Función para calcular la matriz inversa modular de una matriz 2x2
void inverseMatrix(int matrix[MATRIX_SIZE][MATRIX_SIZE], int inverse[MATRIX_SIZE][MATRIX_SIZE], int mod) {
    int det = determinant(matrix);
    int det_inv = 0;
    for (int i = 0; i < mod; i++) {
        if ((det * i) % mod == 1) {
            det_inv = i;
            break;
        }
    }
    inverse[0][0] = (matrix[1][1] * det_inv) % mod;
    inverse[0][1] = (-matrix[0][1] * det_inv) % mod;
    inverse[1][0] = (-matrix[1][0] * det_inv) % mod;
    inverse[1][1] = (matrix[0][0] * det_inv) % mod;
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (inverse[i][j] < 0) {
                inverse[i][j] += mod;
            }
        }
    }
}

// Función para multiplicar matrices y aplicar módulo
void multiplyMatrix(int matrix[MATRIX_SIZE][MATRIX_SIZE], int vector[MATRIX_SIZE], int result[MATRIX_SIZE], int mod) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        result[i] = 0;
        for (int j = 0; j < MATRIX_SIZE; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
        result[i] = result[i] % mod;
    }
}

// Función para descifrar el texto utilizando el cifrado de Hill
string hillDecipher(string text, int key[MATRIX_SIZE][MATRIX_SIZE], int mod) {
    int inverseKey[MATRIX_SIZE][MATRIX_SIZE];
    inverseMatrix(key, inverseKey, mod);
    string decryptedText = "";
    for (size_t i = 0; i < text.length(); i += MATRIX_SIZE) {
        int vector[MATRIX_SIZE];
        for (int j = 0; j < MATRIX_SIZE; j++) {
            vector[j] = static_cast<unsigned char>(text[i + j]);
        }
        int result[MATRIX_SIZE];
        multiplyMatrix(inverseKey, vector, result, mod);
        for (int j = 0; j < MATRIX_SIZE; j++) {
            decryptedText += static_cast<char>(result[j]);
        }
    }
    // Eliminar padding
    decryptedText.erase(remove(decryptedText.begin(), decryptedText.end(), '\0'), decryptedText.end());
    return decryptedText;
}

struct Node {
    char ch;
    Node* left;
    Node* right;

    Node(char c) : ch(c), left(nullptr), right(nullptr) {}
};

struct Patient {
    string name;
    int age;
    float height;
    float weight;
    string diagnosisDate;
    string diagnosis;
};

Node* deserializeHuffmanTree(const string& str, int& index) {
    if (index >= str.size()) return nullptr;

    if (str[index] == '1') {
        return new Node(str[++index]);
    }

    Node* node = new Node('\0');
    node->left = deserializeHuffmanTree(str, ++index);
    node->right = deserializeHuffmanTree(str, ++index);
    return node;
}

string decode(Node* root, const string& encodedData) {
    string decodedString;
    Node* curr = root;
    for (char bit : encodedData) {
        if (bit == '0') {
            curr = curr->left;
        } else {
            curr = curr->right;
        }

        if (!curr->left && !curr->right) {
            decodedString += curr->ch;
            curr = root;
        }
    }
    return decodedString;
}

void saveImage(const vector<unsigned char>& imageData, int width, int height, int channels, const string& filename) {
    stbi_write_jpg(filename.c_str(), width, height, channels, imageData.data(), 100);
}

void readFromFile(const string& filename, string& encodedData, string& serializedTree, string& patientData, int& width, int& height, int& channels) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cerr << "Error opening file for reading." << endl;
        return;
    }

    inFile.read(reinterpret_cast<char*>(&width), sizeof(width));
    inFile.read(reinterpret_cast<char*>(&height), sizeof(height));
    inFile.read(reinterpret_cast<char*>(&channels), sizeof(channels));

    uint32_t encodedSize;
    inFile.read(reinterpret_cast<char*>(&encodedSize), sizeof(encodedSize));
    vector<char> compressedData(encodedSize);
    inFile.read(compressedData.data(), encodedSize);

    uint32_t encodedPatientDataSize;
    inFile.read(reinterpret_cast<char*>(&encodedPatientDataSize), sizeof(encodedPatientDataSize));
    vector<char> compressedPatientData(encodedPatientDataSize);
    inFile.read(compressedPatientData.data(), encodedPatientDataSize);

    uint32_t treeSize;
    inFile.read(reinterpret_cast<char*>(&treeSize), sizeof(treeSize));
    vector<char> compressedTree(treeSize);
    inFile.read(compressedTree.data(), treeSize);

    inFile.close();

    int key[MATRIX_SIZE][MATRIX_SIZE] = {{3, 3}, {2, 5}};
    int mod = MOD;  // Número de caracteres en el conjunto ASCII

    // string decryptedCompressedData = hillDecipher(string(compressedData.begin(), compressedData.end()), key, mod);
    // cout << "Decompressed data: " << decryptedCompressedData << endl;

    string decryptedCompressedPatientData = hillDecipher(string(compressedPatientData.begin(), compressedPatientData.end()), key, mod);
    cout << "Decompressed patient data: " << decryptedCompressedPatientData << endl;

    // string decryptedCompressedTree = hillDecipher(string(compressedTree.begin(), compressedTree.end()), key, mod);
    // cout << "Decompressed tree: " << decryptedCompressedTree << endl;

    // Descomprimir los datos
    // uLongf decompressedSize = decryptedCompressedData.size() * 4; // Estimar tamaño descomprimido
    uLongf decompressedSize = compressedData.size() * 4; // Estimar tamaño descomprimido
    encodedData.resize(decompressedSize);
    int res = uncompress(reinterpret_cast<Bytef*>(&encodedData[0]), &decompressedSize, reinterpret_cast<const Bytef*>(compressedData.data()), compressedData.size());


    // Manejar errores de descompresión
    while (res == Z_BUF_ERROR) {
        decompressedSize *= 2; // Aumentar el tamaño del buffer
        encodedData.resize(decompressedSize);
        res = uncompress(reinterpret_cast<Bytef*>(&encodedData[0]), &decompressedSize, reinterpret_cast<const Bytef*>(compressedData.data()), compressedData.size());
    }

    if (res != Z_OK) {
        cerr << "Error descomprimiendo los datos: " << res << endl;
        return;
    }
    encodedData.resize(decompressedSize);

    // Descomprimir el árbol
    // uLongf decompressedTreeSize = decryptedCompressedTree.size() * 4; // Estimar tamaño descomprimido
    uLongf decompressedTreeSize = compressedTree.size() * 4; // Estimar tamaño descomprimido
    serializedTree.resize(decompressedTreeSize);
    res = uncompress(reinterpret_cast<Bytef*>(&serializedTree[0]), &decompressedTreeSize, reinterpret_cast<const Bytef*>(compressedTree.data()), compressedTree.size());

    // Manejar errores de descompresión
    while (res == Z_BUF_ERROR) {
        decompressedTreeSize *= 2; // Aumentar el tamaño del buffer
        serializedTree.resize(decompressedTreeSize);
        res = uncompress(reinterpret_cast<Bytef*>(&serializedTree[0]), &decompressedTreeSize, reinterpret_cast<const Bytef*>(compressedTree.data()), compressedTree.size());
    }

    if (res != Z_OK) {
        cerr << "Error descomprimiendo el árbol: " << res << endl;
        return;
    }
    serializedTree.resize(decompressedTreeSize);
}

int main() {
    string patientData, encodedData, serializedTree;
    int width, height, channels;

    readFromFile("compressed.pap", encodedData, serializedTree, patientData, width, height, channels);

    int index = 0;
    Node* root = deserializeHuffmanTree(serializedTree, index);

    string decodedString = decode(root, encodedData);
    vector<unsigned char> imageData(decodedString.begin(), decodedString.end());

    saveImage(imageData, width, height, channels, "imagenRecuperada.jpg");

    cout << patientData << endl;
    cout << "Image saved as imagenRecuperada.jpg" << endl;

    return 0;
}