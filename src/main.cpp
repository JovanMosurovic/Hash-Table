#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "DoubleHashing.h"

using namespace std;

const string red = "\033[1;31m";
const string green = "\033[1;32m";
const string resetColor = "\033[0m";
const string underline = "\033[4m";
const string bold = "\033[1m";

const double maxLoadFactor = 0.75; // max table fill ratio
const double maxAvgAccess = 2; // max successful access count
const double minLoadFactor = 0.25; // min table fill ratio

class HashTable {
    struct Element {
        int key;
        string value;
        bool isOccupied;
        bool isDeleted;
        Element() : isOccupied(false), isDeleted(false) {};
        Element(int key, string value) : key(key), value(move(value)), isOccupied(true), isDeleted(false) {}
    };
    DoubleHashing *address;
    unsigned int size;
    float minimalSize;
    unsigned int numberOfElements;
    unsigned int totalAccessSuccess;
    unsigned int searchAccessSuccess = 0;
    unsigned int successAccessStatistics = 0;
    unsigned int searchAccessUnsuccess = 0;
    unsigned int unsuccessAccessStatistics = 0;
    bool adaptive;
    Element* table;

    bool isPrime(int number) {
        if (number <= 1) return false;
        if (number <= 3) return true;

        if (number % 2 == 0 || number % 3 == 0) return false;

        for (int i = 5; i * i <= number; i += 6) {
            if (number % i == 0 || number % (i + 2) == 0) return false;
        }
        return true;
    }
    int findPreviousPrime(int number) {
        int prime = number;
        while (!isPrime(prime)) {
            --prime;
        }
        return prime;
    }

    int getP(unsigned int size) {
        return findPreviousPrime(size);
    }
    int getQ(unsigned int size) {
        int p = findPreviousPrime(size);
        int start = p / 3;
        while(start < p) {
            if(isPrime(start)) {
                return start;
            }
            start++;
        }
        return -1;
    }

    void resizeTable() {
        cout << "We are expanding the table..." << endl;
        unsigned int oldSize = size;
        Element *oldTable = table;

        size *= 2;
        table = new Element[size];

        numberOfElements = 0;
        totalAccessSuccess = 0;

        for (int i = 0; i < oldSize; i++) {
            if (oldTable[i].isOccupied && !oldTable[i].isDeleted) {
                insertKey(oldTable[i].key, oldTable[i].value);
            }
        }
        cout << bold << "The table size has been increased successfully." << resetColor << endl;
        delete[] oldTable;
    }
    void shrinkTable() {
        cout << "We are collapsing the table..." << endl;
        int oldSize = size;
        Element* oldTable = table;

        size /= 2;
        table = new Element[size];

        numberOfElements = 0;
        totalAccessSuccess = 0;

        for (int i = 0; i < oldSize; i++) {
            if (oldTable[i].isOccupied && !oldTable[i].isDeleted) {
                insertKey(oldTable[i].key, oldTable[i].value);
            }
        }

        cout << bold << "Table size reduced successfully." << resetColor << endl;
        delete[] oldTable;
    }
    void reloadTable() {
        vector<Element> elements;
        for (int i = 0; i < size; i++) {
            if (table[i].isOccupied) {
                elements.push_back(table[i]);
            }
        }

        clear();
        size = (minimalSize*2);

        for (const auto& element : elements) {
            insertKey(element.key, element.value);
        }
    }


public:
    HashTable(const int size) : size(size), address(new DoubleHashing(getP(size), getQ(size))), minimalSize(float(size)/2), adaptive(false) { // todo: vrati na true
        table = new Element[size];
        for (int i = 0; i < size; ++i) {
            table[i].isOccupied = false;
            table[i].isDeleted = false;
        }
        numberOfElements = 0;
        totalAccessSuccess = 0;
    }
    ~HashTable() {delete[] table;}

//    unsigned int hashFunction1(int key, int size) {
//        const float A = 0.618033;
//        float temp = key * A;
//        temp -= (int)temp;
//        return (unsigned int)(size * temp);
//    }

    unsigned int hashFunction1(int key, unsigned int size) { // for the assignment
        return abs(key) % size;
    }

    unsigned int hashFunction2(int key, unsigned int index, int i) {
        return (address->getAddress(abs(key), index, i) % size);
    }

//    unsigned int hashFunction2(int key, int index, int i) { // for tests
//        return  (index+4+(key % 2))%size; // mojTest1
//       // return (index + 2 + (key % 3)) % size; //mojTest2
//    }

