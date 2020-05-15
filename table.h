#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Column {
public:
    bool type;
    int size;
    std::string name;
    Column() {std::cout << "constr";}
    ~Column() {std::cout << "destr";}
};

class Column_struct {
    Column* columns;
    int _size;
public:
    Column_struct() {_size = 0; std::cout << "3\n";}
    Column_struct(int size);
    Column_struct(const Column_struct &);
    ~Column_struct();
    Column & operator[](int index) const;
    int field_id(std::string) const;
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
};
*/
enum poliz_t {A, S, I, N, M, D, O};

struct Item {
    enum poliz_t type;
    int value;
};

class Poliz {
    Item* ptr;
    int _size;
public:
    Poliz() {_size = 0;}
    Poliz(const Poliz &);
    ~Poliz();
    Item & operator[](int index) const;
    void push(Item);
    int get_size();
    std::string print();
};

class Table {
    FILE* fd;
    int size;
    Column_struct columns;
    std::string file;
    std::string next_word(); 
    bool where_re();
public:
    Table(std::string);
    ~Table();
    void print();
    const Column_struct get_struct();
    int get_size();
    int where_type;
    int where_id;
    bool where_not;
    std::string where_str;
    Poliz where_poliz;
};