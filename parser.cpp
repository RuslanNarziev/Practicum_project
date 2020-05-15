#include <iostream>
#include "table.h"
#include <unistd.h>

enum lex_t {ID, NUM, OTHER, END, STR};

class Parser {    
    char c;
    enum lex_t cur_lex_type;
    std::string cur_lex_text;
    const char* str;
    bool err;
    void next();
    std::string answer;
    void create();
    std::string field();
    void drop();
    void insert();
    std::string value(const Column &, int);
    void where(Table & );
    void L_E(const Column_struct & , Poliz &);
    void L_M(const Column_struct & , Poliz &);
    void L_F(const Column_struct & , Poliz &);
public:
    Parser(const char* str_);
    std::string parse();
};

Parser::Parser(const char* str_) {
    cur_lex_type = OTHER;
    str = str_ + 1; 
    c = str_[0];
    err = false;
}

void Parser::next() {
    enum state_h {ID, NUM, OK, H, OTH, ST} state = H;
    cur_lex_text.clear();
    if(cur_lex_type == END) {
        state = OK;
        err = true;
    }
    while(state != OK) {
        switch (state) {
        case H:
            if (std::isspace(c) && (c != '\n'))
                c = *str++;
            else if(c == '\'') {
                state = ST;
                cur_lex_type = STR;
            } 
            else { 
                if(c == '\n') {
                    state = OK;
                    cur_lex_type = END;
                } else if(std::isalpha(c) || (c == '_')) {
                    state = ID;
                    cur_lex_type = ::ID;
                } else if(std::isdigit(c)) {
                    state = NUM;
                    cur_lex_type = ::NUM; 
                } else {
                    state = OTH;
                    cur_lex_type = OTHER;
                }
                cur_lex_text += c;
            }
            break;
        case ID:
            c = *str++;
            if(std::isalpha(c) || (c == '_') || std::isdigit(c))
                cur_lex_text += c;
            else
                state = OK;
            break;
        case NUM:
            c = *str++;
            if(std::isdigit(c))
                cur_lex_text += c;
            else
                state = OK;
            break;
        case ST:
            c = *str++;
            if(c == '\'') {
                state = OK;
                c = *str++;
            } else if(c == '\n') {
                state = OK;
                err = true;
            } else if(c == '\\') {
                c = *str++;
                if((c == '\\') || (c == '\''))
                    cur_lex_text += c;
                else
                    err = true;
            } else
                cur_lex_text += c;
            break;     
        case OTH:
            c = *str++;
            state = OK;
            break;
        }
    }
}

std::string Parser::parse() {
    next();
    try {
        if(cur_lex_text == "CREATE")
            create();
        else if(cur_lex_text == "DROP")
            drop();
        else if(cur_lex_text == "INSERT")
            insert();
        else if(cur_lex_text == "WHERE") {
            Table T("ASD");
            where(T);
        } else {
            answer = "Incorrect call\n";
        }
    }
    catch(std::string st) {
        answer = st;
    }
    return answer;
}

void Parser::create() {
    std::string name;
    std::string header;
    next();
    if(cur_lex_text != "TABLE")
        throw std::string("Incorrect Create-call");
    next();
    if(cur_lex_type != ID)
        throw std::string("Incorrect Create-call");
    name = cur_lex_text;
    next();
    if(cur_lex_text != "(")
        throw std::string("Incorrect Create-call");
    next();
    int size = 1;
    try {
        header += field();
    }
    catch(std::string ans) {
        throw ans + std::to_string(size);
    }
    while((cur_lex_text == ",")) {
        next();
        size++;
        try {
            header += field();
        }
        catch(std::string ans) {
            throw ans + std::to_string(size);
        }
    }
    if(cur_lex_text != ")")
        throw std::string("Incorrect Create-call");
    next(); 
    if(cur_lex_type != END)
        throw std::string("Incorrect Create-call");
    
    FILE* fd_w = fopen(name.data(), "w");
    if(fd_w != NULL) {
        header = std::to_string(size) + ' ' + header + '\n';
        fputs(header.data(), fd_w);
        fclose(fd_w);
        answer =  "Table " + name + " is created";
    } else
        answer = "Database error: Table " + name + " is not created";
}

