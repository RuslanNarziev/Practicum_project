#include "parser.h"

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
        else if(cur_lex_text == "DELETE")
            delet();
        else if(cur_lex_text == "UPDATE")
            update();
        else if(cur_lex_text == "SELECT")
            select();
        else 
            answer = "Incorrect call\n";
    }
    catch(std::string st) {
        answer = st;
    }
    return answer;
}

void Parser::create() {
    std::string name;
    std::string header;
    std::vector<std::string> names;
    next();
    if(cur_lex_text != "TABLE")
        throw std::string("Incorrect Create-call: expected \"TABLE\" after \"CREATE\"");
    next();
    if(cur_lex_type != ID)
        throw std::string("Incorrect Create-call: invalid table's name");
    name = cur_lex_text;
    next();
    if(cur_lex_text != "(")
        throw std::string("Incorrect Create-call: expected \'(\'");
    next();
    int size = 1;
    try {
        header += field(names);
    }
    catch(std::string ans) {
        throw ans + std::to_string(size);
    }
    while((cur_lex_text == ",")) {
        next();
        size++;
        try {
            header += field(names);
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
    if(fopen(name.data(), "r"))
        throw std::string("Incorrect Create-call: such table exist");
    FILE* fd_w = fopen(name.data(), "w");
    if(fd_w != NULL) {
        header = std::to_string(size) + ' ' + header + '\n';
        fputs(header.data(), fd_w);
        fclose(fd_w);
        answer =  "Table " + name + " is created";
    } else
        answer = "Database error: Table " + name + " is not created";
}

std::string Parser::field(std::vector<std::string> & names) {
    std::string ret;
    if(cur_lex_type != ID)
        throw std::string("Incorrect Create-call: wrong field #");
    for(int i = 0; i < names.size(); i++)
        if(names[i] == cur_lex_text)
            throw std::string("Incorrect Create-call: name \"" + cur_lex_text + "\" met twice - wrond field#");
    names.push_back(cur_lex_text);
    ret += cur_lex_text + ' ';
    next();
    if(cur_lex_text == "LONG")
        ret += "L ";
    else if(cur_lex_text == "TEXT") {
        next();
        if(cur_lex_text != "(")
            throw std::string("Incorrect Create-call: wrong field #");
        next();
        if(cur_lex_type != NUM)
            throw std::string("Incorrect Create-call: wrong field #");
        if(!atol(cur_lex_text.data()))
            throw std::string("Incorrect Create-call: wrong field #");
        else
            ret += "T" + std::to_string(atol(cur_lex_text.data())) + ' ';
        next();
        if(cur_lex_text != ")")
            throw std::string("Incorrect Create-call: wrong field #");
    } else throw std::string("Incorrect Create-call: wrong field #");
    next();
    return ret;
}

void Parser::drop() {
    std::string name;
    next();
    if(cur_lex_text != "TABLE")
        throw std::string("Incorrect Drop-call: expected \"TABLE\"");
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
        throw std::string("Incorrect Insert-call: expected \"INTO\"");    
    next();
    if(cur_lex_type != ID)
        throw std::string("Incorrect Insert-call: table's name should be identifier"); 
    name = cur_lex_text;
    next();
    if(cur_lex_text != "(")
        throw std::string("Incorrect Insert-call: expected \'(\'");  
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
            throw std::string("Incorrect Insert-call: expected \',\'");  
        next();
        val += value(columns[i], i + 1);
        i++;
    }

    if(cur_lex_text != ")")
        throw std::string("Incorrect Insert-call: expected \')\'");      
    next();
    if(cur_lex_type != END)
        throw std::string("Incorrect Insert-call");  

    FILE* fd = fopen(name.data(), "a");
    fputs(val.data(), fd);
    fflush(fd);
    answer = "Correct insert";
}

std::string Parser::value(const Column & column, int i) {
    std::string ret;
    if(cur_lex_type == NUM)
        if(column.type)
            throw std::string("Incorrect: argument #") + std::to_string(i) + " should be text";
        else
            ret = std::to_string(atol(cur_lex_text.data())) + ' '; 
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
    int id;
    bool _not = false;
    Poliz poliz;
    std::string str;
    const Column_struct columns = table.get_struct();
    next();
    if(cur_lex_text == "ALL") {
        table.where_type = 0;
        table.where_not = false;
        return;
    }

    if((cur_lex_type == STR)) {
        bool str_in = false;
        table.where_type = 0;
        str = cur_lex_text;
        next();
        if(cur_lex_text == "NOT") {
            next();
            _not = true;
        }
        if(cur_lex_text != "IN")
            throw std::string("Incorrect where-clause: expected \"IN\"");
        next();
        if(cur_lex_text != "(")
            throw std::string("Incorrect where-clause: expected ( after \"IN\"");
        next();
        str_in = (cur_lex_text == str);
        next();
        while(cur_lex_text == ",") {
            next();
            if(cur_lex_type != STR)
                throw std::string("Incorrect where-clause: expected string");
            str_in = (cur_lex_text == str) || str_in;
            next();
        }
        if(cur_lex_text != ")")
            throw std::string("Incorrect where-clause: expected \')\'");
        next();
        if(cur_lex_type != END)
            throw std::string("Incorrect where-clause");
        table.where_not = _not ^ !str_in;
        return;
    }

    id = columns.field_id(cur_lex_text);
    if(id && columns[id-1].type) {
        next();
        if(cur_lex_text == "NOT") {
            next();
            _not = true;
        }
        table.where_not = _not;
        table.where_id = id - 1;
        if(cur_lex_text == "LIKE") {
            next();
            if((cur_lex_type != STR) || err)
                throw std::string("Incorrect where-clause: expected string after\"LIKE\"");
            if(!can_be_regular(cur_lex_text))
                throw std::string("Incorrect where-clause: this string is invalid");
            table.where_str = cur_lex_text + '\n';
            next();
            if(cur_lex_type != END)
                throw std::string("Incorrect where-clause");
            table.where_type = 1;
            return;
        } else if(cur_lex_text == "IN") {
            table.where_type = 2;
            next();
            in(true, table);
            return;
        } else
            throw std::string("Incorrect where-clause: expected \"LIKE\" or \"IN\"");
    }

    open_count = 0;
    should_be_closed = 0;
    can_be_num = true;    
    if((cur_lex_type == NUM) || (id = columns.field_id(cur_lex_text)) && !columns[id-1].type) {
        L_E(columns, poliz);
    } else {
        B_E(columns, poliz);
    }
    if(can_be_num) {
        table.where_type = 3;
        if(cur_lex_text == "NOT") {
            next();
            _not = true;
        }
        if(cur_lex_text != "IN")
            throw std::string("Incorrect where-clause: expected IN");
        next();
        table.where_type = 3;
        table.where_poliz = poliz;
        table.where_not = _not;
        in(false, table);
        return;
    } else {
        if(cur_lex_type != END)
            throw std::string("Incorrect where-clause");
        table.where_not = false;
        table.where_poliz = poliz;
        table.where_type = 4;
        return;   
    }
}

void Parser::in(bool text, Table & table) {
    if(cur_lex_text != "(")
        throw std::string("Incorrect where-clause: expected (");
    next();
    if((text && (cur_lex_type != STR)) || (!text && (cur_lex_type != NUM)))
        throw std::string("Incorrect where-clause: invalid type");
    if(text)
        table.where_str = '\'' + cur_lex_text + "\' ";
    else
        table.where_str = cur_lex_text + ' ';
    table.where_count = 1;
    next();
    while(cur_lex_text == ",") {
        next();
        if((text && (cur_lex_type != STR)) || (!text && (cur_lex_type != NUM)))
            throw std::string("Incorrect where-clause: invalid type");
        if(text)
            table.where_str = '\'' + cur_lex_text + "\' ";
        else
            table.where_str += cur_lex_text + ' ';
        table.where_count += 1;
        next();
    }
    if(cur_lex_text != ")")
        throw std::string("Incorrect where-clause: expected )");
    next();
    if(cur_lex_type != END)
        throw std::string("Incorrect where-clause");
    return;
}

void Parser::B_E(const Column_struct & columns, Poliz & poliz) {
    B_M(columns, poliz);
    if(should_be_closed) {
        --should_be_closed;
        return;
    }
    Item* item;
    while (cur_lex_text == "OR") {
        next();
        open_count = 0;
        B_M(columns, poliz);
        item = new Item;
        item->type = OR;
        poliz.push(item);
    }
}

void Parser::B_M(const Column_struct & columns, Poliz & poliz) {
    B_F(columns, poliz);
    if(should_be_closed)
        return;
    Item* item;
    while (cur_lex_text == "AND") {
        next();
        open_count = 0;
        B_F(columns, poliz);
        item = new Item;
        item->type = AND;
        poliz.push(item);
    }
}

void Parser::B_F(const Column_struct & columns, Poliz & poliz) {
    bool text;
    bool no = false;
    int id;
    while(cur_lex_text == "NOT") {
        no = !no;
        next();
    }
    if(cur_lex_text != "(")
        throw std::string("Incorrect where-clause: expected ( after \"IN\"");
    next();
    open_count += 1;
    int count = open_count;
    if((cur_lex_type == NUM) || (cur_lex_type == STR) || (id = columns.field_id(cur_lex_text))) {
        int count = open_count;
        if(cur_lex_type == STR) {
            TextItem* item = new TextItem;
            item->type = ST;
            item->str = cur_lex_text;
            poliz.push(item);
            next();
            text = true;
        } else if((cur_lex_type == ID) && columns[id-1].type) {
            text = true;
            NumItem* item = new NumItem;
            item->type = T;
            item->value = id - 1;
            poliz.push(item);
            next();
        } else {
            text = false;
            int temp = open_count;
            L_E(columns, poliz);
            should_be_closed = open_count;
            if(temp == open_count)
                if(can_be_num)
                    return;
                else
                    throw std::string("Incorrect where-clause: expected comparison operation");
            if(temp < open_count)
                throw std::string("Incorrect where-clause: unexpected symbol \')\'");
        }
        open_count = 0;
        can_be_num = false;
        BoolItem* item_ = new BoolItem;
        item_->type = B;
        if(cur_lex_text == "!") {
            next();
            if(cur_lex_text != "=")
                throw std::string("Incorrect where_clause: expected '=' after '!'");
            item_->reverse = true;
            item_->less = false;
            item_->equal = true;
            next();
        } else if(cur_lex_text == "=") {
            item_->reverse = false;
            item_->less = false;
            item_->equal = true;
            next();
        } else if(cur_lex_text == ">") {
            next();
            item_->reverse = true;
            item_->less = true;
            if(cur_lex_text == "=") {
                next();
                item_->equal = false;
            } else
                item_->equal = true;
        } else if(cur_lex_text == "<") {
            next();
            item_->reverse = false;
            item_->less = true;
            if(cur_lex_text == "=") {
                next();
                item_->equal = true;
            } else
                item_->equal = false;
        } else
            throw std::string();
        
        if(text) {
            if(cur_lex_type == STR) {
                TextItem* item = new TextItem;
                item->type = ST;
                item->str = cur_lex_text;
                poliz.push(item);
                next();
            } else {
                int id = columns.field_id(cur_lex_text);
                if(!id || !columns[id-1].type)
                    throw std::string("Incorrect where-clause: expected text-field");
                NumItem* item = new NumItem;
                item->type = I;
                item->value = id - 1;
                poliz.push(item);
                next();
            }
        } else {
            L_E(columns, poliz);
        }
        poliz.push(item_);
    } 
    else {
        B_E(columns, poliz);
    }
    if(should_be_closed)
        return;
    --open_count;
    if(cur_lex_text != ")")
        throw std::string("Incorrect where-clause: expected )");
    if(no) {
        Item* item = new Item;
        item->type = NO;
        poliz.push(item);
    }
    next();
}

void Parser::L_E(const Column_struct & columns, Poliz & poliz) {
    int count = open_count;
    L_M(columns, poliz);
    if(open_count != count)
        return;
    bool pl;
    Item* item;
    while ((pl = (cur_lex_text == "+")) || (cur_lex_text == "-")) {
        next();
        int temp = open_count;
        open_count = 0;
        L_M(columns, poliz);
        open_count = temp;
        item = new Item;
        if(pl)
            item->type = A;
        else
            item->type = S;
        poliz.push(item);
    }
    if(count && (cur_lex_text == ")")) {
        open_count++;
        next();
    }
}

void Parser::L_M(const Column_struct & columns, Poliz & poliz) {
    int count = open_count;
    if(open_count) {
        open_count--;
        L_E(columns, poliz);
        if(cur_lex_text == ")") {
            open_count++;
            next();
        } if(open_count != count)
            return;
    } else {
        L_F(columns, poliz);

    }
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
        int temp = open_count;
        open_count = 0;
        L_F(columns, poliz);
        open_count = temp;
        poliz.push(item);
    }
}

