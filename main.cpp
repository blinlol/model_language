#include <iostream>
#include <string>
#include <vector>
#include <stack>

#include "types.h"
#include "Lex.h"
#include "Identifier.h"
#include "Table_ID.h"
#include "Scanner.h"
#include "Poliz.h"
#include "Parser.h"
#include "Executer.h"
#include "Interpretator.h"



int main(){
    using namespace std;

    // Table_ID TID;
    // Scanner scanner("test", TID);
    // Parser parser("test", TID);
    Interpretator vasya("test");

    try {
        vasya.interpretation();
    }
    catch (char c){
        cout << "(main) bad char: " << c << endl;
    }
    catch (Lex l){
        cout << "(main) bad Lex: " << l << endl;
    }
    catch (char const* s){
        cout << "(main) " << s << endl;
    }
    
    cout << endl;
    // cout << TID << endl;
}