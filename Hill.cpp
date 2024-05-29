#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;

const int MATRIX_SIZE = 2;
const int MOD = 256;  // Todos los caracteres ASCII

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

// Función para cifrar el texto utilizando el cifrado de Hill
string hillCipher(string text, int key[MATRIX_SIZE][MATRIX_SIZE], int mod) {
    // Padding con un carácter especial (ASCII 0)
    char paddingChar = '\0';
    while (text.length() % MATRIX_SIZE != 0) {
        text += paddingChar;
    }

    string encryptedText = "";
    for (size_t i = 0; i < text.length(); i += MATRIX_SIZE) {
        int vector[MATRIX_SIZE];
        for (int j = 0; j < MATRIX_SIZE; j++) {
            vector[j] = static_cast<unsigned char>(text[i + j]);
        }
        int result[MATRIX_SIZE];
        multiplyMatrix(key, vector, result, mod);
        for (int j = 0; j < MATRIX_SIZE; j++) {
            encryptedText += static_cast<char>(result[j]);
        }
    }
    return encryptedText;
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
    decryptedText.erase(std::remove(decryptedText.begin(), decryptedText.end(), '\0'), decryptedText.end());
    return decryptedText;
}

int main() {
    int key[MATRIX_SIZE][MATRIX_SIZE] = {{3, 3}, {2, 5}};
    int mod = MOD;

    string text;
    cout << "Ingrese el texto a cifrar: ";
    getline(cin, text);

    string encryptedText = hillCipher(text, key, mod);
    cout << "Texto cifrado: " << encryptedText << endl;

    string decryptedText = hillDecipher(encryptedText, key, mod);
    cout << "Texto descifrado: " << decryptedText << endl;

    return 0;
}
