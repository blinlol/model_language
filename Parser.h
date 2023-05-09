// #include <iostream>
// #include <string>
// #include <vector>
// #include <stack>
#include <unordered_map>

// #include "types.h"
// #include "Lex.h"
// #include "Identifier.h"
// #include "Table_ID.h"
// #include "Scanner.h"
// #include "Poliz.h"


class Parser{
    private:
        Lex cur_lex;
        Scanner scanner;
        std::stack< int > st_int;
        std::stack< Type_of_Lex > st_lex; // semantic analyse
        Table_ID& TID;
        std::vector<int> pl_breaks;
        std::vector<Lex> lex_buffer;
        std::unordered_map<int, std::vector<int>> undefined_labels;  // undef_l[i] - pl wait for declaration of TID[i]

        void program_s();
        void sentence_s();
        void define_s();
        void assign_s();
        void condition_s();       
        void expression_s();
        void T_s();
        void F_s();
        void if_s();
        void block_s();
        void for_s();
        void while_s();
        void read_s();
        void write_s();
        void break_s();
        void id_s();
        void label_s();
        void goto_s();


        void declare(Type_of_Lex type, Lex id);
        void assign(Lex id, Lex val);
        void make_breaks();


        void check_id();
        void check_id(Lex);
        void check_not();          
        void check_operation();   
        void check_assign();
        void check_bool();
        void check_id_in_read();
        void check_label();


        void get_lex(){
            // log("get_lex");
            if (lex_buffer.empty()){
                cur_lex = scanner.get_lex();
                log("get_lex", cur_lex);
            }
            else{
                cur_lex = *begin(lex_buffer);
                lex_buffer.erase(begin(lex_buffer));
                log("get_lex (buffer)", cur_lex);
            }
        }

        template <typename T1>
        void log(T1 s1){ 
            // std::cout << "(Parser) " << s1 << std::endl;
        }
        template <typename T1, typename T2>
        void log(T1 s1, T2 s2){ 
            // std::cout << "(Parser) " << s1 << "\t" << s2 << std::endl;
        }
        template <typename T1, typename T2, typename T3>
        void log(T1 s1, T2 s2, T3 s3){ 
            // std::cout << "(Parser) " << s1 << "\t" << s2 << "\t" << s3 << std::endl;
        }
        
    
    public:
        Poliz prog;
        Parser(const char * prog_name, Table_ID &t): TID(t), scanner(prog_name, t), prog() {};
        void analize();


// void check_expression(){
//     get_lex();
//     while (cur_lex.get_type() != LEX_FIN){
//         expression_s();
//         get_lex();
//     }
// }



};

void Parser::analize(){
    get_lex();
    program_s();
    // prog.print();
    log("anal completed");
}

void Parser::declare(Type_of_Lex type, Lex id){
    log("declare");
    int i = id.get_value();
    if (TID[i].get_declare()){
        log(id, " is already declared");
        throw "id is already declared";
    }
    TID[i].put_declare();
    TID[i].put_type(type);
}

void Parser::make_breaks(){
    for (auto pl: pl_breaks){
        prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl);
    }
}

void Parser::check_id(){
    log("check_id");
    int i = cur_lex.get_value();
    if (TID[i].get_declare()){
        st_lex.push(TID[i].get_type());
    }
    else{
        log("not declared ", cur_lex);
        throw cur_lex;
    }
}

void Parser::check_id(Lex l){
    log("check_id(Lex)");
    int i = l.get_value();
    if (TID[i].get_declare()){
        st_lex.push(TID[i].get_type());
    }
    else{
        log("not declared ", cur_lex);
        throw cur_lex;
    }
}

void Parser::check_not(){
    log("check_not");
    if (st_lex.empty()){
        log("st_lex is empty");
        throw cur_lex;
    }
    auto target_type = st_lex.top();
    st_lex.pop();
    if (target_type == LEX_BOOL){
        st_lex.push(LEX_BOOL);
        prog.put_lex(Lex(LEX_NOT));
    }
    else{
        log("require bool");
        throw cur_lex;
    }
}

