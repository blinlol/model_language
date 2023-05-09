// #include <iostream>
// #include <string>
// #include <vector>
// #include <stack>

// #include "types.h"
// #include "Lex.h"
// #include "Identifier.h"
// #include "Table_ID.h"
// #include "Scanner.h"


class Poliz{
    private:
        std::vector<Lex> program;

    public:
        Poliz() = default;
        ~Poliz() = default;

        template<typename T1>
        void log(T1 s1){
            // std::cout << "(Poliz) " << s1 << std::endl;
        };

        void print();
        void put_lex(Lex l);
        void put_lex(Lex l, int ind);
        void blank();
        int get_free() { return program.size(); }
        Lex& operator[](int index);
};

void Poliz::print(){
    log("print");
    for (auto l: program){
        std::cout << l << "\t";
    }
    std::cout << std::endl;
}

void Poliz::put_lex(Lex l){
    log("put_lex(l)");
    program.push_back(l);
}

void Poliz::put_lex(Lex l, int ind){
    log("put_lex(l, i)");
    if (ind > program.size()){
        log("index out of range");
        throw l;
    }
    if (ind == program.size()){
        put_lex(l);
    }
    else{
        program[ind] = l;
    }
}

void Poliz::blank(){
    log("blank");
    put_lex(Lex{});
}

Lex& Poliz::operator[](int i){
    if (i >= program.size()){
        log("index out of range");
        throw "out of range";
    }
    return program[i];
}



