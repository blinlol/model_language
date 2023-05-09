
class Scanner{
    private:
        enum state { 
            H,
            F,
            IDENTIFIER, 
            NUMBER, 
            COMMENT, 
            CONST_STR
            // ALE, 
            // DELIM, 
            // NEQ 
        };

        static std::vector< std::string > T_Words;
        static std::vector< std::string > T_Delimiters;
        static std::vector< Type_of_Lex > words;
        static std::vector< Type_of_Lex > delimiters;
        Table_ID &TID;
        state cur_state;
        std::FILE * fp;
        char c;
        std::string buf;

        void log(std::string s){
            // std::cout << "(Scanner) " << s << std::endl;
        }

        void clear(){ buf = ""; }

        void add(){ buf.push_back(c); }
        
        // возвращаем индекс слова buf в таблице list
        int look( const std::string &buf, const std::vector< std::string > &list ){
            log("look");
            for (int i=0; i < list.size(); i++){
                if (buf == list[i]){
                    return i;
                }
            }
            return 0;
        }

        void read_c(){
            c = std::fgetc(fp);
            // cout << c;
        }
        
        void in_H(int &d, Lex &result);
        void in_NUMBER(int &d, Lex &result);
        void in_IDENTIFIER(Lex &result);
        void in_AND(Lex &result);
        void in_OR(Lex &result);
        void in_LESS(Lex &result);
        void in_EQUAL(Lex &result);
        void in_GREATER(Lex &result);
        void in_NOT(Lex &result);
        void in_SLASH(Lex &result);
        void in_COMMENT(Lex &result);
        void in_QUOTE(Lex &result);
        void in_DELIMITER(Lex &result);

    public:
        Lex get_lex();
        
        // programms filename and TID
        Scanner( const char* program, Table_ID &t): TID(t){
            fp = std::fopen( program, "r" );
            cur_state = H;
            read_c();
            clear();
        }
};

std::vector< Type_of_Lex > Scanner::words = {
    LEX_NULL,
    LEX_INT,
    LEX_STRING,
    LEX_BOOL,
    LEX_IF,
    LEX_ELSE,
    LEX_FOR,
    LEX_WHILE,
    LEX_BREAK,
    LEX_GOTO,
    LEX_READ,
    LEX_WRITE,
    LEX_TRUE,
    LEX_FALSE,
    LEX_NULL
};
std::vector< std::string > Scanner::T_Words = {
    "",
    "int",
    "string",
    "bool",
    "if",
    "else",
    "for",
    "while",
    "break",
    "goto",
    "read",
    "write",
    "true",
    "false",
    ""
};
std::vector< Type_of_Lex > Scanner::delimiters = {
    LEX_NULL, 
    LEX_FIN,
    LEX_ASSIGN,
    LEX_AND,
    LEX_OR,
    LEX_NOT,
    LEX_PLUS,
    LEX_MINUS,
    LEX_MUL,
    LEX_DIV,
    LEX_SEMICOLON,
    LEX_COLON,
    LEX_COMMA,
    LEX_LESS,
    LEX_LESS_EQUAL,
    LEX_EQUAL,
    LEX_GREATER_EQUAL,
    LEX_GREATER,
    LEX_NOT_EQUAL,
    LEX_L_BRACKET,
    LEX_R_BRACKET,
    LEX_L_CURLY_BRACKET,
    LEX_R_CURLY_BRACKET,
    LEX_QUOTES,
    LEX_COMMENT,
    LEX_NULL
};
std::vector< std::string > Scanner::T_Delimiters = {
    "",
    "@",
    "=",
    "&&",
    "||",
    "!",
    "+",
    "-",
    "*",
    "/",
    ";",
    ":",
    ",",
    "<",
    "<=",
    "==",
    ">=",
    ">",
    "!=",
    "(",
    ")",
    "{",
    "}",
    "\"",
    // "/*",
    // "*/",
    ""
};

void Scanner::in_H(int &d, Lex &result){
    log("in_H");
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t'){
        read_c();
    }
    else if (c == '@'){
        result = Lex(LEX_FIN);
        cur_state = F;
    }
    else if (std::isdigit(c)){
        cur_state = NUMBER;
        d = c - '0';
        read_c();
    }
    else if (std::isalpha(c)){
        cur_state = IDENTIFIER;
        clear();
        add();
        read_c();
    }
    else if (c == '&'){
        in_AND(result);
    }
    else if (c == '|'){
        in_OR(result);
    }
    else if (c == '<'){
        in_LESS(result);
    }
    else if (c == '='){
        in_EQUAL(result);
    }
    else if (c == '>'){
        in_GREATER(result);
    }
    else if (c == '!'){
        in_NOT(result);
    }
    else if (c == '/'){
        in_SLASH(result);
    }
    else if (c == '\"'){
        cur_state = CONST_STR;
        clear();
        read_c();
    }
    else{
        in_DELIMITER(result);
    }
}