void Parser::check_operation(){
    log("check_operation");
    if (st_lex.size() < 3){
        log("st_lex.size() < 3");
        throw cur_lex;
    }

    auto rhs = st_lex.top();
    st_lex.pop();
    auto operation = st_lex.top();
    st_lex.pop();
    auto lhs = st_lex.top();
    st_lex.pop();

    Type_of_Lex op_type;
    Type_of_Lex result_type;
    
    if (  operation == LEX_AND || 
          operation == LEX_OR){
        op_type = LEX_BOOL;
        result_type = LEX_BOOL;
    }
    else if (operation == LEX_PLUS){
        if (lhs == LEX_STRING){
            op_type = LEX_STRING;
            result_type = LEX_STRING;
        }
        else {
            op_type = LEX_INT;
            result_type = LEX_INT;
        }
    }
    else if (operation == LEX_MINUS || 
             operation == LEX_MUL || 
             operation == LEX_DIV){    
        op_type = LEX_INT;
        result_type = LEX_INT;
    }
    else if (operation == LEX_LESS || 
             operation == LEX_LESS_EQUAL || 
             operation == LEX_EQUAL || 
             operation == LEX_NOT_EQUAL || 
             operation == LEX_GREATER_EQUAL || 
             operation == LEX_GREATER){
        result_type = LEX_BOOL;
        if (lhs == LEX_STRING){
            op_type = LEX_STRING;
        }
        else{
            op_type = LEX_INT;
        }
    }
    else{
        log("wrong operation ", operation);
        throw cur_lex;
    }

    if (rhs == op_type && lhs == op_type){
        st_lex.push(result_type);
        prog.put_lex(Lex(operation));
    }
    else{
        log("wrong rhs, lhs", rhs, lhs);
        throw cur_lex;
    }
}

void Parser::check_assign(){
    log("check_assign");
    auto rhs = st_lex.top();
    st_lex.pop();
    auto lhs = st_lex.top();
    st_lex.pop();
    if (lhs != rhs){
        throw "wrong types in assign";
    }
    
}

void Parser::check_bool(){
    log("check_bool");
    if (st_lex.empty()){
        log("st_lex is empty");
        throw cur_lex;
    }
    auto t = st_lex.top();
    st_lex.pop();
    if (t != LEX_BOOL){
        throw "expression is not boolean";
    }
}

void Parser::check_id_in_read(){
    if (!TID[cur_lex.get_value()].get_declare()){
        log("variable is not declared");
        throw cur_lex;
    }
}


void Parser::program_s(){
    log("program_s");
    while (cur_lex.get_type() != LEX_FIN){
        sentence_s();
        
    }
}

void Parser::sentence_s(){
    log("sentence_s");
    switch (cur_lex.get_type()){
    case LEX_INT:
    case LEX_BOOL:
    case LEX_STRING:
        define_s();
        if (cur_lex.get_type() != LEX_SEMICOLON){
            throw cur_lex;
        }
        get_lex();
        break;
    
    case LEX_ID:
        id_s();
        // if (cur_lex.get_type() != LEX_SEMICOLON){
        //     throw cur_lex;
        // }
        // get_lex();
        break;

    case LEX_IF:
        if_s();
        break;
    
    case LEX_FOR:
        for_s();
        make_breaks();
        break;
    
    case LEX_WHILE:
        while_s();
        make_breaks();
        break;

    case LEX_COMMENT:
        get_lex();
        break;
    
    case LEX_READ:
        read_s();
        if (cur_lex.get_type() != LEX_SEMICOLON){
            throw cur_lex;
        }
        get_lex();
        break;
    
    case LEX_WRITE:
        write_s();
        if (cur_lex.get_type() != LEX_SEMICOLON){
            throw cur_lex;
        }
        get_lex();
        break;

    case LEX_BREAK:
        break_s();
        if (cur_lex.get_type() != LEX_SEMICOLON){
            throw cur_lex;
        }
        get_lex();
        break;

    case LEX_GOTO:
        goto_s();
        if (cur_lex.get_type() != LEX_SEMICOLON){
            throw cur_lex;
        }
        get_lex();
        break;

    default:
        throw cur_lex;
    }
}

