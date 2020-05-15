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

class Column_struct {
    Column* columns;
    int _size;
public:
    Column_struct() {_size = 0;}
    Column_struct(int size);
    Column_struct(const Column_struct &);
    ~Column_struct();
    Column & operator[](int index) const;
    Column_struct & operator=(const Column_struct & );
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
};

struct NumItem: Item {
    int value;
};

struct TextItem: Item {
    std::string str;
};

class Poliz {
    Item** ptr;
    int _size;
public:
    Poliz() {_size = 0;}
    Poliz(const Poliz &);
    ~Poliz();
    Poliz & operator=(const Poliz &);
    const Item* operator[](int index) const;
    void push(Item*);
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