    string findKeyWithStatistics(long key);
    string findKey(long key);
    bool insertKey(long key, string value);
    bool deleteKey(long key);
    double avgAccessSuccess() const;
    double avgAccessSuccessInstructions() const;
    double avgAccessUnsuccess() const;
    double avgAccessUnsuccessEst() const;
    double avgAccessUnsuccessInstructions() const;
    void resetStatistics();
    void clear();
    unsigned int keyCount() const; // getter for numberOfElements
    unsigned int tableSize() const; // getter for size
    friend ostream& operator<<(ostream& os, const HashTable& ht);
    double fillRatio() const;

    unsigned int getTotalAccessSuccess() const;
    bool isAdaptive() const;
    void setAdaptive(bool adaptive_);
};

//<editor-fold desc="Operations from the HashTable class">

string HashTable::findKeyWithStatistics(long key) {
    unsigned int index = hashFunction1(key, size);
    int numberOfAccess = 0;
    int i = 0;
    while(i < size) {
        numberOfAccess++;
        if(table[index].isOccupied && !table[index].isDeleted && table[index].key == key) {
            searchAccessSuccess += numberOfAccess;
            successAccessStatistics++;
            return table[index].value;
        }
        else {
            index = hashFunction2(key, index, i);
        }
        i++;
    }

    // Additional search
    for(int j = 0; j < size; j++) {
        if(table[j].isOccupied && !table[j].isDeleted && table[j].key == key) {
            return table[j].value;
        }
    }

    cout << bold << red << "The key was not found in the table!" << resetColor << endl;
    searchAccessUnsuccess += numberOfAccess;
    unsuccessAccessStatistics++;
    return "";
}

string HashTable::findKey(long key) {
    unsigned int index = hashFunction1(key, size);
    int i = 0;
    while(i < size) {
        if(table[index].isOccupied && !table[index].isDeleted && table[index].key == key) {
            return table[index].value;
        }
        else {
            index = hashFunction2(key, index, i);
        }
        i++;
    }

    // Additional search
    for(int j = 0; j < size; j++) {
        if(table[j].isOccupied && !table[j].isDeleted && table[j].key == key) {
            return table[j].value;
        }
    }

    return "";
}

bool HashTable::insertKey(long key, string value) {

    if(numberOfElements == size) {
        cout << red << "The table is full!" << resetColor << endl;
        cout << red << "The key " << bold << key << resetColor << red << " was not added to the table!" << resetColor << endl;
        return false;
    }
    if(fillRatio() > maxLoadFactor && adaptive) {
        cout << red << "The table exceeds the fill ratio of " << maxLoadFactor*100 << "%" << resetColor << endl;
        resizeTable();
    }
    if(avgAccessSuccess() > maxAvgAccess && adaptive) {
        cout << red << "The average number of successful accesses exceeds " << bold << maxAvgAccess << resetColor << endl;
        resizeTable();
    }
    if(!findKey(key).empty()) {
        cout << red << "The key " << key << " already exists in the table!" << resetColor << endl;
        return false;
    }
    Element newElem(key, move(value));
    cout << "We are adding a key " << bold << newElem.key << resetColor << " with value \"" << bold << newElem.value << resetColor << "\"..." << endl;

    unsigned int index = hashFunction1(key, size);
    int i = 0;
    while(i < size) {
        totalAccessSuccess++;
        if(!table[index].isOccupied || table[index].isDeleted) {
            table[index] = newElem;
            table[index].isOccupied = true;
            numberOfElements++;
            cout << green << "The element has been successfully added at position: " << resetColor << index << endl;
            return true;
        }
        else {
            index = hashFunction2(key, index, i);
        }
        i++;
    }
    cout << red << "The key " << bold << key << resetColor << red << " was not added to the table!" << resetColor << endl;
    return false;
}

bool HashTable::deleteKey(long key) {
    unsigned int index = hashFunction1(key, size);
    int i = 0;
    bool found = false;

    while(i < size) {
        if(table[index].isOccupied && !table[index].isDeleted && table[index].key == key) {
            table[index].isDeleted = true;
            numberOfElements--;
            found = true;
            break;
        } else {
            index = hashFunction2(key, index, i);
        }
        i++;
    }

    if (!found) {
        cout << red << "The key was not found in the table!" << resetColor << endl;
        return false;
    } else {
        cout << green << "The key " << resetColor << bold <<  key << resetColor << green << " has been successfully deleted from the position: " << resetColor << bold << index << resetColor << endl;
    }

    if (fillRatio() < minLoadFactor && size > minimalSize) {
        shrinkTable();
    }

    return true;
}

