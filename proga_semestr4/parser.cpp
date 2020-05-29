#include "parserSQL.h"

Parser::Parser (const char* str_)
{
    cur_lex_type = OTHER;
    str = str_ + 1;
    c = str_[0];
    err = false;
}

void Parser::next ()
{
    enum state_h {ID, NUM, OK, H, OTH, ST} state = H;
    cur_lex_text.clear ();
    if (cur_lex_type == END)
    {
        state = OK;
        err = true;
    }
    while(state != OK)
    {
        switch (state)
        {
            case H:
                if (std::isspace(c) && (c != '\n'))
                {
                    c = *str++;
                }
                else if (c == '\'')
                {
                    state = ST;
                    cur_lex_type = STR;
                }
                else
                {
                    if (c == '\n')
                    {
                        state = OK;
                        cur_lex_type = END;
                    } else if (std::isalpha(c) || (c == '_'))
                    {
                        state = ID;
                        cur_lex_type = ::ID;
                    } else if (std::isdigit(c))
                    {
                        state = NUM;
                        cur_lex_type = ::NUM;
                    } else
                    {
                        state = OTH;
                        cur_lex_type = OTHER;
                    }
                    cur_lex_text += c;
                }
                break;
            case ID:
                c = *str++;
                if (std::isalpha(c) || (c == '_') || std::isdigit(c))
                {
                    cur_lex_text += c;
                }
                else
                {
                    state = OK;
                }
                break;
            case NUM:
                c = *str++;
                if (std::isdigit(c))
                {
                    cur_lex_text += c;
                }
                else
                {
                    state = OK;
                }
                break;
            case ST:
                c = *str++;
                if (c == '\'')
                {
                    state = OK;
                    c = *str++;
                }
                else if (c == '\n')
                {
                    state = OK;
                    err = true;
                }
                else
                {
                    cur_lex_text += c;
                }
                break;     
            case OTH:
                c = *str++;
                state = OK;
                break;
        }
    }
}

std::string Parser::parse ()
{
    next ();
    try
    {
        if (cur_lex_text == "CREATE")
            create ();
        else if (cur_lex_text == "DROP")
            drop ();
        else if (cur_lex_text == "INSERT")
            insert ();
        else if (cur_lex_text == "DELETE")
            remove ();
        else if (cur_lex_text == "UPDATE")
            update ();
        else if (cur_lex_text == "SELECT")
            select ();
        else
        {
//            std::cout << str;
            answer = "Wrong command\n";
        }
    }
    catch (std::string st)
    {
        answer = st;
    }
    return answer;
}

void Parser::create ()
{
    std::string name;
    std::string header;
    next ();
    if (cur_lex_text != "TABLE")
        throw std::string ("Wrong CREATE command: expected \"TABLE\" after \"CREATE\"");
    next();
    if (cur_lex_type != ID)
        throw std::string ("Wrong CREATE command: invalid table's name");
    name = cur_lex_text;
    next ();
    if (cur_lex_text != "(")
        throw std::string ("Wrong CREATE command: expected \'(\'");
    next ();
    int size = 1;
    try
    {
        header += field ();
    }
    catch (std::string ans)
    {
        throw ans + std::to_string (size);
    }
    while ((cur_lex_text == ","))
    {
        next ();
        size++;
        try
        {
            header += field ();
        }
        catch (std::string ans)
        {
            throw ans + std::to_string (size);
        }
    }
    if (cur_lex_text != ")")
        throw std::string ("Wrong CREATE command");
    next (); 
    if (cur_lex_type != END)
        throw std::string ("Wrong CREATE command");
    
    FILE* fd_w = fopen (name.data (), "w");
    if (fd_w != NULL)
    {
        header = std::to_string (size) + ' ' + header + '\n';
        fputs (header.data (), fd_w);
        fclose (fd_w);
        answer =  "Table " + name + " is created";
    }
    else
    {
        answer = "Database error: Table " + name + " is not created";
    }
}

