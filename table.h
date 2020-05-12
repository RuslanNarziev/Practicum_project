#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Column {
    bool type;
    int size;
    std::string name;
};

/*
class Field {
    virtual ~Field();
};

class Num : public Field {
    long num;
};

class Str : public Field {
    char* str;
    unsigned char size;
    ~Field();
}; */


class Table {
    FILE* fd;
    int size;
    Column* columns;
    std::string file;
    std::string next_word(); 
public:
    Table(std::string _file);
    ~Table();
    void print();
};