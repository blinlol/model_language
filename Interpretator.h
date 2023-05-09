// #include <iostream>
// #include <string>
// #include <vector>
// #include <stack>

// #include "types.h"
// #include "Lex.h"
// #include "Identifier.h"
// #include "Table_ID.h"
// #include "Scanner.h"
// #include "Poliz.h"
// #include "Parser.h"
// #include "Executer.h"



class Interpretator{
    private:
        Parser parser;
        Executer executer;
        Table_ID TID;
    
    public:
        Interpretator(const char* program): TID{}, parser{program, TID}, executer{}{};
        void interpretation();
        template<typename T1>
        void log(T1 s1){
            // std::cout << "(Interpretator) " << s1 << std::endl;
        };
};

void Interpretator::interpretation(){
    log("start");
    parser.analize();
    executer.execute(parser.prog, TID);
    // std::cout << TID;
}