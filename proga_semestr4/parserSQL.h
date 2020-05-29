#include <iostream>
#include <unistd.h>
#include "tableSQL.h"

enum lex_t
    {ID, NUM, OTHER, END, STR};

class Parser
{    
    int should_be_closed;
    int open_count;
    bool can_be_num;
    char c;
    enum lex_t cur_lex_type;
    std::string cur_lex_text;
    const char* str;
    bool err;
    void next ();
    std::string answer;
    void create ();
    std::string field ();
    void drop ();
    void insert ();
    std::string value (const Column &, int);
    void where (Table & );
    void in (bool text, Table & table);
    void remove ();
    void update ();
    void select ();
    void logic_add (const Column_struct & , Poliz &);
    void logic_mult (const Column_struct & , Poliz &);
    void logic_expr (const Column_struct & , Poliz &);
    void num_add (const Column_struct & , Poliz &);
    void num_mult (const Column_struct & , Poliz &);
    void num_expr (const Column_struct & , Poliz &);

public:
    Parser (const char* str_);
    std::string parse ();
};