std::string Parser::field() {
    std::string ret;
    if(cur_lex_type != ID)
        throw std::string("Incorrect : wrong field #");
    ret += cur_lex_text + ' ';
    next();
    if(cur_lex_text == "LONG")
        ret += "L ";
    else if(cur_lex_text == "TEXT") {
        next();
        if(cur_lex_text != "(")
            throw std::string("Incorrect : wrong field #");
        next();
        if(cur_lex_type != NUM)
            throw std::string("Incorrect : wrong field #");
        if(!atol(cur_lex_text.data()))
            throw std::string("Incorrect : wrong field #");
        else
            ret += "T" + std::to_string(atol(cur_lex_text.data())) + ' ';
        next();
        if(cur_lex_text != ")")
            throw std::string("Incorrect : wrong field #");
    } else throw std::string("Incorrect : wrong field #");
    next();
    return ret;
}

void Parser::drop() {
    std::string name = cur_lex_text;
    next();
    if(cur_lex_text != "TABLE")
        throw std::string("Incorrect Drop-call");
    next();
    if(cur_lex_type != ID)
         throw std::string("Incorrect Drop-call: table's name should be identifier");   
    name = cur_lex_text;
    next();
    if(cur_lex_type != END)
        throw std::string("Incorrect Drop-call");
    if (!unlink(name.data()))
        answer = "Table " + name + " is deleted";
    else {
	    switch(errno) {
	    case EACCES : 
            answer = "Incorrect : access denied";
	    case ENOENT : 
            answer = "Incorrect : no such table in database";
        }
	}
}

void Parser::insert() {
    std::string name;
    std::string val;
    next();
    if(cur_lex_text != "INTO")
        throw std::string("Incorrect Insert-call");    
    next();
    if(cur_lex_type != ID)
        throw std::string("Incorrect Insert-call: table's name should be identifier"); 
    name = cur_lex_text;
    next();
    if(cur_lex_text != "(")
        throw std::string("Incorrect Insert-call");  
    next(); 

    int i = 1;
    int size;
    Column_struct columns;
    Table T(name);
    size = T.get_size();
    columns = T.get_struct();

    val = value(columns[0], 1);
    while(i < size) {
        if(cur_lex_text != ",")
            throw std::string("Incorrect Insert-call");  
        next();
        val += value(columns[i], i + 1);
        i++;
    }

    if(cur_lex_text != ")")
        throw std::string("Incorrect Insert-call");      
    next();
    if(cur_lex_type != END)
        throw std::string("Incorrect Insert-call");  

    FILE* fd = fopen(name.data(), "a");
    val += "\n";
    fputs(val.data(), fd);
    answer = "Correct insert";
}

std::string Parser::value(const Column & column, int i) {
    std::string ret;
    if(cur_lex_type == NUM)
        if(column.type)
            throw std::string("Incorrect: argument #") + std::to_string(i) + " should be text";
        else
            ret = cur_lex_text + ' '; 
    else if((cur_lex_type == STR) && !err)
        if(column.type) {
            ret = '\'' + cur_lex_text + '\'';
            if(column.size < cur_lex_text.size())
                throw std::string("Incorrect: argument #") + std::to_string(i) + " should be less";
        } else
            throw std::string("Incorrect: argument #") + std::to_string(i) + " should be number";
    else
        throw std::string("Incorrect: wrong field #") + std::to_string(i);
    next();
    return ret;
}

