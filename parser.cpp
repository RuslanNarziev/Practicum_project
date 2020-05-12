#include <iostream>
#include "table.h"

enum lex_t {ID, NUM, OTHER, END};

class Parser {    
    char c;
    enum lex_t cur_lex_type;
    std::string cur_lex_text;
    const char* str;
    bool err;
    void next();
    char* answer;
    unsigned int answer_len;
    void create();
public:
    Parser(const char* str_);
    std::string parse();
};

Parser::Parser(const char* str_) {
    str = str_ + 1; 
    c = str_[0];
    err = false;
}

void Parser::next() {
    if(cur_lex_type != END) {
        while(std::isspace(c) && (c != '\n'))
            c = *str++;
        if(c == '\n')
            cur_lex_type = END;
        else {
            cur_lex_text.clear();
            bool id = false;
            bool num = false;
            if(std::isalpha(c) || (c == '_'))
                id = true;
            if(std::isdigit(c))
                num = true;
            while(!std::isspace(c)) {
                if(std::isalpha(c) || (c == '_'))
                    num = false;
                else if(!std::isdigit(c)) {
                    num = false;
                    id = false;
                }
                cur_lex_text += c;
                c = *str++;
            }
            if(id)
                cur_lex_type = ID;
            else if(num)
                cur_lex_type = NUM;
            else
                cur_lex_type = OTHER;           
        }
    
    } else 
        err = true;
}

std::string Parser::parse() {
    next();
    if(err) {
        answer = malloc(16);
        const char* answer_ = "Incorrect call\n";
        strcpy(answer, answer_);
        answer_len = 15;
    } else {
        switch (cur_lex_text) {
        case "CREATE":
            create();
            break;
        case "SELECT":
            Select();
            break;
        case "SELECT":
            Select();
            break;
        case "SELECT":
            Select();
            break;
        case "SELECT":
            Select();
            break;
        case "SELECT":
            Select();
            break;
        
        default:
            break;
        }
    }
    return answer;
}

int main() {
    std::string str = "A\n";
    Parser A(str.data());
    std::cout << A.parse() << A.parse() << A.parse();
    return 0;
}