void Parser::L_F(const Column_struct & columns, Poliz & poliz) {
    if (cur_lex_text == "(") {
        next();
        int temp = open_count;
        open_count = 0;
        L_E(columns, poliz);
        open_count = temp;
        if (cur_lex_text != ")")
            throw std::string("Incorrect where-clause: expected )");
        next();
    } else if (cur_lex_type == NUM) {
        NumItem* item = new NumItem;
        item->type = N;
        item->value = atol(cur_lex_text.data());
        poliz.push(item);
        next();
    } else {
        int id = columns.field_id(cur_lex_text);
        if(!id || columns[id-1].type)
            throw std::string("Incorrect where-clause: invalid field ") + cur_lex_text;
        NumItem* item = new NumItem;
        item->type = I;
        item->value = id - 1;
        poliz.push(item);
        next();
    }
}

void Parser::delet() {
    std::string name = cur_lex_text;
    next();
    if(cur_lex_text != "FROM")
        throw std::string("Incorrect Delete-call: expected \"FROM\" after \"DELETE\"");
    next();
    if(cur_lex_type != ID)
         throw std::string("Incorrect Delete-call: table's name should be identifier");   
    name = cur_lex_text;
    next();
    if(cur_lex_text != "WHERE")
        throw std::string("Incorrect Delete-call: expected \"WHERE\"");
    Table table(name);
    where(table);
    table.delet();
    answer = "Correct Delete-call";
}