void Parser::assign_s(){
    log("assign_s");
    if (cur_lex.get_type() != LEX_ID){
        throw cur_lex;
    }
    Lex id = cur_lex;
    check_id();
    prog.put_lex(Lex(POLIZ_ADDRESS, cur_lex.get_value()));

    get_lex();
    if (cur_lex.get_type() != LEX_ASSIGN){
        throw cur_lex;
    }
    
    get_lex();
    expression_s();
    check_assign();
    prog.put_lex(Lex(LEX_ASSIGN));
}

void Parser::define_s(){
    log("define_s");
    Type_of_Lex type = cur_lex.get_type();
    if (type != LEX_BOOL &&
        type != LEX_INT &&
        type != LEX_STRING){
        throw cur_lex;
    }
    
    get_lex();
    while (true){
        if (cur_lex.get_type() != LEX_ID){
            log("define_s expected identifier");
            throw cur_lex;
        }
        Lex id = cur_lex;
        declare(type, id);

        get_lex();
        if (cur_lex.get_type() == LEX_ASSIGN){
            check_id(id);
            prog.put_lex(Lex(POLIZ_ADDRESS, id.get_value()));
            get_lex();
            expression_s();
            check_assign();
            prog.put_lex(Lex(LEX_ASSIGN));

        }
        if (cur_lex.get_type() == LEX_COMMA){
            get_lex();
        }
        else{
            break;
        }
    }

    // get_lex();
    // if (cur_lex.get_type() != LEX_ID){
    //     throw cur_lex;
    // }
    // Lex identifier = cur_lex;
    // declare(type, identifier);

    // get_lex();
}

void Parser::condition_s(){
    expression_s();
    if (cur_lex.get_type() == LEX_NOT_EQUAL ||
        cur_lex.get_type() == LEX_GREATER ||
        cur_lex.get_type() == LEX_GREATER_EQUAL ||
        cur_lex.get_type() == LEX_EQUAL ||
        cur_lex.get_type() == LEX_LESS_EQUAL ||
        cur_lex.get_type() == LEX_LESS){
        
        st_lex.push(cur_lex.get_type());
        get_lex();
        expression_s();
        check_operation();
    }
}

void Parser::expression_s(){
    log("expression_s");
    bool is_unary_minus=false;
    if (cur_lex.get_type() == LEX_MINUS){
        st_lex.push(LEX_UNARY_MINUS);
        get_lex();
        is_unary_minus = true;
    }
    T_s();
    
    if (is_unary_minus){
        check_operation();
    }

    while (cur_lex.get_type() == LEX_PLUS || 
           cur_lex.get_type() == LEX_MINUS ||
           cur_lex.get_type() == LEX_OR){
        
        st_lex.push(cur_lex.get_type());
        get_lex();
        T_s();
        check_operation();
    }
}

void Parser::T_s(){
    log("T_s");
    F_s();
    while (cur_lex.get_type() == LEX_MUL || 
           cur_lex.get_type() == LEX_DIV ||
           cur_lex.get_type() == LEX_AND){
        
        st_lex.push(cur_lex.get_type());
        get_lex();
        F_s();
        check_operation();
   }
}

void Parser::F_s(){
    log("F_s");
    switch (cur_lex.get_type()){
        case LEX_ID:
            check_id();
            prog.put_lex(cur_lex);
            get_lex();
            break;
        
        case LEX_NUM:
            st_lex.push(LEX_INT);
            prog.put_lex(cur_lex);
            get_lex();
            break;
        
        case LEX_TRUE:
        case LEX_FALSE:
            st_lex.push(LEX_BOOL);
            prog.put_lex(cur_lex);
            get_lex();
            break;
        
        case LEX_NOT:
            get_lex();
            F_s();
            check_not();
            break;
        
        case LEX_L_BRACKET:
            get_lex();
            condition_s();
            if (cur_lex.get_type() != LEX_R_BRACKET){
                log("F: balance of bracket is broken");
                throw cur_lex;
            }
            get_lex();
            break;
        
        case LEX_CONST_STRING:
            st_lex.push(LEX_STRING);
            prog.put_lex(cur_lex);
            get_lex();
            break;

        default:
            throw cur_lex;
    }
}

