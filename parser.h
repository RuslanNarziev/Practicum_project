#include <iostream>
#include "table.h"
#include <unistd.h>

enum lex_t {ID, NUM, OTHER, END, STR};

class Parser {    
    int should_be_closed;
    int open_count;
    bool can_be_num;
    char c;
    enum lex_t cur_lex_type;
    std::string cur_lex_text;
    const char* str;
    bool err;
    void next();
    std::string answer;
    void create();
    std::string field(std::vector<std::string> &);
    void drop();
    void insert();
    std::string value(const Column &, int);
    void where(Table & );
    bool can_be_regular(std::string str);
    void in(bool text, Table & table);
    void delet();
    void update();
    void select();
    void L_E(const Column_struct & , Poliz &);
    void L_M(const Column_struct & , Poliz &);
    void L_F(const Column_struct & , Poliz &);
    void B_E(const Column_struct & , Poliz &);
    void B_M(const Column_struct & , Poliz &);
    void B_F(const Column_struct & , Poliz &);
public:
    Parser(const char* str_);
    std::string parse();
};