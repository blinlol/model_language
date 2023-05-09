enum Type_of_Lex{
    // 0
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
    // 10
    LEX_READ,   
    LEX_WRITE,
    LEX_TRUE,
    LEX_FALSE,
    // 14
    LEX_FIN,
    
    LEX_ASSIGN,
    LEX_AND,
    LEX_OR,
    LEX_NOT,
    LEX_PLUS,
    // 20
    LEX_MINUS,
    LEX_UNARY_MINUS,
    LEX_MUL,
    LEX_DIV,
    LEX_SEMICOLON,
    LEX_COLON,
    LEX_COMMA,
    LEX_LESS,
    LEX_LESS_EQUAL,
    LEX_EQUAL,
    LEX_GREATER_EQUAL,
    // 30
    LEX_GREATER,
    LEX_NOT_EQUAL,
    LEX_L_BRACKET,
    LEX_R_BRACKET,
    LEX_L_CURLY_BRACKET,
    LEX_R_CURLY_BRACKET,
    LEX_QUOTES,
    LEX_COMMENT,    
    // 38
    LEX_NUM,
    LEX_ID,
    LEX_CONST_STRING,
    LEX_LABEL,

    POLIZ_LABEL,
    POLIZ_ADDRESS,
    POLIZ_GO,
    POLIZ_FGO

};