double HashTable::avgAccessSuccess() const {
    return (double)(totalAccessSuccess) / numberOfElements;
}

double HashTable::avgAccessSuccessInstructions() const {
    if(successAccessStatistics == 0) return 0;
    return (double)searchAccessSuccess / successAccessStatistics;
}

double HashTable::avgAccessUnsuccess() const {
    double Pnu = 0.0;
    unsigned int notEmpty = numberOfElements;
    double currFraction = (double)numberOfElements/size;
    for (int i = 0; i < size-1 && notEmpty > 0; ++i) {
        Pnu += currFraction;
        notEmpty--;
        currFraction *= (double)((notEmpty)*1.0)/(size-i-1);
    }
    return (Pnu+1);
}

double HashTable::avgAccessUnsuccessEst() const {
    return (1/(1-fillRatio()));
}

double HashTable::avgAccessUnsuccessInstructions() const {
    if(unsuccessAccessStatistics == 0) return 0;
    return (double)searchAccessUnsuccess / unsuccessAccessStatistics;
}

void HashTable::resetStatistics() {
    searchAccessSuccess = 0;
    successAccessStatistics = 0;
    searchAccessUnsuccess = 0;
    unsuccessAccessStatistics = 0;
}

void HashTable::clear() {
    for (int i = 0; i < size; ++i) {
        table[i].isOccupied = false;
        table[i].isDeleted = false;
    }
    numberOfElements = 0;
    cout << green << "The table has been successfully cleared!" << resetColor << endl;
}

unsigned int HashTable::keyCount() const {
    return numberOfElements;
}

unsigned int HashTable::tableSize() const {
    return size;
}

double HashTable::fillRatio() const {
    return (double)numberOfElements / size;
}

ostream& operator<<(ostream& os, const HashTable& ht) {
    cout << underline << bold << "\t\tPRINT TABLE" << resetColor << endl;
    for (int i = 0; i < ht.size; ++i) {
        if (ht.table[i].isOccupied && !ht.table[i].isDeleted) {
            os << '\t' << ht.table[i].key << "\t\t\t" << ht.table[i].value << endl;
        }
        else {
            os <<  "\tEMPTY" <<  endl;
        }
    }
    return os;
}

unsigned int HashTable::getTotalAccessSuccess() const {
    return totalAccessSuccess;
}

bool HashTable::isAdaptive() const {
    return adaptive;
}

void HashTable::setAdaptive(bool adaptive_) {
    this->adaptive = adaptive_;
    reloadTable();
}

//</editor-fold>

void testPerformanceEvaluation(HashTable &ht, unsigned int numOfKeysToInsert) {
    srand(14);

    int minKey = 1000000;
    int maxKey = 9999999;
    for (int i = 0; i < numOfKeysToInsert; ++i) {
        int randomKey = (maxKey - minKey + 1) * ((double) rand() / (RAND_MAX + 1.0)) + minKey;
        cout << "Generated key: " << bold << randomKey << resetColor << endl;
        ht.findKeyWithStatistics(randomKey);
    }

    cout << "Average number of successful access attempts: " << bold << ht.avgAccessSuccessInstructions() << resetColor << endl;
    cout << "Average number of unsuccessful access attempts: " << bold << ht.avgAccessUnsuccessInstructions() << resetColor << endl;
}

HashTable* createHashTableFromFile(const string& filePath) {
    ifstream file(filePath);

    int count = 0;
    string line;
    while (getline(file, line)) {
        count++;
    }

    auto* ht = new HashTable(count);

    file.clear();
    file.seekg(0, ios::beg);

    string key, value;
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, key, '\t');
        getline(ss, value, '\t');
        long keyValue = stol(value);
        ht->insertKey(keyValue, key);
    }

    file.close();
    return ht;
}

void tableSuccessfullyCreated() {
    cout << green;
    printf("\n\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n");
    printf("\xB3       Table successfully created!    \xB3\n");
    printf("\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n");
    cout << resetColor;
}

