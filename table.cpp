#include "table.h"
//#include "re.h"

int Poliz::count = 0;

void Item::apply(std::vector<long> & stack, Record & record, std::string & string, std::string & string_2, bool & f) {
    long a, b;
    a = stack.back();
    stack.pop_back();
    switch (type) {
        case A:
            b = stack.back();
            stack.pop_back();
            stack.push_back(b + a);
            break;
        case S:
            b = stack.back();
            stack.pop_back();
            stack.push_back(b - a);
            break;
        case M:
            b = stack.back();
            stack.pop_back();
            stack.push_back(b * a);
            break;
        case D:
            b = stack.back();
            stack.pop_back();
            stack.push_back(b / a);
            break;
        case O:
            b = stack.back();
            stack.pop_back();
            stack.push_back(b % a);
            break;
        case NO:
            b = 1;
            if(a)
                b = 0;
            stack.push_back(b);
            break;
        case AND:
            b = stack.back();
            stack.pop_back();
            stack.push_back(a && b);
            break;
        case OR:
            b = stack.back();
            stack.pop_back();
            stack.push_back(a || b);
            break;
    }
}

void NumItem::apply(std::vector<long> & stack, Record & record, std::string & string, std::string & string_2, bool & f) {
    if(type == N)
        stack.push_back(value);
    else if(type == I)
        stack.push_back(atol(record[value].data()));
    else if(f) 
        string_2 = record[value];
    else {
        f = true;
        string = record[value];
    }
}

void TextItem::apply(std::vector<long> & stack, Record & record, std::string & string, std::string & string_2, bool & f) {
    if(f)
        string_2 = str;
    else {
        f = true;
        string = str;
    }
}

void BoolItem::apply(std::vector<long> & stack, Record & record, std::string & string, std::string & string_2, bool & f) {
    bool eq, l;
    if(f) {
        eq = (string == string_2);
        l = (string < string_2);
    } else {
        long b = stack.back();
        stack.pop_back();
        long a = stack.back();
        stack.pop_back();
        eq = (a == b);
        l = (a < b);
    }
    bool res = (eq && equal || l && less) ^ reverse;
    if(res)
        stack.push_back(1);
    else
        stack.push_back(0);
    f = false;
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

Poliz::Poliz(Poliz & items) {
    count += 1;
    _size = items._size;
    ptr = new Item*[_size];
    for (int i = 0; i < _size; i++)
        ptr[i] = items[i];
}

Poliz::~Poliz() {
    if(_size) {
        if(count == 1) {
            for (int i = 0; i < _size; i++)
                delete ptr[i];
        } count -= 1;
        delete [] ptr;
    }
}

Item* Poliz::operator[](int index) const {
    return ptr[index];
}

Poliz & Poliz::operator=(Poliz & items) {
    count += 1;
    _size = items._size;
    ptr = new Item*[_size];
    for (int i = 0; i < _size; i++)
        ptr[i] = items[i];
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
        count += 1;
        _size = 1;
        ptr = ptr_2;
        *ptr = item;
    }
}

int Poliz::get_size() const {
    return _size;
}

long Poliz::exec(Record & record) const{
    std::vector<long> stack;
    std::string string, string_2;
    bool f = false;
    for(int i = 0; i < _size; i++)
        ptr[i]->apply(stack, record, string, string_2, f);
    long a = stack.back();
    return a;
}

int Column_struct::field_id(std::string str) const {
    int id = 0;
    int i = 0;
    while(!id && (i < _size)) {
        if(str == columns[i].name)
            id = i + 1;
        i++;
    }
    //std::cout << id << " ";
    return id;
}

std::string Table::next_word(const char* & str) {
    int c = *str++;
    std::string word;
    enum lex_t {H, STR, W, OK} state = H;
    while(state != OK) {
        switch (state) {
        case H:
            if(!std::isspace(c))
                if(c == '\'')
                    state = STR;
                else {
                    state = W;
                    word += c;
                }
            break;
        
        case STR:
            if(c == '\'')
                state = OK;
            else
                word += c;
            break;
        case W:
            if(std::isspace(c)) 
                state = OK;
            else
                word += c;
        }
        if(state != OK)
            c = *str++;
    } return word;
}