std::string Parser::field ()
{
    std::string ret;
    if (cur_lex_type != ID)
        throw std::string ("Error : wrong field #");
    ret += cur_lex_text + ' ';
    next ();
    if (cur_lex_text == "LONG")
        ret += "L ";
    else if (cur_lex_text == "TEXT")
    {
        next ();
        if (cur_lex_text != "(")
            throw std::string ("Error : wrong field #");
        next ();
        if (cur_lex_type != NUM)
            throw std::string ("Error : wrong field #");
        if (!atol (cur_lex_text.data()))
            throw std::string ("Error : wrong field #");
        else
            ret += "T" + std::to_string (atol (cur_lex_text.data())) + ' ';
        next ();
        if (cur_lex_text != ")")
            throw std::string ("Error : wrong field #");
    }
    else throw std::string ("Error : wrong field #");
    next ();
    return ret;
}

void Parser::drop ()
{
    std::string name;
    next ();
    if (cur_lex_text != "TABLE")
        throw std::string ("Wrong DROP command");
    next ();
    if (cur_lex_type != ID)
         throw std::string ("Wrong DROP command");   
    name = cur_lex_text;
    next ();
    if (cur_lex_type != END)
        throw std::string("Wrong DROP command");
    if (!unlink (name.data()))
        answer = "Table " + name + " is deleted";
    else
    {
	    switch(errno)
        {
	        case EACCES : 
                answer = "Error : access denied";
	        case ENOENT : 
                answer = "Error : no such table in database";
        }
	}
}

void Parser::insert ()
{
    std::string name;
    std::string val;
    next ();
    if (cur_lex_text != "INTO")
        throw std::string ("Wrong INSERT command");    
    next ();
    if (cur_lex_type != ID)
        throw std::string ("Wrong INSERT command"); 
    name = cur_lex_text;
    next ();
    if (cur_lex_text != "(")
        throw std::string ("Wrong INSERT command");  
    next (); 

    int i = 1;
    int size;
    Column_struct columns;
    Table T (name);
    size = T.get_size ();
    columns = T.get_struct ();

    val = value (columns[0], 1);
    while (i < size)
    {
        if (cur_lex_text != ",")
            throw std::string ("Wrong INSERT command");  
        next ();
        val += value (columns[i], i + 1);
        i++;
    }
    if (cur_lex_text != ")")
        throw std::string ("Wrong INSERT command");      
    next ();
    if (cur_lex_type != END)
        throw std::string ("Wrong INSERT command");  
    FILE* fd = fopen (name.data(), "a");
    val += "\n";
    fputs (val.data(), fd);
    fflush (fd);
    answer = "Correct insert";
}

std::string Parser::value (const Column & column, int i)
{
    std::string ret;
    if (cur_lex_type == NUM)
        if (column.type)
            throw std::string ("Error: argument #") + std::to_string(i) + " must be text";
        else
            ret = std::to_string (atol (cur_lex_text.data())) + ' '; 
    else if ((cur_lex_type == STR) && !err)
        if(column.type)
        {
            ret = '\'' + cur_lex_text + "\' ";
            if (column.size < cur_lex_text.size())
                throw std::string ("Error: argument #") + std::to_string(i) + " must be less";
        } else
            throw std::string ("Error: argument #") + std::to_string(i) + " must be number";
    else
        throw std::string ("Error: wrong field #") + std::to_string(i);
    next ();
    return ret;
}

