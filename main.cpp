#include <windows.h> // SetConsoleCP, SetConsoleOutputCP
#include <iostream>
#include <cstdlib> // rand, srand
#include <ctime> // time
#include <fstream>
#include <string>
#include <algorithm> // find_if
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

string getTrimmedString(std::string str, std::string const &whiteSpaces = " \r\n\t\v\f") {
    auto start = str.find_first_not_of(whiteSpaces);
    str.erase(0, start);
    auto end = str.find_last_not_of(whiteSpaces);
    str.erase(end + 1);

    return str;
}

std::string getUserString(const string &propose) {
    while (true) {
        string userInput;
        printf("%s: ", propose.c_str());
        std::getline(std::cin, userInput);

        userInput = getTrimmedString(userInput);
        if (userInput.empty()) {
            std::cout << "Строка не может быть пустой. Попробуйте снова!" << std::endl;
            continue;
        }

        return userInput;
    }
}

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
    const bool SUCCESS = true;
    const bool FAILURE = false;

    std::ifstream fileReader(fileName, std::ios::binary);
    if (!fileReader.is_open() && fileReader.bad()) {
        fileReader.close();
        return FAILURE;
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

    return SUCCESS;
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

bool startOfCashMachine(const char* path) {
    const bool SUCCESS = true;
    const bool FAILURE = false;
    const int NEW_FILE_CREATED = 1;
    string data;

    std::cout << "Добро пожаловать и спасибо, что воспользовались нашим банкоматом." << std::endl;

    int initializationFlag = initialization(path);

    bool isReadSuccessfully = readFromBinaryFile(path, data);

    if (!isReadSuccessfully) {
        std::cout << "Чтение из файла закончилось неудачей. Проверьте файл" << std::endl;
        return FAILURE;
    }

    if (initializationFlag == NEW_FILE_CREATED) {
        std::cout << "Инициализация: в банкомат добавлена сумма в размере: " << data << std::endl;
    } else {
        std::cout << "С прошлого раза сохранилась сумма в размере: " << data << std::endl;
    }

    return SUCCESS;
}

string getIndexOfMenu() {
    const char* menu[] = {
            "нажмите + чтобы добавить сумму",
            "нажмите - чтобы снять сумму",
            "введите любой другой знак, чтобы завершить программу"
    };

    int sizeOfMenu = std::end(menu) - std::begin(menu);

    for (int i = 0; i < sizeOfMenu; ++i) {
        std::cout << i << ": " << menu[i] << std::endl;
    }

    return getUserString("Введите цифру");
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::srand(std::time(nullptr)); // NOLINT(cert-msc51-cpp)

    string data;

    const char* path = R"(..\cash_machine.bin)";

    bool statusCashMachine = startOfCashMachine(path);

    if (!statusCashMachine) {
        system("pause");
        return 0;
    }

    while(true) {
        char userSelect = getIndexOfMenu()[0];

        switch (userSelect) {
            case '+':
                break;
            case '-':
                break;
            default:
        }
    }

    std::cout << "" << std::endl;
}