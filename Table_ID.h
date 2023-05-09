
class Table_ID{
    private:
        std::vector<Identifier> id_vec;

    public:
        Identifier& operator[] ( int k ){ return id_vec[k]; }
        friend std::ostream& operator << ( std::ostream &s, const Table_ID& t ){
            s << "type\tname\tval\tdecl\tassigned" << std::endl;
            // s << "[ ";
            for (Identifier i: t.id_vec){
                s << i << std::endl;
            }
            // s << "]" << std::endl;
            return s;
        }
        int put( const std::string buf );
};

int Table_ID::put ( const std::string buf ){
    for ( int j=0; j < id_vec.size(); ++j){
        if ( buf == id_vec[j].get_name() ){
            return j;
        }
    }

    id_vec.push_back(Identifier());
    id_vec.back().put_name(buf);
    return id_vec.size() - 1;
}

