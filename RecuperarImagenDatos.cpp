#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Node {
    char ch;
    Node* left;
    Node* right;

    Node(char c) : ch(c), left(nullptr), right(nullptr) {}
};

struct Patient {
    std::string name;
    int age;
    float height;
    float weight;
    std::string diagnosisDate;
    std::string diagnosis;
};

Node* deserializeHuffmanTree(const std::string& str, int& index) {
    if (index >= str.size()) return nullptr;

    if (str[index] == '1') {
        return new Node(str[++index]);
    }

    Node* node = new Node('\0');
    node->left = deserializeHuffmanTree(str, ++index);
    node->right = deserializeHuffmanTree(str, ++index);
    return node;
}

std::string decode(Node* root, const std::string& encodedData) {
    std::string decodedString;
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

void saveImage(const std::vector<unsigned char>& imageData, int width, int height, int channels, const std::string& filename) {
    stbi_write_jpg(filename.c_str(), width, height, channels, imageData.data(), 100);
}

void readFromFile(const std::string& filename, std::string& encodedData, std::string& serializedTree, int& width, int& height, int& channels, Patient& patient) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening file for reading." << std::endl;
        return;
    }

    inFile.read(reinterpret_cast<char*>(&width), sizeof(width));
    inFile.read(reinterpret_cast<char*>(&height), sizeof(height));
    inFile.read(reinterpret_cast<char*>(&channels), sizeof(channels));

    std::getline(inFile, patient.name);
    patient.name = patient.name.substr(6); // Remove the "Name: " prefix

    std::string line;
    std::getline(inFile, line);
    patient.age = std::stoi(line.substr(5)); // Remove the "Age: " prefix

    std::getline(inFile, line);
    patient.height = std::stof(line.substr(8)); // Remove the "Height: " prefix

    std::getline(inFile, line);
    patient.weight = std::stof(line.substr(8)); // Remove the "Weight: " prefix

    std::getline(inFile, patient.diagnosisDate);
    patient.diagnosisDate = patient.diagnosisDate.substr(16); // Remove the "Diagnosis Date: " prefix

    std::getline(inFile, patient.diagnosis);
    patient.diagnosis = patient.diagnosis.substr(11); // Remove the "Diagnosis: " prefix

    uint32_t encodedSize;
    inFile.read(reinterpret_cast<char*>(&encodedSize), sizeof(encodedSize));
    encodedData.resize(encodedSize);
    inFile.read(&encodedData[0], encodedSize);

    uint32_t treeSize;
    inFile.read(reinterpret_cast<char*>(&treeSize), sizeof(treeSize));
    serializedTree.resize(treeSize);
    inFile.read(&serializedTree[0], treeSize);

    inFile.close();
}

int main() {
    std::string encodedData, serializedTree;
    int width, height, channels;
    Patient patient;

    readFromFile("compressed.pap", encodedData, serializedTree, width, height, channels, patient);

    int index = 0;
    Node* root = deserializeHuffmanTree(serializedTree, index);

    std::string decodedString = decode(root, encodedData);
    std::vector<unsigned char> imageData(decodedString.begin(), decodedString.end());

    saveImage(imageData, width, height, channels, "imagenRecuperada.jpg");

    std::cout << "Patient Data:" << std::endl;
    std::cout << "Name: " << patient.name << std::endl;
    std::cout << "Age: " << patient.age << std::endl;
    std::cout << "Height: " << patient.height << std::endl;
    std::cout << "Weight: " << patient.weight << std::endl;
    std::cout << "Diagnosis Date: " << patient.diagnosisDate << std::endl;
    std::cout << "Diagnosis: " << patient.diagnosis << std::endl;

    std::cout << "Image saved as imagenRecuperada.jpg" << std::endl;

    return 0;
}
