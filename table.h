#include <iostream>
#include <fstream>
#include <sstream> 
#include <stdlib.h>
#include <string.h>
#include <vector>

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

class Record {
    std::string* fields;
public:
    Record(int size) {fields = new std::string[size];}
    ~Record() {delete [] fields;} 
    std::string & operator[](int index) {return fields[index];}
};

enum poliz_t {A, S, I, N, M, D, O, B, NO, ST, AND, OR, T};

struct Item {
    enum poliz_t type;
    virtual void apply(std::vector<long> &, Record& , std::string& , std::string& , bool &);
    virtual ~Item() = default;
};

struct NumItem: Item {
    long value;
    void apply(std::vector<long> &, Record& , std::string& , std::string& , bool &);
};

struct TextItem: Item {
    std::string str;
    void apply(std::vector<long> &, Record& , std::string& , std::string& , bool &);
};

struct BoolItem : Item {
    bool reverse;
    bool equal;
    bool less;
    void apply(std::vector<long> &, Record& , std::string& , std::string& , bool &);
};

class Poliz {
    Item** ptr;
    int _size;
public:
    static int count;
    Poliz() {_size = 0;}
    Poliz(Poliz &);
    ~Poliz();
    Poliz & operator=(Poliz &);
    Item* operator[](int index) const;
    void push(Item*);
    int get_size() const;
    long exec(Record &) const;
};

class Table {
    std::string name;
    std::string header;
    std::ifstream file;
    int size;
    Column_struct columns;
    std::string next_word(const char* &); 
    bool where_re(const char*, const char*);
    bool where(Record &);
    void get_record(Record &, const char* );
public:
    Table(std::string);
    ~Table();
    const Column_struct get_struct();
    int get_size();
    int where_type;
    int where_id;
    int where_count;
    bool where_not;
    std::string where_str;
    Poliz where_poliz;
    void delet();
    void update(int type, std::string &, int first_id, int second_id, const Poliz &);
    void select(std::string &, std::vector<int> &);
};