void Parser::if_s(){
    log("if_s");
    // if
    if (cur_lex.get_type() != LEX_IF){
        throw cur_lex;
    }
    // condition
    get_lex();
    if (cur_lex.get_type() != LEX_L_BRACKET){
        log("if_s expected left bracket");
        throw cur_lex;
    }
    get_lex();
    condition_s();
    check_bool();
    if (cur_lex.get_type() != LEX_R_BRACKET){
        log("if_s expected right bracket");
        throw cur_lex;
    }

    auto pl2 = prog.get_free();
    prog.blank();
    prog.put_lex(Lex(POLIZ_FGO));

    // true block
    get_lex();
    block_s();
    
    auto pl3 = prog.get_free();
    prog.blank();
    prog.put_lex(Lex(POLIZ_GO));
    prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl2);

    // else
    if (cur_lex.get_type() == LEX_ELSE){
        get_lex();
        block_s();
    }
    prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl3);
}

void Parser::block_s(){
    log("block_s");
    if (cur_lex.get_type() != LEX_L_CURLY_BRACKET){
        sentence_s();
    }
    else{
        get_lex();
        while (cur_lex.get_type() != LEX_R_CURLY_BRACKET){
            sentence_s();
        }
        get_lex();
    }
}

void Parser::for_s(){
    log("for_s");
    pl_breaks.clear();

    // for
    if (cur_lex.get_type() != LEX_FOR){
        throw cur_lex;
    }
    // left bracket
    get_lex();
    if (cur_lex.get_type() != LEX_L_BRACKET){
        log("for_s expected left bracket");
        throw cur_lex;
    }

    // assign
    get_lex();
    if (cur_lex.get_type() != LEX_SEMICOLON){
        if (cur_lex.get_type() == LEX_ID){
            assign_s();
        }
        else{
            define_s();
        }
    }
    
    // semicolon
    if (cur_lex.get_type() != LEX_SEMICOLON){
        log("for_s expected semicolon");
        throw cur_lex;
    }

    auto ppl1 = prog.get_free();

    // condition
    get_lex();
    condition_s();
    check_bool();

    auto pl2 = prog.get_free();
    prog.blank();
    prog.put_lex(Lex(POLIZ_FGO));

    auto pl3 = prog.get_free();
    prog.blank();
    prog.put_lex(Lex(POLIZ_GO));

    auto ppl4 = prog.get_free();
         
    // semicolon
    if (cur_lex.get_type() != LEX_SEMICOLON){
        log("for_s expected semicolon");
        throw cur_lex;
    }
    get_lex();

    // assign
    if (cur_lex.get_type() != LEX_R_BRACKET){
        assign_s();
    }

    prog.put_lex(Lex(POLIZ_LABEL, ppl1));
    prog.put_lex(Lex(POLIZ_GO));

    // right bracket
    if (cur_lex.get_type() != LEX_R_BRACKET){
        log("for_s expected right bracket");
        throw cur_lex;
    }
    get_lex();

    prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl3);
    block_s();

    prog.put_lex(Lex(POLIZ_LABEL, ppl4));
    prog.put_lex(Lex(POLIZ_GO));

    prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl2);
    prog.blank();
}

void Parser::while_s(){
    log("while_s");
    pl_breaks.clear();

    // while
    if (cur_lex.get_type() != LEX_WHILE){
        throw cur_lex;
    }
    // left bracket
    get_lex();
    if (cur_lex.get_type() != LEX_L_BRACKET){
        log("while_s expected left bracket");
        throw cur_lex;
    }

    auto ppl1 = prog.get_free();
    
    // condition
    get_lex();
    condition_s();
    check_bool();

    // right bracket
    if (cur_lex.get_type() != LEX_R_BRACKET){
        log("while_s expected right bracket");
        throw cur_lex;
    }

    auto pl2 = prog.get_free();
    prog.blank();
    prog.put_lex(Lex(POLIZ_FGO));

    // block
    get_lex();
    block_s();

    prog.put_lex(Lex(POLIZ_LABEL, ppl1));
    prog.put_lex(Lex(POLIZ_GO));
    prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl2);
}

