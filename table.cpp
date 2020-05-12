#include "table.h"

std::string Table::next_word() {
    int c;
    std::string word;
    do
        c = fgetc(fd);
    while(std::isspace(c));
    word += c;
    while(!std::isspace(c = fgetc(fd)) && (c != EOF))
        word += c;
    return word;
}

Table::Table(std::string _file) {
    const char* word;
    file = _file;
    fd = fopen(_file.data(), "r");
    size = atoi(next_word().data());
    columns = new Column[size];
    for (int i = 0; i < size; i++) {
        columns[i].name = next_word();
        word = next_word().data();
        if(word[0] == 'T') {
            word += 1;
            columns[i].type = true;
            columns[i].size = atoi(word);
        } else {
            columns[i].type = false;
        }
    }
}

Table::~Table() {
    //delete columns;
    fclose(fd);
}

void Table::print() {
    for (int i = 0; i < size; i++) {
        std::cout << columns[i].name << " ";
        if(columns[i].type)
            std::cout << "text" << columns[i].size;
        else
            std::cout << "long";
        std::cout << std::endl;
    }
}

int main() {
    std::string str;
    std::cin >> str;
    Table A(str);
    A.print();
}