void Parser::update() {
    std::string name;
    next();
    if(cur_lex_type != ID)
         throw std::string("Incorrect Update-call: table's name should be identifier");   
    name = cur_lex_text;
    next();
    if(cur_lex_text != "SET")
        throw std::string("Incorrect Update-call: expected \"SET\"");
    next();
    Table table(name);
    const Column_struct columns = table.get_struct();
    int id = columns.field_id(cur_lex_text);
    if(!id)
        throw std::string("Incorrect Update-call: no such field in table : ") + cur_lex_text;
    bool text = columns[id-1].type;
    next();
    if(cur_lex_text != "=")
        throw std::string("Incorrect Update-call: expected \'=\' ");
    next();

    int type;
    int second_id;
    Poliz poliz;
    std::string str;
    if(text) {
        if(cur_lex_type == STR) {
            str = cur_lex_text;
            type = 0;
        }
        else {
            second_id = columns.field_id(cur_lex_text);
            if(!second_id)
                throw std::string("Incorrect Delete-call: no such field in table : ") + cur_lex_text;
            second_id -= 1;
            if(!columns[second_id].type)
                throw std::string("Incorrect Delete-call: expected text-field");
            type = 1;
        }
        next();
    } else {
        L_E(columns, poliz);
        type = 2;
    }

    if(cur_lex_text != "WHERE")
        throw std::string("Incorrect Delete-call: expected \"WHERE\"");
    where(table);
    table.update(type, str, id - 1, second_id, poliz);
    answer = "Correct Update-call";
}

