#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <functional>
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

void saveToFile(const std::string& filename, const std::string& encodedData, const std::string& serializedTree, int width, int height, int channels, const Patient& patient) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    outFile.write(reinterpret_cast<const char*>(&width), sizeof(width));
    outFile.write(reinterpret_cast<const char*>(&height), sizeof(height));
    outFile.write(reinterpret_cast<const char*>(&channels), sizeof(channels));

    outFile << "Name: " << patient.name << "\n";
    outFile << "Age: " << patient.age << "\n";
    outFile << "Height: " << patient.height << "\n";
    outFile << "Weight: " << patient.weight << "\n";
    outFile << "Diagnosis Date: " << patient.diagnosisDate << "\n";
    outFile << "Diagnosis: " << patient.diagnosis << "\n";

    uint32_t encodedSize = encodedData.size();
    outFile.write(reinterpret_cast<const char*>(&encodedSize), sizeof(encodedSize));
    outFile.write(encodedData.c_str(), encodedSize);

    uint32_t treeSize = serializedTree.size();
    outFile.write(reinterpret_cast<const char*>(&treeSize), sizeof(treeSize));
    outFile.write(serializedTree.c_str(), treeSize);

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

    saveToFile("compressed.pap", encodedData, serializedTree, width, height, channels, patient);

    std::cout << "Image and patient data compressed and saved as compressed.pap" << std::endl;

    return 0;
}