void Parser::where(Table & table) {
    const char* str_2 = str;
    char c_2 = c;
    std::string ans;
    const Column_struct columns = table.get_struct();
    next();
    if(cur_lex_text == "ALL") {
        table.where_type = 0;
        answer = "ALL";
        return;
    }
    try {
        int id;
        bool _not = false;
        if(cur_lex_type != ID)
            throw std::string("Incorrect where-clause");
        if(!(id = columns.field_id(cur_lex_text)))
            throw std::string("Incorrect where-clause: no such field in table");
        if(!columns[id-1].type)
            throw std::string("Incorrect where-clause: field ") + std::to_string(id) + " is not text";
        next();
        if(cur_lex_text == "NOT") {
            next();
            _not = true;
        }
        if(cur_lex_text != "LIKE")
            throw std::string("Incorrect where-clause");
        next();
        if((cur_lex_type != STR) || err)
            throw std::string("Incorrect where-clause");
        table.where_str = cur_lex_text;
        next();
        if(cur_lex_type != END)
            throw std::string("Incorrect where-clause");
        table.where_not = _not;
        table.where_type = 1;
        table.where_id = id - 1;
        answer = table.where_str + std::to_string(id);
        return;
    }
    catch(std::string st) {
        ans = st;
        str = str_2;
        c = c_2;
    }

    next();
    try {
        int id;
        bool _not = false;
        bool text = false;
        Poliz poliz;
        if((cur_lex_type == STR)) {
            id = 0;
            text = true;
        } else {
            id = columns.field_id(cur_lex_text);
            if(id && (columns[id-1].type))
               text = true; 
        }
        if(!text)
            L_E(columns, poliz);
        if(cur_lex_text == "NOT") {
            next();
            _not = true;
        }
        if(cur_lex_text != "IN")
            throw std::string("Incorrect where-clause: expected IN");
        next();
        if(cur_lex_text != "(")
            throw std::string("Incorrect where-clause: expected (");
        next();
        if((text && (cur_lex_type != STR)) || (!text && (cur_lex_type != NUM)))
            throw std::string("Incorrect where-clause: invalid type");
        table.where_str += cur_lex_text + ' ';
        next();
        while(cur_lex_text == ",") {
            next();
            if((text && (cur_lex_type != STR)) || (!text && (cur_lex_type != NUM)))
                throw std::string("Incorrect where-clause: invalid type");
            table.where_str += cur_lex_text + ' ';
            next();
        }
        if(cur_lex_text != ")")
            throw std::string("Incorrect where-clause: expected )");
        next();
        if(cur_lex_type != END)
            throw std::string("Incorrect where-clause");
        table.where_not = _not;
        if(text)
            if(id) {
                table.where_id = id - 1;
                table.where_type = 2;
                std::cout << table.where_str;
            }
            else 
                table.where_type = 0;
        else {
            table.where_type = 3;
            std::cout << table.where_str;
            table.where_poliz = poliz;
            answer = poliz.print();
            return;
        } 
    }
    catch(std::string st) {
        ans = st;
        str = str_2;
        c = c_2;
    }

    next();
    try {
        
    }

}

void Parser::L_E(const Column_struct & columns, Poliz & poliz) {
    L_M(columns, poliz);
    bool pl;
    Item* item;
    while ((pl = (cur_lex_text == "+")) || (cur_lex_text == "-")) {
        next();
        L_M(columns, poliz);
        item = new Item;
        if(pl)
            item->type = A;
        else
            item->type = S;
        poliz.push(item);
    }
}

void Parser::L_M(const Column_struct & columns, Poliz & poliz) {
    L_F(columns, poliz);
    Item* item;
    while ((cur_lex_text == "*") || (cur_lex_text == "/") || (cur_lex_text == "%")) {
        item = new Item;
        if(cur_lex_text == "*")
            item->type = M;
        else if(cur_lex_text == "/")
            item->type = D;
        else
            item->type = O;
        next();
        L_F(columns, poliz);
        poliz.push(item);
    }
}

void Parser::L_F(const Column_struct & columns, Poliz & poliz) {
    if (cur_lex_text == "(") {
        next();
        L_E(columns, poliz);
        if (cur_lex_text != ")")
            throw std::string("Incorrect where_clause: expected )");
        next();
    } else if (cur_lex_type == NUM) {
        NumItem* item = new NumItem;
        item->type = N;
        item->value = atoi(cur_lex_text.data());
        poliz.push(item);
        next();
    } else {
        int id = columns.field_id(cur_lex_text);
        if(!id || columns[id-1].type)
            throw std::string("Incorrect where_clause: invalid field ") + cur_lex_text;
        NumItem* item = new NumItem;
        item->type = I;
        item->value = id - 1;
        poliz.push(item);
        next();
    }
}



int main() {
    std::string str = "WHERE a + b * (a / b % 8) IN (7, 8)\n";
    Parser A(str.data());
    std::cout << A.parse();
    return 0;
}