void Scanner::in_NUMBER(int &d, Lex &result){
    while (std::isdigit(c)){
        d = d * 10 + c - '0';
        read_c();
    }
    result = Lex(LEX_NUM, d);
    cur_state = F;
}
void Scanner::in_IDENTIFIER(Lex &result){
    while (std::isalnum(c)){
        add();
        read_c();
    }
    int i = look(buf, T_Words);
    if (i != 0){
        if (words[i] == LEX_TRUE){
            result = Lex(LEX_TRUE, 1);
        }
        else{
            result = Lex(words[i], 0);
        }
    }
    else{
        int ind = TID.put(buf);
        result = Lex(LEX_ID, ind);
    }
    cur_state = F;
    
}
void Scanner::in_AND(Lex &result){
    read_c();
    if (c == '&'){
        result = Lex(LEX_AND);
        cur_state = F;
        read_c();
    }
    else{
        throw c;
    }
}
void Scanner::in_OR(Lex &result){
    read_c();
    if (c == '|'){
        result = Lex(LEX_OR);
        cur_state = F;
        read_c();
    }
    else{
        throw c;
    }
}
void Scanner::in_LESS(Lex &result){
    read_c();
    if (c == '='){
        result = Lex(LEX_LESS_EQUAL);
        cur_state = F;
        read_c();
    }
    else{
        result = Lex(LEX_LESS);
        cur_state = F;
    }
}
void Scanner::in_EQUAL(Lex &result){
    read_c();
    if (c == '='){
        result = Lex(LEX_EQUAL);
        cur_state = F;
        read_c();
    }
    else{
        result = Lex(LEX_ASSIGN);
        cur_state = F;
    }
}
void Scanner::in_GREATER(Lex &result){
    read_c();
    if (c == '='){
        result = Lex(LEX_GREATER_EQUAL);
        cur_state = F;
        read_c();
    }
    else{
        result = Lex(LEX_GREATER);
        cur_state = F;
    }
}
void Scanner::in_NOT(Lex &result){
    read_c();
    if (c == '='){
        result = Lex(LEX_NOT_EQUAL);
        cur_state = F;
        read_c();
    }
    else{
        result = Lex(LEX_NOT);
        cur_state = F;
    }
}
void Scanner::in_SLASH(Lex &result){
    read_c();
    if (c != '*'){
        result = Lex(LEX_DIV);
        cur_state = F;
    }
    else{
        cur_state = COMMENT;
        read_c();
    }
}
void Scanner::in_COMMENT(Lex &result){
    char prev_c = c;
    read_c();
    while ( !(prev_c == '*' && c == '/') ){
        if (c == '@'){
            throw c;
        }
        prev_c = c;
        read_c();
    }
    read_c();
    cur_state = F;
    result = Lex(LEX_COMMENT);
}
void Scanner::in_QUOTE(Lex &result){
    while (c != '\"'){
        add();
        read_c();
    }
    
    int ind = TID.put(buf);
    result = Lex(LEX_CONST_STRING, ind);
    
    cur_state = F;
    read_c();
}
void Scanner::in_DELIMITER(Lex &result){
    std::string s;
    s.push_back(c);
    int i = look(s, T_Delimiters);
    if (i != 0){
        cur_state = F;
        result = Lex(delimiters[i]);
        read_c();
    }
    else{
        throw c;
    }
}

Lex Scanner::get_lex(){
    log("get_lex");
    clear();
    // read_c();
    int d=0, j;
    cur_state = H;
    Lex result;
    while (true){
        switch (cur_state){
            case H:
                in_H(d, result);
                break;
            
            case F:
                return result;
                break;
            
            case NUMBER:
                in_NUMBER(d, result);
                break;

            case IDENTIFIER:
                in_IDENTIFIER(result);
                break;
            
            case COMMENT:
                in_COMMENT(result);
                break;

            case CONST_STR:
                in_QUOTE(result);
                break;

            default:
                throw c;
        }
    }
}


