// #include <iostream>
// #include "types.h"


class Lex {
    private:
        Type_of_Lex type;
        int val;
    
    public:
        Lex(Type_of_Lex t = LEX_NULL, int v = 0){
            type = t; val = v;
        }

        Lex(const Lex& l) : type{l.type}, val{l.val} {};
    
        Type_of_Lex get_type () { return type; }
    
        int get_value () { return val; }
    
        friend std::ostream& operator << ( std::ostream &s, Lex l ){
            s << '(' << l.type << ", " << l.val << ");";
            return s;
        }
};
