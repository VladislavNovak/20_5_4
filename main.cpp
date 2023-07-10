#include <windows.h> // SetConsoleCP, SetConsoleOutputCP
#include <iostream>
#include <cstdlib> // rand, srand
#include <ctime> // time
#include <fstream>
#include <string>

using std::cout;
using std::endl;
using std::string;

int getRoundedIntWithStep(int val, int step = 10) {
    return (val + step / 2) / step * step;
}

int getRandomIntFromRange(int from, int to) {
    return (from + std::rand() % (to - from + 1)); // NOLINT(cert-msc50-cpp)
}

// ранее isFileExist
bool hasFileExist(const char* path) {
    std::ifstream file(path);
    return file.is_open() && !file.bad();
}

bool readFromBinaryFile(const char* fileName, string &data) {
    std::ifstream fileReader(fileName, std::ios::binary);
    if (!fileReader.is_open() && fileReader.bad()) {
        fileReader.close();
        return false;
    }

    fileReader.seekg(0, std::ifstream::end);

    int bufferSize = (int)fileReader.tellg();
    char* buffer = new char[bufferSize + 1];
    buffer[bufferSize] = '\0';

    fileReader.seekg(0, std::ifstream::beg);

    fileReader.read(buffer, bufferSize);

    data += buffer;
    delete[] buffer;

    fileReader.close();

    return true;
}

void writeToBinaryFile(const char* path, const string &data, bool isAppMode = false, const char delim = ';') {
    std::ofstream file(path, std::ios::binary | (isAppMode ? std::ios::app : std::ios::out));

    if (isAppMode) file << delim;

    file.write(data.c_str(), strlen(data.c_str())); // NOLINT(cppcoreguidelines-narrowing-conversions)

    file.close();
}


int initialization(const char* path) {
    const int READ_EXISTING_FILE = 0;
    const int NEW_FILE_CREATED = 1;

    // Проверяем существование файла cash_machine.bin и если он существует, возвращаем соответствующий флаг
    if (hasFileExist(path))
        return READ_EXISTING_FILE;

    // В противном случае генерируем начальное число и записываем зачисляем его в файл с банкоматом
    int currentCash = getRoundedIntWithStep(getRandomIntFromRange(0, 5000), 100);
    writeToBinaryFile(path, std::to_string(currentCash));

    // возвращаем соответствующий флаг
    return NEW_FILE_CREATED;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::srand(std::time(nullptr)); // NOLINT(cert-msc51-cpp)

    const char* path = R"(..\cash_machine.bin)";
    string data;

    initialization(path);
    readFromBinaryFile(path, data);

    cout << "result: " << data << ": " << data.length() << endl;

    for (auto i : data) cout << ":" << i << endl;
}