void Parser::where (Table & table)
{
//    std::cout << "where nachalo\n";
    const char* str_2 = str;
    char c_2 = c;
    std::string ans;
    const Column_struct columns = table.get_struct ();
    next ();
    if (cur_lex_text == "ALL")
    {
        table.where_type = 0;
        table.where_not = false;
        return;
    }
    try
    {
        int id;
        bool _not = false;
        if (cur_lex_type != ID)
            throw std::string ("Wrong WHERE clause");
        if (!(id = columns.field_id (cur_lex_text)))
            throw std::string ("Wrong WHERE clause: no such field in table");
        if (!columns[id-1].type)
            throw std::string ("Wrong WHERE clause: field ") + std::to_string(id) + " is not text";
        next ();
        if (cur_lex_text == "NOT")
        {
            next ();
            _not = true;
        }
        if (cur_lex_text != "LIKE")
            throw std::string ("Wrong WHERE clause");
        next ();
        if ((cur_lex_type != STR) || err)
            throw std::string ("Wrong WHERE clause");
        table.where_str = cur_lex_text + '\n';
        next ();
        if (cur_lex_type != END)
            throw std::string ("Wrong WHERE clause");
        table.where_not = _not;
        table.where_type = 1;
        table.where_id = id - 1;
        return;
    }
    catch (std::string st)
    {
        ans = st;
        str = str_2;
        c = c_2;
    }

    next ();
    try
    {
        int id;
        bool _not = false;
        bool str_in = false;
        bool text = false;
        Poliz poliz;
        std::string str;
        if((cur_lex_type == STR))
        {
//        	std::cout << "8888888\n";
            table.where_type = 0;
            str_in = true;
            text = true;
            str = cur_lex_text;
        } 
        else
        {
            id = columns.field_id(cur_lex_text);
            if (id && (columns[id-1].type))
            {
               text = true; 
               table.where_type = 2;
            }
        }
        if (!text)
        {
            num_add(columns, poliz);
            table.where_type = 3;
        }
        else 
            next();
        if (cur_lex_text == "NOT")
        {
            next();
            _not = true;
        }
        if (cur_lex_text != "IN")
            throw std::string ("Wrong WHERE clause: expected IN");
        next ();
        if (cur_lex_text != "(")
            throw std::string ("Wrong WHERE clause: expected (");
        next ();
        if ((text && (cur_lex_type != STR)) || (!text && (cur_lex_type != NUM)))
            throw std::string ("Wrong WHERE clause: invalid type");
        if (str_in && (cur_lex_text == str))
            str_in = false;
        if (text)
            table.where_str = '\'' + cur_lex_text + "\' ";
        else
            table.where_str = cur_lex_text + ' ';
        table.where_count = 1;
        next ();
//        std::cout << "1111\n";
        while (cur_lex_text == ",")
		{
            next ();
            if ((text && (cur_lex_type != STR)) || (!text && (cur_lex_type != NUM)))
            {
                throw std::string ("Wrong WHERE clause: invalid type");
        	}
            if (str_in && (cur_lex_text == str))
            {
				str_in = false;
			}
            if (text)
                table.where_str += '\'' + cur_lex_text + "\' ";
            else
                table.where_str += cur_lex_text + ' ';
            table.where_count += 1;
            next ();
        }
        if (cur_lex_text != ")")
            throw std::string ("Wrong WHERE clause: expected )");
        next ();
        if (cur_lex_type != END)
            throw std::string ("Wrong WHERE clause");
        table.where_not = _not ^ str_in;
        if (text && id)
                table.where_id = id - 1;
        if (!text)
		{
            table.where_type = 3;
            table.where_poliz = poliz;
        }
//        std::cout << table.where_type << std::endl;
        return;
    }
    catch (std::string st)
    {
//        std::cout << "beliberda\n";
//        std::cout << Poliz::count << std::endl;
        ans = st;
        str = str_2;
        c = c_2;
    }
    next ();
    Poliz poliz;
    logic_add (columns, poliz); 
    if (cur_lex_type != END)
        throw std::string ("Wrong WHERE clause");
    table.where_not = false;
    table.where_poliz = poliz;
    table.where_type = 4;
    return;   
    
}

void Parser::logic_add (const Column_struct & columns, Poliz & poliz)
{
    logic_mult (columns, poliz);
    Item* item;
    while (cur_lex_text == "OR")
    {
        next ();
        logic_mult (columns, poliz);
        item = new Item;
        item->type = OR;
        poliz.push (item);
    }
}

void Parser::logic_mult (const Column_struct & columns, Poliz & poliz)
{
    logic_expr (columns, poliz);
    Item* item;
    while (cur_lex_text == "AND")
    {
        item = new Item;
        item->type = AND;
        next ();
        logic_expr (columns, poliz);
        poliz.push (item);
    }
}