void finish() {
    printf("\n\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n");
    printf("\xB3         Exiting the program...       \xB3\n");
    printf("\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n");
    printf("----------------------------------------");
    printf("\n\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n");
    printf("\xB3    Program successfully completed!   \xB3\n");
    printf("\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n");
    exit(0);
}

void menu(HashTable &ht) {
    int choice;
    do {
        printf("\n\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n");
        printf("\xB3                 MENU                 \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  1. FIND THE KEY IN THE TABLE        \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  2. INSERT KEY INTO TABLE            \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  3. DELETE KEY FROM TABLE            \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  4. AVERAGE SUCCESSFUL ACCESS NUMBER \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  5. AVERAGE FAILED ACCESS NUMBER     \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  6. EST. AVG FAILED ACCESS           \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  7. RESET STATISTICS                 \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  8. CLEAR THE TABLE                  \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  9. NUMBER OF INSERTED KEYS          \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  10. TABLE SIZE                      \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  11. PRINT TABLE                     \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  12. FILL RATIO                      \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  13. PERFORMANCE EVALUATION          \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  14. ADAPTIVE HASH TABLE             \xB3\n");
        printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
        printf("\xB3  0. EXIT                             \xB3\n");
        printf("\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n");
        cout << "\xB3" << "Enter a number to select your desired option: " << endl << "\xC4>";
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << "You have selected the option \"FIND THE KEY IN THE TABLE\"" << endl;
                cout << "Enter the key you want to find: " << endl << "\xC4>";
                long key;
                cin >> key;
                if(!ht.findKey(key).empty()) {
                    cout << "\nValue found: " << bold << ht.findKey(key) << resetColor << endl;
                    break;
                }
                cout << red << "The key was not found in the table!" << resetColor << endl;

                break;
            }

            case 2: {
                cout << "You have selected the option \"INSERT KEY INTO TABLE\"" << endl;
                cout << "Enter the key you want to insert: " << endl << "\xC4>";
                long key;
                cin >> key;
                cout << "Enter the value you want to insert: " << endl << "\xC4>";
                string value;
                cin >> value;
                ht.insertKey(key, value);
                break;
            }

            case 3: {
                cout << "You have selected the option \"DELETE KEY FROM TABLE\"" << endl;
                cout << "Enter the key you want to delete: " << endl << "\xC4>";
                long key;
                cin >> key;
                ht.deleteKey(key);
                break;
            }

            case 4: {
                cout << "You have selected the option \"AVERAGE SUCCESSFUL ACCESS NUMBER\"" << endl;
                cout << "\nTotal access success: " << ht.getTotalAccessSuccess() << endl;
                cout << "Number of elements: " << ht.keyCount() << endl;
                cout << "The average number of successful accesses is " << bold << ht.avgAccessSuccess() << resetColor << endl;
                break;
            }

            case 5: {
                cout << "You have selected the option \"AVERAGE FAILED ACCESS NUMBER\"" << endl;
                cout << "The average number of failed accesses is " << bold << ht.avgAccessUnsuccess() << resetColor << endl;
                break;
            }

            case 6: {
                cout << "You have selected the option \"EST. AVG FAILED ACCESS\"" << endl;
                cout << "The approximate average number of failed accesses is " << bold << ht.avgAccessUnsuccessEst() << resetColor << endl;
                break;
            }

            case 7: {
                cout << "You have selected the option \"RESET STATISTICS\"" << endl;
                ht.resetStatistics();
                cout << green << "Statistics have been successfully reset!" << resetColor << endl;
                break;
            }

            case 8: {
                cout << "You have selected the option \"CLEAR THE TABLE\"" << endl;
                if (ht.keyCount() == 0) {
                    cout << red << "The table is already empty!" << resetColor << endl;
                    break;
                }
                ht.clear();
                break;
            }

            case 9: {
                cout << "You have selected the option \"NUMBER OF INSERTED KEYS\"" << endl;
                cout << "The number of inserted keys is: " << bold << ht.keyCount() << resetColor << endl;
                break;
            }

            case 10: {
                cout << "You have selected the option \"TABLE SIZE\"" << endl;
                cout << "The table size is: " << bold << ht.tableSize() << resetColor << endl;
                break;
            }

            case 11: {
                cout << "You have selected the option \"PRINT TABLE\"" << endl;
                putchar('\n');
                cout << ht;
                break;
            }

            case 12: {
                cout << "You have selected the option \"FILL RATIO\"" << endl;
                cout << "\nNumber of elements: " << ht.keyCount() << endl;
                cout << "Table size: " << ht.tableSize() << endl;
                cout << "The table fill ratio is: " << bold << ht.fillRatio() << resetColor << endl;
                cout << "The table fill ratio in percentage is: " << bold << ht.fillRatio()*100 << "%" << resetColor << endl;
                break;
            }

            case 13: {
                cout << "You have selected the option \"PERFORMANCE EVALUATION \"" << endl;
                unsigned int numberOfKeysToInsert = ht.tableSize()*10;
                testPerformanceEvaluation(ht, numberOfKeysToInsert);
                ht.resetStatistics();
                break;
            }

            case 14: {
                cout << "You have selected the option \"ADAPTIVE HASH TABLE\"" << endl;
                printf("\n\xDA\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n");
                printf("\xB3         ADAPTIVE HASH TABLE          \xB3\n");
                printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
                printf("\xB3  0. OFF                              \xB3\n");
                printf("\xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n");
                printf("\xB3  1. ON                               \xB3\n");
                printf("\xC0\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n");
                cout << "\xB3" << "Enter a number to select your desired option: " << endl << "\xC4>";
                int choiceForAdaptive;
                cin >> choiceForAdaptive;
                switch (choiceForAdaptive) {
                    case 0:
                        if(!ht.isAdaptive()) {
                            cout << red << "The adaptive hash table is already turned OFF!" << resetColor << endl;
                            break;
                        }
                        ht.setAdaptive(false);
                        cout << bold << "\nThe mode of adaptive hes table is: " << red << "OFF" << resetColor << endl;
                        break;

                    case 1:
                        if(ht.isAdaptive()) {
                            cout << red << "The adaptive hash table is already turned ON!" << resetColor << endl;
                            break;
                        }
                        ht.setAdaptive(true);
                        cout << bold << "\nThe mode of adaptive hes table is: " << green << "ON" << resetColor << endl;
                        break;

                    default:
                        cout << "Wrong choice, please enter the numbers 0 ili 1." << endl;
                }
                break;
            }

            case 0:
                finish();
                break;

            default:
                cout << "Wrong choice, please enter the numbers 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 or 0 to exit." << endl;
        }

    } while (choice != 0);
}