Table::Table(std::string _file) {
    name = _file;
    const char* word;
    file.open(_file, std::ios::in);
    if(!file.is_open())
        throw std::string("Server Error: can't open the table");
    std::getline(file, header);
    header += '\n';
    const char* str_ = header.data();
    size = atoi(next_word(str_).data());
    columns = Column_struct(size);
    for (int i = 0; i < size; i++) {
        columns[i].name = next_word(str_);
        word = next_word(str_).data();
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
    file.close();
}

const Column_struct Table::get_struct() {
    return columns;
}

int Table::get_size() {
    return size;
}

bool Table::where(Record & rec) {
    bool ret;
    int i = 0;
    const char* str;
    long a;
    switch (where_type) {
    case 0:
        ret = true;
        break;
    case 1:
        ret = where_re(where_str.data(), (rec[where_id] + '\n').data());
        break;
    case 2:
        ret = false;
        where_str += '\n';
        str = where_str.data();
        while(!ret && (i < where_count)) {
            ret = (rec[where_id] == next_word(str)); 
            i++;
        }
        break;
    case 3:
        ret = false;
        where_str += '\n';
        str = where_str.data();
        a = where_poliz.exec(rec);
        while(!ret && (i < where_count)) {
            ret = (a == atol(next_word(str).data())); 
            i++;
        }
        break;
    case 4:
        ret = where_poliz.exec(rec);
        break;
    }
    return ret ^ where_not;
}

void Table::get_record(Record & record, const char* str) {
    for(int i = 0; i < size; i++)
        record[i] = next_word(str);
}

void Table::delet() {
    Record record(size);
    std::ofstream fout;
    fout.open(name, std::ios::out);
    fout << header;
    std::string str;
    std::getline(file, str);
    str += '\n';
    while(!file.eof()) {
        get_record(record, str.data());
        if(!where(record))
            fout << str + '\n';
        std::getline(file, str);
        str += '\n';
    }
    fout.close();
}

void Table::update(int type, std::string & str_field, int first_id, int second_id, const Poliz & poliz) {
    Record record(size);
    std::ofstream fout;
    fout.open(name, std::ios::out);
    fout << header;
    std::string str;
    std::getline(file, str);
    str += '\n';
    while(!file.eof()) {
        get_record(record, str.data());
        if(where(record)) {
            for(int i = 0; i < size; i++) {
                if(i == first_id) {
                    if(type == 0)
                        fout << str_field + ' ';
                    else if(type == 1)
                        fout << '\'' + record[second_id] + "\' ";
                    else
                        fout << std::to_string(poliz.exec(record)) + ' ';
                } 
                else if(columns[i].type)
                    fout << '\'' << record[i] << "\' ";
                else 
                    fout << record[i] + ' ';
            }
            fout << '\n';
        } else
            fout << str;
        std::getline(file, str);
        str += '\n';
    }
    fout.close();
}

void Table::select(std::string & answer, std::vector<int> & id_s) {
    Record record(size);
    std::string str;
    std::getline(file, str);
    str += '\n';
    for(int i = 0; i < id_s.size(); i++) {
        answer += columns[id_s[i]].name + ' ';
    }
    answer += "\n-----------------\n";
    while(!file.eof()) {
        get_record(record, str.data());
        if(where(record)) {
            for(int i = 0; i < id_s.size(); i++) {
                answer += record[id_s[i]] + ' ';
            }
            answer += '\n';
        } std::getline(file, str);
        str += '\n';
    }
}

bool Table::where_re(const char* sample, const char* value) {
    const char* last_s;
    const char* last_v;
    bool like = false;
    bool percent = false;
    bool no;
    bool here;
    char c = *sample++;
    char v;
    char a;
    enum re_t {BEG, PER, IN, OK} state = BEG;
    while(state != OK) {
        switch (state) {
        case BEG:
            if(c == '%') {
                state = PER;
                percent = true;
                while(*sample == '%')
                    sample++;
                last_s = sample;
                last_v = value;
                c = *sample++;
            } else if(c == '\n') {
                if(*value == '\n') {
                    state = OK;
                    like = true; 
                } else if(percent) {
                    sample = last_s;
                    value = last_v + 1;
                    c = *sample++;
                    state = PER;
                } else
                    state = OK;
            } else if(c == '_') {
                if((v = *value++) == '\n')
                    state = OK;
                else
                    c = *sample++; 
            } else if(c == '[') {
                state = IN;
                c = *sample++;
            } else {
                v = *value++;
                if(v == c)
                    c = *sample++;
                else if(v == '\n')
                    state = OK;
                else if(percent) {
                    sample = last_s;
                    value = last_v + 1;
                    last_v += 1;
                    c = *sample++;
                    state = PER;
                } else
                    state = OK;
            }
            break;

        case PER:
            if(c == '\n') {
                like = true;
                state = OK;
            } else if(c == '_') {
                if((v = *value++) == '\n')
                    state = OK;
                else {
                    c = *sample++;
                    state = BEG;
                }
            } else if(c == '[') {
                state = IN;
                c = *sample++;
            } else {
                v = *value++;
                if(v == c) {
                    c = *sample++;
                    state = BEG;
                } else if(v == '\n')
                    state = OK;
                else
                    last_v += 1;
            }
            break;

        case IN:
            no = false;
            if(c == '^') {
                no = true;
                c = *sample++;
            } 
            a = c;
            c = *sample++;
            v = *value++;
            if(c == '-') {
                c = *sample++;
                here = ((v >= a) && (v <= c));
                c = *sample++;
            } else {
                here = (v == a);
                while(c != ']') {
                    here = here || (v == c);
                    c = *sample++;
                }
            }
            c = *sample++;
            if(here | no)
                state = BEG;
            else if(percent) {
                sample = last_s;
                value = last_v + 1;
                last_v += 1;
                c = *sample++;
                state = PER;
            } else
                state = OK;
            if(v == '\n')
                state = OK;
            break;
        }
    }
    return like;
}