void Parser::logic_expr (const Column_struct & columns, Poliz & poliz)
{
    bool no = false;
    bool text = false;
    while(cur_lex_text == "NOT")
    {
        no = !no;
        next ();
    }
    if (cur_lex_text != "(")
        throw std::string ("Wrong WHERE clause: expected (");
    const char* str_2 = str;
    char c_2 = c;
    Poliz poliz_2 = poliz;
    next ();
    try
    {
        if (cur_lex_type == STR)
        {
            text = true;
            TextItem* item = new TextItem;
            item->type = ST;
            item->str = cur_lex_text;
            poliz.push (item);
            next ();
        }
        else
        {
            int id = columns.field_id (cur_lex_text);
            if (id && columns[id-1].type)
            {
                text = true;
                NumItem* item = new NumItem;
                item->type = T;
                item->value = id - 1;
                poliz.push (item);
                next ();
            }
        } 
        if (!text)
            num_add (columns, poliz);
        BoolItem* item_ = new BoolItem;
        item_->type = B;
        if (cur_lex_text == "!")
        {
            next ();
            if (cur_lex_text != "=")
                throw std::string ("Wrong WHERE clause: expected '=' after '!'");
            item_->reverse = true;
            item_->less = false;
            item_->equal = true;
            next ();
        }
        else if (cur_lex_text == "=")
        {
            item_->reverse = false;
            item_->less = false;
            item_->equal = true;
            next ();
        }
        else if(cur_lex_text == ">")
        {
            next ();
            item_->reverse = true;
            item_->less = true;
            if (cur_lex_text == "=")
            {
                next ();
                item_->equal = false;
            }
            else
                item_->equal = true;
        }
        else if (cur_lex_text == "<")
        {
            next ();
            item_->reverse = false;
            item_->less = true;
            if (cur_lex_text == "=")
            {
                next ();
                item_->equal = true;
            }
            else
                item_->equal = false;
        }
        else
            throw std::string();

        if (text)
        {
            if (cur_lex_type == STR)
            {
                TextItem* item = new TextItem;
                item->type = ST;
                item->str = cur_lex_text;
                poliz.push (item);
                next ();
            }
            else
            {
                int id = columns.field_id (cur_lex_text);
                if (!id || !columns[id-1].type)
                    throw std::string ("Wrong WHERE clause: expected text field");
                NumItem* item = new NumItem;
                item->type = I;
                item->value = id - 1;
                poliz.push (item);
                next ();
            }
            poliz.push (item_);
        }
        else
        {
            num_add (columns, poliz);
            poliz.push (item_);
        }
    }
    catch (std::string)
    {
        c = c_2;
        str = str_2;
        next ();
        poliz = poliz_2;
        logic_add (columns, poliz);
    }
    if (no)
    {
        Item* item = new Item;
        item->type = NO;
        poliz.push (item);
    }
    if (cur_lex_text != ")")
        throw std::string ("Wrong WHERE clause: expected )");
    next ();
}

void Parser::num_add (const Column_struct & columns, Poliz & poliz)
{
    num_mult (columns, poliz);
    bool pl;
    Item* item;
    while ((pl = (cur_lex_text == "+")) || (cur_lex_text == "-"))
    {
        next ();
        num_mult (columns, poliz);
        item = new Item;
        if (pl)
            item->type = A;
        else
            item->type = S;
        poliz.push (item);
    }
}

void Parser::num_mult (const Column_struct & columns, Poliz & poliz)
{
    num_expr (columns, poliz);
    Item* item;
    while ((cur_lex_text == "*") || (cur_lex_text == "/") || (cur_lex_text == "%"))
    {
        item = new Item;
        if (cur_lex_text == "*")
            item->type = M;
        else if (cur_lex_text == "/")
            item->type = D;
        else
            item->type = O;
        next ();
        num_expr (columns, poliz);
        poliz.push (item);
    }
}

void Parser::num_expr (const Column_struct & columns, Poliz & poliz)
{
    if (cur_lex_text == "(")
    {
        next ();
        num_add (columns, poliz);
        if (cur_lex_text != ")")
            throw std::string ("Wrong WHERE clause: expected )");
        next ();
    }
    else if (cur_lex_type == NUM)
    {
        NumItem* item = new NumItem;
        item->type = N;
        item->value = atol (cur_lex_text.data());
        poliz.push (item);
        next ();
    }
    else
    {
        int id = columns.field_id (cur_lex_text);
        if (!id || columns[id-1].type)
            throw std::string ("Wrong WHERE clause: invalid field ") + cur_lex_text;
        NumItem* item = new NumItem;
        item->type = I;
        item->value = id - 1;
        poliz.push (item);
        next ();
    }
}