//<editor-fold desc="Testing">

void myTest1() {
    HashTable ht = *new HashTable(9);
    // Testing insertKey method
    cout << "Adding keys to the table:" << endl;
    // keys 38, 28, 33, 20, 23 i 39
    ht.insertKey(38, "Key 38");
    ht.insertKey(28, "Key 28");
    ht.insertKey(33, "Key 33");
    ht.insertKey(20, "Key 20");
    ht.insertKey(23, "Key 23");
    ht.insertKey(39, "Key 39");

    cout << ht;

    cout << "Average number of successful access attempts: " << ht.avgAccessSuccess() << endl;
    cout << "Average number of unsuccessful access attempts: " << ht.avgAccessUnsuccess() << endl;

    // Testiranje fillRatio metode
    cout << "The table fill ratio: " << ht.fillRatio() << endl;

}

void myTest2() {
    HashTable ht = *new HashTable(7);
    ht.insertKey(18, "Key 18");
    ht.insertKey(23, "Key 23");
    ht.insertKey(4, "Key 4");
    ht.insertKey(13, "Key 13");
    ht.insertKey(8, "Key 8");

    cout << ht;

    cout << "Average number of successful access attempts: " << ht.avgAccessSuccess() << endl;
    cout << "Average number of unsuccessful access attempts: " << ht.avgAccessUnsuccess() << endl;

    // Testiranje fillRatio metode
    cout << "The table fill ratio: " << ht.fillRatio() << endl;

}

void fileTest() {
    //HashTable ht = *createHashTableFromFile("DZ3_Recnik_10K.txt");
    HashTable ht = *createHashTableFromFile("test.txt");
    cout << ht;
    cout << "Average number of successful access attempts: " << ht.avgAccessSuccess() << endl;
    cout << "Average number of unsuccessful access attempts: " << ht.avgAccessUnsuccess() << endl;

    // Testiranje fillRatio metode
    cout << "The table fill ratio: " << ht.fillRatio() << endl;
}

//</editor-fold>

int main() {

    HashTable *ht = createHashTableFromFile("DZ3_Recnik_10k.txt");
    tableSuccessfullyCreated();
    menu(*ht);

    delete ht;
    return 0;
}
