#include <iostream>
#include <vector>

bool like(const char* sample, const char* value) {
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
    if(like)
        std::cout << "YES";
    else
        std::cout << "NO";
    return like;
}

int main() {
    std::string str = "%a%ty%[0-7]\n";
    //std::cin >> str;
    //str += '\n';
    std::string str_2 = "atttty_87\n";
    // std::cin >> str_2;
    //str_2 += '\n';
    like(str.data(), str_2.data());
    return 0;
}