void Parser::remove ()
{
    std::string name = cur_lex_text;
    next ();
    if (cur_lex_text != "FROM")
        throw std::string ("Wrong DELETE command: expected \"FROM\" after \"DELETE\"");
    next ();
    if (cur_lex_type != ID)
         throw std::string ("Wrong DELETE command: table's name should be identifier");   
    name = cur_lex_text;
    next ();
    if (cur_lex_text != "WHERE")
        throw std::string ("Wrong DELETE command: expected \"WHERE\"");
    Table table (name);
    where (table);
    table.remove ();
    answer = "Correct DELETE command";
}

void Parser::update ()
{
    std::string name;
    next ();
    if (cur_lex_type != ID)
         throw std::string ("Wrong UPDATE command: table's name should be identifier");   
    name = cur_lex_text;
    next ();
    if (cur_lex_text != "SET")
        throw std::string ("Wrong UPDATE command: expected \"SET\"");
    next ();
    Table table (name);
    const Column_struct columns = table.get_struct ();
    int id = columns.field_id (cur_lex_text);
    if (!id)
        throw std::string ("Wrong UPDATE command: no such field in table : ") + cur_lex_text;
    bool text = columns[id-1].type;
    next ();
    if (cur_lex_text != "=")
        throw std::string ("Wrong UPDATE command: expected \'=\' ");
    next ();

    int type;
    int second_id;
    Poliz poliz;
    std::string str;
    if (text)
    {
        if (cur_lex_type == STR)
        {
            str = cur_lex_text;
            type = 0;
        }
        else
        {
            second_id = columns.field_id(cur_lex_text);
            if (!second_id)
                throw std::string ("Wrong DELETE command: no such field in table : ") + cur_lex_text;
            second_id -= 1;
            if (!columns[second_id].type)
                throw std::string ("Wrong DELETE command: expected text-field");
            type = 1;
        }
        next ();
    }
    else
    {
        num_add (columns, poliz);
        type = 2;
    }

    if (cur_lex_text != "WHERE")
        throw std::string ("Wrong DELETE command: expected \"WHERE\"");
    where (table);
    table.update (type, str, id - 1, second_id, poliz);
    answer = "Correct UPDATE command";
}

void Parser::select()
{
    std::string name;
    next ();
    std::vector<std::string> fields;
    bool star = false;
    if (cur_lex_text == "*")
    {
        next ();
        star = true;
    }
    else
    {
        if (cur_lex_type != ID)
            throw std::string ("Wrong SELECT command: expected field's identifier");
        fields.push_back (cur_lex_text);
        next ();
        while (cur_lex_text == ",")
        {
            next ();
            if (cur_lex_type != ID)
                throw std::string ("Wrong SELECT command: expected field's identifier");
            fields.push_back (cur_lex_text);
            next ();
        }
    }
    if (cur_lex_text != "FROM")
        throw std::string ("Wrong SELECT command: expected \"FROM\"");
    next ();
    if (cur_lex_type != ID)
         throw std::string ("Wrong SELECT command: table's name must be identifier");  
    name = cur_lex_text;
    next ();
    if (cur_lex_text != "WHERE")
        throw std::string ("Wrong SELECT command: expected \"WHERE\"");
//    std::cout << "Poliz count = " << Poliz::count << '\n';
    Table table (name);
//    std::cout << "Poliz count = " << Poliz::count << '\n';
    const Column_struct columns = table.get_struct ();
    std::vector<int> id_s;
    if (star)
    {
        for (int i = 0; i < table.get_size(); i++)
            id_s.push_back (i);
    }
    else
    {
        for (int i = 0; i < fields.size(); i++) {
            int id = columns.field_id(fields[i]);
            if (!id)
                throw std::string ("Wrong SELECT command: no such field on table: ") + fields[i];
            id_s.push_back (id-1);
        }
    }
    where (table);
    table.select (answer, id_s);
}
