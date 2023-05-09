
class Identifier{
    private:
        std::string name;
        bool declare; // было описано или нет
        Type_of_Lex type;
        bool assign; // имеет значение или нет
        int value;
    public:
        Identifier (){
            declare = false;
            assign = false;
            type = LEX_NULL;
            value = 0;
        }

        friend std::ostream& operator << ( std::ostream &s, const Identifier& i ){
            s << '(' << i.type << "\t" << i.name << "\t" << i.value << "\t" << i.declare << "\t" << i.assign << ")";
            return s;
        }

        std::string get_name (){ return name; } 
        
        void put_name (const std::string n){ name = n; }
        
        bool get_declare (){ return declare; }
        
        void put_declare (){ declare = true; }
        
        Type_of_Lex get_type (){ return type; }
        
        void put_type ( Type_of_Lex t ){ type = t; }
        
        bool get_assign (){ return assign; }
        
        void put_assign (){ assign = true; }
        
        int get_value (){ return value; }
        
        void put_value (int v){ value = v; }
};

