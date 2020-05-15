#include "table.h"
//#include "re.h"

void Item::apply() {
    switch (type) {
        case A:
            std::cout << "+ ";
            break;
        case S:
            std::cout << "- ";
            break;
        case M:
            std::cout << "* ";
            break;
        case D:
            std::cout << "/ ";
            break;
        case O:
            std::cout << "% ";
            break;
    }
}

void NumItem::apply() {
    if(type == N)
        std::cout << std::to_string(value) + ' ';
    else
        std::cout << std::to_string(value) + "id ";
}

void TextItem::apply() {
    std::cout << str;
}

Column_struct::Column_struct(int size) {
    columns = new Column[size];
    _size = size;
}

Column_struct::Column_struct(const Column_struct & col) {
    _size = col._size;
    columns = new Column [_size];
    for (int i = 0; i < _size; i++)
        columns[i] = col.columns[i];
}

Column_struct & Column_struct::operator=(const Column_struct & col) {
    _size = col._size;
    columns = new Column [_size];
    for (int i = 0; i < _size; i++)
        columns[i] = col.columns[i];
    return *this;
}

Column_struct::~Column_struct() {
    if(_size)
        delete [] columns;
}

Column & Column_struct::operator[](int index) const {
    return columns[index];
}

Poliz::Poliz(const Poliz & items) {
    _size = items._size;
    ptr = new Item*[_size];
    for (int i = 0; i < _size; i++)
        ptr[i] = new Item(*(items[i]));
}

Poliz::~Poliz() {
    if(_size) {
        for (int i = 0; i < _size; i++)
            delete ptr[i];
        delete [] ptr;
    }
}

const Item* Poliz::operator[](int index) const {
    return ptr[index];
}

Poliz & Poliz::operator=(const Poliz & items) {
    _size = items._size;
    ptr = new Item*[_size];
    for (int i = 0; i < _size; i++)
        ptr[i] = new Item(*(items[i]));
    return *this;
}

void Poliz::push(Item* item) {
    Item** ptr_2 = new Item*[_size + 1];
    if(_size)  {
        for(int i = 0; i < _size; i++)
            ptr_2[i] = ptr[i];
        ptr_2[_size] = item;
        delete [] ptr;
        ptr = ptr_2;
        ++_size;
    } else {
        _size = 1;
        ptr = ptr_2;
        *ptr = item;
    }
}

int Poliz::get_size() {
    return _size;
}

void Poliz::print() {
    for(int i = 0; i < _size; i++)
        ptr[i]->apply();
}

int Column_struct::field_id(std::string str) const {
    int id = 0;
    int i = 0;
    while(!id && (i < _size)) {
        if(str == columns[i].name)
            id = i + 1;
        i++;
    }
    return id;
}

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
    columns = Column_struct(size);
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

const Column_struct Table::get_struct() {
    return columns;
}

int Table::get_size() {
    return size;
}