void Parser::read_s(){
    log("read_s");
    if (cur_lex.get_type() != LEX_READ){
        throw cur_lex;
    }

    get_lex();
    if (cur_lex.get_type() != LEX_L_BRACKET){
        log("read_s expected left bracket");
        throw cur_lex;
    }
    
    get_lex();
    while (true){
        if (cur_lex.get_type() != LEX_ID){
            log("read_s expected identifier");
            throw cur_lex;
        }

        check_id_in_read();
        prog.put_lex(Lex(POLIZ_ADDRESS, cur_lex.get_value()));
        prog.put_lex(Lex(LEX_READ));
        
        get_lex();        
        if (cur_lex.get_type() != LEX_COMMA){
            break;
        }
        get_lex();
    }

    if (cur_lex.get_type() != LEX_R_BRACKET){
        log("read_s expected right bracket");
        throw cur_lex;
    }
    get_lex();
}

void Parser::write_s(){
    log("write_s");
    if (cur_lex.get_type() != LEX_WRITE){
        throw cur_lex;
    }
    get_lex();
    if (cur_lex.get_type() != LEX_L_BRACKET){
        log("write_s expected left bracket");
        throw cur_lex;
    }
    get_lex();
    while (true){
        condition_s();
        prog.put_lex(Lex(LEX_WRITE));
        if (cur_lex.get_type() != LEX_COMMA){
            break;
        }
        get_lex();
    }

    if (cur_lex.get_type() != LEX_R_BRACKET){
        log("write_s expected right bracket");
        throw cur_lex;
    }
    get_lex();
}

void Parser::break_s(){
    log("break_s");
    if (cur_lex.get_type() != LEX_BREAK){
        throw cur_lex;
    }
    get_lex();

    pl_breaks.push_back(prog.get_free());
    prog.blank();
    prog.put_lex(Lex(POLIZ_GO));
}

void Parser::id_s(){
    log("id_s");
    if (cur_lex.get_type() != LEX_ID){
        throw cur_lex;
    }
    auto temp = cur_lex;
    get_lex();
    lex_buffer.push_back(temp);
    lex_buffer.push_back(cur_lex);
    if (cur_lex.get_type() == LEX_COLON){
        get_lex();
        label_s();
    }
    else{
        get_lex();
        assign_s();
        if (cur_lex.get_type() != LEX_SEMICOLON){
            throw cur_lex;
        }
        get_lex();
    }
}

void Parser::label_s(){
    log("label_s");
    if (cur_lex.get_type() != LEX_ID){
        throw cur_lex;
    }

    declare(LEX_LABEL, cur_lex);
    int i = cur_lex.get_value();
    TID[i].put_value(prog.get_free());
    TID[i].put_assign();

    get_lex();
    if (cur_lex.get_type() != LEX_COLON){
        log("label_s expected colon");
        throw cur_lex;
    }

    for (auto pl: undefined_labels[i]){
        prog.put_lex(Lex(POLIZ_LABEL, TID[i].get_value()), pl);
    }
    undefined_labels[i].clear();

    get_lex();
    sentence_s();
}

void Parser::goto_s(){
    log("goto_s");
    if (cur_lex.get_type() != LEX_GOTO){
        throw cur_lex;
    }
    
    get_lex();
    if (cur_lex.get_type() != LEX_ID){
        log("goto_s expected identifier");
        throw cur_lex;
    }
    
    int i = cur_lex.get_value();
    if (TID[i].get_declare()){
        prog.put_lex(Lex(POLIZ_LABEL, TID[i].get_value()));
        prog.put_lex(Lex(POLIZ_GO));
    }
    else{
        undefined_labels[i].push_back(prog.get_free());
        prog.blank();
        prog.put_lex(Lex(POLIZ_GO));
    }
    get_lex();
}