void Parser::select() {
    std::string name;
    next();
    std::vector<std::string> fields;
    bool star = false;
    if(cur_lex_text == "*") {
        next();
        star = true;
    } else {
        if(cur_lex_type != ID)
            throw std::string("Incorrect Select-call: expected field's identifier");
        fields.push_back(cur_lex_text);
        next();
        while(cur_lex_text == ",") {
            next();
            if(cur_lex_type != ID)
                throw std::string("Incorrect Select-call: expected field's identifier");
            fields.push_back(cur_lex_text);
            next();
        }
    }
    if(cur_lex_text != "FROM")
        throw std::string("Incorrect Select-call: expected \"FROM\"");
    next();
    if(cur_lex_type != ID)
         throw std::string("Incorrect Select-call: table's name should be identifier");  
    name = cur_lex_text;
    next();
    if(cur_lex_text != "WHERE")
        throw std::string("Incorrect Select-call: expected \"WHERE\"");
    Table table(name);
    const Column_struct columns = table.get_struct();
    std::vector<int> id_s;
    if(star) {
        for(int i = 0; i < table.get_size(); i++)
            id_s.push_back(i);
    } else {
        for(int i = 0; i < fields.size(); i++) {
            int id = columns.field_id(fields[i]);
            if(!id)
                throw std::string("Incorrect Select-call: no such field on table: ") + fields[i];
            id_s.push_back(id-1);
        }
    }
    where(table);
    table.select(answer, id_s);
}

bool Parser::can_be_regular(std::string str) {
    str += '\n';
    const char* s = str.data();
    char c = *s++;
    while(c != '\n') {
        if(c == '[') {
            c = *s++;
            if(c == '\\')
                c = *s++;
            if(c == '\n')
                return false;
            c = *s++;
            while(c != ']') {
                if(c == '\\')
                    c = *s++;
                if(c == '\n')
                    return false;
                c = *s++;
            }
        }
        c = *s++;
    }
    return true;
}