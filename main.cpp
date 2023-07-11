#include <windows.h> // SetConsoleCP, SetConsoleOutputCP
#include <iostream>
#include <cstdlib> // rand, srand
#include <ctime> // time
#include <fstream>
#include <sstream> // stringstream
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

vector<string> getSplitStringOnRecords(string const &str, const char delim = ',', bool isEmptyDenied = true) {
    vector<string> records;
    std::stringstream ss(str);

    string rawRecord;
    // Делим строки на токены по delim
    while (std::getline(ss, rawRecord, delim)) {
        string record = getTrimmedString(rawRecord);
        // Не позволяет добавлять пустой элемент
        if (record.empty() && isEmptyDenied) continue;

        records.push_back(record);
    }

    return records;
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

        return getSplitStringOnRecords(userInput, ' ')[0];
    }
}

bool isNumeric(std::string const &str) {
    auto it = std::find_if(
            str.begin(),
            str.end(),
            [](char const &c) { return !std::isdigit(c); });

    return !str.empty() && it == str.end();
}

// Если from != to, тогда ввести цифры возможно лишь в диапазоне от from до to
int getUserNumeric(const string &msg = "Введите цифры", int from = 0, int to = 0) {
    string warning = "Попробуйте снова. Это должно быть целое число";
    bool isRange = (from != to);

    while (true) {
        string userInput = getUserString(msg);

        if (!isNumeric(userInput)) {
            printf("%s\n", warning.c_str());
            continue;
        }

        int num = std::stoi(userInput);

        if (isRange && (num < from || num > to)) {
            printf("%s в диапазоне (%i - %i)\n", warning.c_str(), from, to);
            continue;
        }

        return num;
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
    bool result = false;

    std::ifstream file(path);

    if (file.is_open() && !file.bad()) result = true;

    file.close();

    return result;
}

bool readFromBinaryFile(const char* fileName, string &data) {
    const bool SUCCESS = true;
    const bool FAILURE = false;

    std::ifstream fileReader(fileName, std::ios::binary);

    if (!hasFileExist(fileName)) {
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

    std::cout << "Добро пожаловать и спасибо, что воспользовались нашим банкоматом." << std::endl;

    int initializationFlag = initialization(path);

    if (!hasFileExist(path)) {
        std::cout << "Чтение из файла закончилось неудачей. Проверьте файл" << std::endl;
        return FAILURE;
    }

    const int NEW_FILE_CREATED = 1;
    string data;
    readFromBinaryFile(path, data);

    if (initializationFlag == NEW_FILE_CREATED) {
        std::cout << "Инициализация: в банкомат добавлена сумма в размере: " << data << std::endl;
    } else {
        std::cout << "С прошлого раза сохранилась сумма в размере: " << data << std::endl;
    }

    return SUCCESS;
}

int getUserNumericWithStep(int allowedAmount) {
    int userEnteredMoney;
    const int STEP = 100;

    string msg = "Введите сумму с шагом в " + std::to_string(STEP);
    bool isCorrectAmount = false;

    while (!isCorrectAmount) {
        userEnteredMoney = getUserNumeric(msg, STEP, allowedAmount);
        int roundedMoney = getRoundedIntWithStep(userEnteredMoney, STEP);

        if (userEnteredMoney != roundedMoney) {
            std::cout << "Данную сумму автомат не может принять. Введите круглую сумму." << std::endl;
            continue;
        }

        isCorrectAmount = true;
    }

    return userEnteredMoney;
}

void showBalance(const char* path) {
    string data;
    readFromBinaryFile(path, data);

    std::cout << "У Вас на балансе: " << data << std::endl;
}

void calculateBalance(const char* path, int limit, bool isAddMode) {
    int userEnteredMoney;
    string data;

    readFromBinaryFile(path, data);

    int balance = std::stoi(data);

    if (isAddMode && balance >= limit) {
        printf("В банкомате максимальное количество денег: %i. Их возможно лишь снять\n", limit);
        return;
    }

    if (!isAddMode && balance <= limit) {
        printf("В банкомате минимальное количество денег: %i. Их возможно лишь добавить\n", limit);
        return;
    }

    std::cout << "В банкомате сейчас денег на сумму: " << data << std::endl;
    int allowedAmount = isAddMode ? limit - balance : balance;
    userEnteredMoney = getUserNumericWithStep(allowedAmount);
    int finalCash = isAddMode ? balance + userEnteredMoney : balance - userEnteredMoney;

    writeToBinaryFile(path, std::to_string(finalCash));

    std::cout << "Изменение баланса прошло успешно" << std::endl;
}

void addToBalance(const char* path) {
    const int MAX_MONEY = 5000;
    calculateBalance(path, MAX_MONEY, true);
}

void decreaseCash(const char* path) {
    const int MIN_MONEY = 0;
    calculateBalance(path, MIN_MONEY, false);
}

string getIndexOfMenu() {
    const char* menu[] = {
            "       +           :чтобы добавить сумму",
            "       -           :чтобы снять сумму",
            "       ?           :чтобы снять сумму",
            "любой другой знак  :чтобы завершить программу"
    };

    int sizeOfMenu = std::end(menu) - std::begin(menu);

    std::cout << "------------МЕНЮ------------" << std::endl;

    for (int i = 0; i < sizeOfMenu; ++i) {
        std::cout << menu[i] << std::endl;
    }

    return getUserString("Введите тип операции");
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::srand(std::time(nullptr)); // NOLINT(cert-msc51-cpp)

    string data;

    const char* path = R"(..\cash_machine.bin)";

    bool statusCashMachine = startOfCashMachine(path);

    if (!statusCashMachine) {
        std::cout << "Что-то пошло не так" << std::endl;
        system("pause");
        return 0;
    }

    while(true) {
        system("pause");
        system("cls");
        char userSelect = getIndexOfMenu()[0];

        if (!hasFileExist(path)) {
            std::cout << "Чтение из файла закончилось неудачей. Проверьте файл" << std::endl;
            break;
        }

        switch (userSelect) {
            case '+':
                addToBalance(path);
                continue;
            case '-':
                decreaseCash(path);
                continue;
            case '?':
                showBalance(path);
                continue;
            default:
                showBalance(path);
        }

        break;
    }

    std::cout << "Завершение программы" << std::endl;
}