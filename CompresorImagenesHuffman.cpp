#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <functional>
#include <zlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Patient {
    std::string name;
    int age;
    float height;
    float weight;
    std::string diagnosisDate;
    std::string diagnosis;
};

struct compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

void buildHuffmanTree(const std::vector<unsigned char>& data, Node*& root, std::map<char, std::string>& huffmanCode) {
    std::map<char, int> freq;
    for (char ch : data) {
        freq[ch]++;
    }

    std::priority_queue<Node*, std::vector<Node*>, compare> pq;
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        int sum = left->freq + right->freq;
        Node* node = new Node('\0', sum);
        node->left = left;
        node->right = right;
        pq.push(node);
    }

    root = pq.top();

    std::function<void(Node*, std::string)> buildCode = [&](Node* node, std::string str) {
        if (!node) return;
        if (!node->left && !node->right) {
            huffmanCode[node->ch] = str;
        }
        buildCode(node->left, str + "0");
        buildCode(node->right, str + "1");
    };

    buildCode(root, "");
}

std::string encode(const std::vector<unsigned char>& data, const std::map<char, std::string>& huffmanCode) {
    std::string encodedString;
    for (char ch : data) {
        encodedString += huffmanCode.at(ch);
    }
    return encodedString;
}

void saveHuffmanTree(Node* root, std::string& str) {
    if (!root) return;
    if (!root->left && !root->right) {
        str += "1";
        str += root->ch;
        return;
    }
    str += "0";
    saveHuffmanTree(root->left, str);
    saveHuffmanTree(root->right, str);
}

void saveToFile(const std::string& filename, const std::string& encryptedData, const std::string& encryptedTree, const std::string& patientData, int width, int height, int channels) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    outFile.write(reinterpret_cast<const char*>(&width), sizeof(width));
    outFile.write(reinterpret_cast<const char*>(&height), sizeof(height));
    outFile.write(reinterpret_cast<const char*>(&channels), sizeof(channels));

    uint32_t encryptedSize = encryptedData.size();
    outFile.write(reinterpret_cast<const char*>(&encryptedSize), sizeof(encryptedSize));
    outFile.write(encryptedData.c_str(), encryptedSize);

    uint32_t encryptedPatientDataSize = patientData.size();
    outFile.write(reinterpret_cast<const char*>(&encryptedPatientDataSize), sizeof(encryptedPatientDataSize));
    outFile.write(patientData.c_str(), encryptedPatientDataSize);

    uint32_t treeSize = encryptedTree.size();
    outFile.write(reinterpret_cast<const char*>(&treeSize), sizeof(treeSize));
    outFile.write(encryptedTree.c_str(), treeSize);

    outFile.close();
}

void getPatientData(Patient& patient) {
    std::cout << "Enter name: ";
    std::getline(std::cin, patient.name);
    std::cout << "Enter age: ";
    std::cin >> patient.age;
    std::cout << "Enter height: ";
    std::cin >> patient.height;
    std::cout << "Enter weight: ";
    std::cin >> patient.weight;
    std::cin.ignore(); // clear the input buffer
    std::cout << "Enter diagnosis date: ";
    std::getline(std::cin, patient.diagnosisDate);
    std::cout << "Enter diagnosis: ";
    std::getline(std::cin, patient.diagnosis);
}


// Función para multiplicar matrices y aplicar módulo
void multiplyMatrix(int matrix[2][2], int vector[2], int result[2], int mod) {
    for (int i = 0; i < 2; i++) {
        result[i] = 0;
        for (int j = 0; j < 2; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
        result[i] = result[i] % mod;
    }
}

// Función para cifrar el texto utilizando el cifrado de Hill
std::string hillCipher(const std::string& text, int key[2][2], int mod) {
    // Padding con un carácter especial (ASCII 0)
    std::string paddedText = text;
    char paddingChar = '\0';
    while (paddedText.length() % 2 != 0) {
        paddedText += paddingChar;
    }

    std::string encryptedText = "";
    for (size_t i = 0; i < paddedText.length(); i += 2) {
        int vector[2];
        for (int j = 0; j < 2; j++) {
            vector[j] = static_cast<unsigned char>(paddedText[i + j]);
        }
        int result[2];
        multiplyMatrix(key, vector, result, mod);
        for (int j = 0; j < 2; j++) {
            encryptedText += static_cast<char>(result[j]);
        }
    }
    return encryptedText;
}

int main() {
    Patient patient;
    getPatientData(patient);

    std::string filename;
    std::cout << "Enter image filename (with .jpg extension): ";
    std::cin >> filename;

    int width, height, channels;
    unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (img == nullptr) {
        std::cerr << "Could not open or find the image." << std::endl;
        return -1;
    }

    std::vector<unsigned char> data(img, img + width * height * channels);
    stbi_image_free(img);

    Node* root = nullptr;
    std::map<char, std::string> huffmanCode;
    buildHuffmanTree(data, root, huffmanCode);

    std::string encodedData = encode(data, huffmanCode);

    std::string serializedTree;
    saveHuffmanTree(root, serializedTree);

    // Compress the encoded data and serialized tree using zlib
    uLongf compressedSize = compressBound(encodedData.size());
    std::vector<char> compressedData(compressedSize);
    compress(reinterpret_cast<Bytef*>(compressedData.data()), &compressedSize, reinterpret_cast<const Bytef*>(encodedData.data()), encodedData.size());

    uLongf compressedTreeSize = compressBound(serializedTree.size());
    std::vector<char> compressedTree(compressedTreeSize);
    compress(reinterpret_cast<Bytef*>(compressedTree.data()), &compressedTreeSize, reinterpret_cast<const Bytef*>(serializedTree.data()), serializedTree.size());

    // Encrypt the compressed data and compressed tree using Hill cipher
    int key[2][2] = {{3, 3}, {2, 5}};
    int mod = 256;

    // Encrypt patient data
    std::string patientData = "Name: " + patient.name + "\n" +
                              "Age: " + std::to_string(patient.age) + "\n" +
                              "Height: " + std::to_string(patient.height) + "\n" +
                              "Weight: " + std::to_string(patient.weight) + "\n" +
                              "Diagnosis Date: " + patient.diagnosisDate + "\n" +
                              "Diagnosis: " + patient.diagnosis + "\n";
    std::string encryptedPatientData = hillCipher(patientData, key, mod);

    saveToFile("compressed.pap", std::string(compressedData.begin(), compressedData.begin() + compressedSize), std::string(compressedTree.begin(), compressedTree.begin() + compressedTreeSize), encryptedPatientData, width, height, channels);

    std::cout << "Image and patient data compressed, encrypted, and saved as compressed.pap" << std::endl;

    return 0;
}
