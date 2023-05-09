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



class Executer{
    private:
        Lex cur_lex;
        std::stack<int> args;
        std::stack<Lex> args_lex;
        int index=0;

    public:
        void execute(Poliz& program, Table_ID& TID);
        template<typename T1>
        void log(T1 s1){
            std::cout << "(Executer) " << s1 << std::endl;
        };
        
};

void Executer::execute(Poliz& program, Table_ID& TID){
    log("start");
    args = {};

    index = 0;
    int value, t1, t2, i, condition;
    bool is_str;
    Lex top;
    while (index < program.get_free()){
        cur_lex = program[index];
        switch (cur_lex.get_type()){
            case LEX_TRUE:
            case LEX_FALSE:
            case LEX_NUM:
            case LEX_STRING:
            case LEX_CONST_STRING:
            case POLIZ_ADDRESS:
            case POLIZ_LABEL:
                args.push(cur_lex.get_value());
                args_lex.push(cur_lex);
                break;
            
            case LEX_ID:
                i = cur_lex.get_value();
                if (TID[i].get_assign()){
                    args.push(TID[i].get_value());
                    args_lex.push(cur_lex);
                }
                else{
                    log("id isnt assign");
                    throw cur_lex;
                }
                break;
            
            case LEX_AND:
                t2 = args.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 && t2);
                if (t1 && t2){
                    args_lex.push(Lex(LEX_TRUE, 1));
                }
                else{
                    args_lex.push(Lex(LEX_FALSE));
                }
                break;
            
            case LEX_OR:
                t2 = args.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 || t2);
                if (t1 && t2){
                    args_lex.push(Lex(LEX_TRUE, 1));
                }
                else{
                    args_lex.push(Lex(LEX_FALSE));
                }
                break;
            
            case POLIZ_GO:
                index = args.top() - 1;
                args.pop();
                args_lex.pop();
                break;
            
            case POLIZ_FGO:
                i = args.top();
                args.pop();
                args_lex.pop();
                condition = args.top();
                args.pop();
                args_lex.pop();
                if (!condition){
                    index = i - 1;
                }
                break;
            
            case LEX_WRITE:
                switch (args_lex.top().get_type()){
                    case LEX_NUM:
                        std::cout << args.top() << std::endl;
                        break;
                    case LEX_TRUE:
                        std::cout << "true" << std::endl;
                        break;
                    case LEX_FALSE:
                        std::cout << "false" << std::endl;
                        break;
                    case LEX_CONST_STRING:
                        top = args_lex.top();
                        std::cout << TID[top.get_value()].get_name() << std::endl;
                        break;
                    case LEX_ID:
                        top = args_lex.top();
                        if (TID[top.get_value()].get_type() == LEX_STRING){
                            int j = TID[top.get_value()].get_value();
                            std::cout << TID[j].get_name() << std::endl;
                        }
                        else if (TID[top.get_value()].get_type() == LEX_BOOL){
                            std::cout << (TID[top.get_value()].get_value() == 1 ? "true" : "false") << std::endl;
                        }
                        else{
                            std::cout << args.top() << std::endl;
                        }
                        break;
                }
                break;

            case LEX_READ:
                i = args.top();
                args.pop();
                args_lex.pop();
                if (TID[i].get_type() == LEX_INT){
                    std::cin >> value;
                }
                else if (TID[i].get_type() == LEX_BOOL){
                    std::string s;
                    std::cin >> s;
                    value = -1;
                    while (value == -1){
                        if (s == "true"){
                            value = 1;
                        }
                        else if (s == "false"){
                            value = 0;
                        }
                        else{
                            log("wrong input, enter false/true");
                            std::cin >> s;
                        }
                    }
                }
                else if (TID[i].get_type() == LEX_STRING){
                    std::string s;
                    std::cin >> s;
                    value = TID.put(s);
                    TID[value].put_type(LEX_CONST_STRING);
                }
                else{
                    throw cur_lex;
                }
                TID[i].put_value(value);
                TID[i].put_assign();
                break;                     

            case LEX_PLUS:
                top = args_lex.top();
                is_str = false;
                if (top.get_type() == LEX_ID){
                    if (TID[top.get_value()].get_type() == LEX_STRING){
                            is_str = true;
                        }
                }
                else if (top.get_type() == LEX_CONST_STRING){
                    is_str = true;
                }
                if (!is_str){
                    t2 = args.top();
                    args.pop();
                    auto type = args_lex.top().get_type();
                    args_lex.pop();
                    t1 = args.top();
                    args.pop();
                    args_lex.pop();
                    args.push(t1 + t2);
                    args_lex.push(Lex(type, t1 + t2));  
                }
                else{
                    std::string lhs, rhs;
                    Lex lhs_l, rhs_l;
                    rhs_l = args_lex.top();
                    args.pop();
                    args_lex.pop();
                    lhs_l = args_lex.top();
                    args.pop();
                    args_lex.pop();

                    if (lhs_l.get_type() == LEX_CONST_STRING){
                        lhs = TID[lhs_l.get_value()].get_name();
                    }
                    else if (lhs_l.get_type() == LEX_ID && 
                             TID[lhs_l.get_value()].get_type() == LEX_STRING){
                        int j = TID[lhs_l.get_value()].get_value();
                        lhs = TID[j].get_name();
                    }
                    if (rhs_l.get_type() == LEX_CONST_STRING){
                        rhs = TID[rhs_l.get_value()].get_name();
                    }
                    else if (rhs_l.get_type() == LEX_ID &&
                             TID[rhs_l.get_value()].get_type() == LEX_STRING){
                        int j = TID[rhs_l.get_value()].get_value();
                        rhs = TID[j].get_name();
                    }

                    int j = TID.put(lhs + rhs);
                    TID[j].put_type(LEX_CONST_STRING);
                    args_lex.push(Lex(LEX_CONST_STRING, j));
                    args.push(j);
                }
                break;
            
            case LEX_MINUS:
                t2 = args.top();
                top = args_lex.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 - t2);
                args_lex.push(Lex(top.get_type(), t1 - t2));
                break;
            
            case LEX_DIV:
                t2 = args.top();
                top = args_lex.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                if (t2 == 0){
                    throw "division by zero";
                }
                args.push(t1 / t2);
                args_lex.push(Lex(top.get_type(), t1 / t2));
                break;
            
             case LEX_MUL:
                t2 = args.top();
                top = args_lex.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 * t2);
                args_lex.push(Lex(top.get_type(), t1 * t2));
                break;
            
            case LEX_LESS:
                t2 = args.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 < t2);
                args_lex.push(Lex((t1 < t2 ? LEX_TRUE : LEX_FALSE), t1 < t2));
                break;
            
            case LEX_LESS_EQUAL:
                t2 = args.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 <= t2);
                args_lex.push(Lex((t1 <= t2 ? LEX_TRUE : LEX_FALSE), t1 <= t2));
                break;
            
            case LEX_EQUAL:
                t2 = args.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 == t2);
                args_lex.push(Lex((t1 == t2 ? LEX_TRUE : LEX_FALSE), t1 == t2));
                break;

            case LEX_GREATER_EQUAL:
                t2 = args.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 >= t2);
                args_lex.push(Lex((t1 >= t2 ? LEX_TRUE : LEX_FALSE), t1 >= t2));
                break;

            case LEX_GREATER:
                t2 = args.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 > t2);
                args_lex.push(Lex((t1 > t2 ? LEX_TRUE : LEX_FALSE), t1 > t2));
                break;
            
            case LEX_NOT_EQUAL:
                t2 = args.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                args.pop();
                args_lex.pop();
                args.push(t1 != t2);
                args_lex.push(Lex((t1 != t2 ? LEX_TRUE : LEX_FALSE), t1 != t2));
                break;

            case LEX_ASSIGN:
                t2 = args.top();
                args.pop();
                args_lex.pop();
                t1 = args.top();
                // args.pop();
                TID[t1].put_value(t2);
                TID[t1].put_assign();
                break;

            case LEX_NULL:
                break;

            default:
                log("unexpected lex");
                throw cur_lex;
        }
        index++;
    }
    log("finish");
}









