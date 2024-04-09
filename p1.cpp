#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
#define end "\n"


struct Alumno{
    char codigo[5];
    char nombre[11];
    char apellidos[20];
    char carrera[5];
    int ciclo;
    float mensualidad;

    void print(){
        cout<<codigo<<" "<<nombre<<" "<<apellidos<<" "<<carrera<<" "<<(ciclo + 1)<<" "<<mensualidad<<end;
    }
};

fstream& operator >>( fstream& stream , Alumno& al ){
    stream.read((char*)&al , sizeof(al) );

    return stream;
}


enum TYPE{
    MOVE_TO_LAST,
    FREE_LIST
};

class FixedRecord{
private:
    string filename;
    TYPE type;

public:

    int get_header(){
        ifstream infile(filename , ios::binary);
        int sz;
        Alumno al;
        int tam;
        if(type == MOVE_TO_LAST) tam = sizeof(Alumno); 
        else tam = sizeof(Alumno) + sizeof(int);

        infile.read((char*)&sz , tam);
        infile.close();
        return sz;
    }

    

    FixedRecord(string filename , TYPE type){
        ofstream f(filename , ios::out | ios::binary);
        this->filename = filename;
        this->type = type;
        if( this->type == MOVE_TO_LAST ){
            int sz_i = 0;
            f.write((char*)& sz_i , sizeof(Alumno));
            f.close(); 
            
        }
        else{
            int sz_i = -1;
            f.write((char*)& sz_i , sizeof(Alumno) + sizeof(int));
            f.close(); 
        }
    }

    void Add(Alumno& record){
        if( type == MOVE_TO_LAST){
            int i = get_header();
            fstream file(filename , ios::in | ios::out | ios::binary);

            file.seekg((i+1)*(int)sizeof(Alumno) , ios::beg);

            file.write((char*)(&record), sizeof(record));

            i++;

            file.seekg(0 , ios :: beg);

            file.write((char*)(&i) , sizeof(Alumno)) ;   

            file.close();
        }
        else{
            int head = get_header();
            

            if( head != -1){
                fstream file(filename , ios::in | ios::out | ios::binary);
                file.seekg(head*(sizeof(Alumno) + sizeof(int)), ios::beg  );
                Alumno temp{};
                int next;
                int x = 0;
                file.read((char*)&temp , sizeof(temp));
                file.read((char*)&next , sizeof(next));
                cout<<"Este es el next"<<next<<end;
                cout<<"Este es el head"<<head<<endl;
                file.seekg(head*( sizeof(Alumno) + sizeof(int) )  , ios::beg  );
                file.write((char*)&record , sizeof(record));
                file.write((char*)&x , sizeof(x));
                file.seekg(0 , ios::beg);
                file.write((char*)&next , sizeof(next) + sizeof(Alumno));
                file.close();          
            }
            else{
                fstream file(filename , ios::in | ios::out | ios::binary | ios::app);
                int x = 0;
                file.write((char*)&record , sizeof(record));
                file.write((char*)&x , sizeof(x));
                file.close();
            }
        }
    }

    void DeleteRecord(int i){
        if( type == MOVE_TO_LAST){
            fstream file(filename , ios::in | ios::out | ios::binary);
            Alumno al1;
            int sz = get_header();
            file.seekg( sz*sizeof(Alumno) , ios::beg);

            file.read((char*)&al1 , sizeof(al1));

            file.seekg(i*sizeof(Alumno) , ios::beg);

            file.write((char*)&al1 , sizeof(al1) );

            sz--;

            file.seekg(0 , ios::beg);

            file.write((char*)&sz , sizeof(Alumno));

            file.close();
        }
        else{
            fstream file(filename , ios::in | ios::out | ios::binary);
            int head = get_header();
            Alumno temp{"A001", "Ana", "Perez", "INF", 1, 1200.50};
            int pos = i;
            file.seekg( pos*( sizeof(Alumno) + sizeof(int) )  , ios::beg  );
            file.write((char*)&temp , sizeof(temp));
            file.write((char*)&head  , sizeof(head));
            
            file.seekg(0 , ios::beg);
            
            file.write( (char*)&pos, sizeof(pos) + sizeof(int) );

            file.close();
        }
    }

    Alumno readRecord(int pos){
        Alumno al{};
        if( type == MOVE_TO_LAST){
            fstream file(filename , ios::in | ios::out | ios::binary);
            file.seekg(pos*sizeof(al) , ios::beg);

            file.write((char*)&al , sizeof(al));
            file.close();
        }



        return al;
    }
    

    vector<Alumno> load(){
        vector<Alumno> ans;
        if( type == MOVE_TO_LAST){
            
            int n = get_header();
            fstream file(filename , ios::in | ios::out | ios::binary);
            file.seekg(1*sizeof(Alumno) , ios::beg);
            int i = 1;
            while(1){
                Alumno al{};
                file>>al;
                if( i > n) break;
            
                ans.push_back(al);
                i++;
            }
           
        }
        else{
            fstream file(filename , ios::in | ios::out | ios::binary);
            file.seekg(1*(sizeof(Alumno) + sizeof(int)) , ios::beg );
            while(1){
                Alumno a1{};
                int next;
                file.read((char*)&a1 , sizeof(a1));
                file.read((char*)&next , sizeof(next));    
                if( file.eof()) break;

                if( next == 0) ans.push_back(a1);
            }    
        }

        return ans;
    }

};






void test1(){
    FixedRecord fr("data.bin" , MOVE_TO_LAST);
    
    Alumno alumnos[10] = {
        {"A001", "Ana", "Perez", "INF", 1, 1200.50},
        {"A002", "Luis", "Gomez", "ADM", 2, 1100.00},
        {"A003", "Mia", "Lopez", "DER", 3, 1300.75},
        {"A004", "Juan", "Diaz", "MED", 4, 1400.25},
        {"A005", "Sara", "Mora", "PSI", 1, 1250.00},
        {"A006", "Leo", "Vega", "ARQ", 2, 1350.45},
        {"A007", "Rita", "Solis", "CIV", 3, 1450.60},
        {"A008", "Tito", "Luna", "IND", 4, 1550.20},
        {"A009", "Lola", "Nuez", "INF", 1, 1150.30},
        {"A010", "Paco", "Rio", "ADM", 2, 1050.90}
    };

    for(int i = 0; i < 10; i++) {
        fr.Add(alumnos[i]);
    }

    vector<Alumno> v_al = fr.load();
    
    for(int i = 0; i < v_al.size(); i++) {
        v_al[i].print();
    }

    int pos = 5;

    fr.DeleteRecord(5);

    cout<<"Eliminando la posicion 5"<<endl;

    v_al = fr.load();
    
    for(int i = 0; i < v_al.size(); i++) {
        v_al[i].print();
    }

    Alumno al3{"A011", "Eva", "Campos", "BIO", 1, 1175.80};

    fr.Add(al3);

    cout<<endl;


    v_al = fr.load();
    
    for(int i = 0; i < v_al.size(); i++) {
        v_al[i].print();
    }
}


void test2(){
    FixedRecord fr("data.bin" , FREE_LIST);
    Alumno alumnos[10] = {
        {"A001", "Ana", "Perez", "INF", 1, 1200.50},
        {"A002", "Luis", "Gomez", "ADM", 2, 1100.00},
        {"A003", "Mia", "Lopez", "DER", 3, 1300.75},
        {"A004", "Juan", "Diaz", "MED", 4, 1400.25},
        {"A005", "Sara", "Mora", "PSI", 1, 1250.00},
        {"A006", "Leo", "Vega", "ARQ", 2, 1350.45},
        {"A007", "Rita", "Solis", "CIV", 3, 1450.60},
        {"A008", "Tito", "Luna", "IND", 4, 1550.20},
        {"A009", "Lola", "Nuez", "INF", 1, 1150.30},
        {"A010", "Paco", "Rio", "ADM", 2, 1050.90}
    };

    for(int i = 0; i < 10; i++) {
        fr.Add(alumnos[i]);
    }
    
    vector<Alumno> v_al = fr.load();
    
    for(int i = 0; i < v_al.size(); i++) {
        v_al[i].print();
    }
    
    int pos = 5;
    
    fr.DeleteRecord(pos);

    cout<<end<<"Eliminando la posicion 5"<<end;

    v_al = fr.load();
    
    for(int i = 0; i < v_al.size(); i++) {
        v_al[i].print();
    }

    cout<<"El head es : "<<fr.get_header()<<end;


    pos = 3;
    
    fr.DeleteRecord(pos);

    cout<<end<<"Eliminando la posicion 3"<<end;

    v_al = fr.load();
    
    for(int i = 0; i < v_al.size(); i++) {
        v_al[i].print();
    }

    
    cout<<"El head es : "<<fr.get_header()<<end;


    cout<<"Insertando un nuevo registro"<<end;

    Alumno al3{"A011", "Eva", "Campos", "BIO", 1, 1175.80};

    fr.Add(al3);

    cout<<endl;


    v_al = fr.load();
    
    for(int i = 0; i < v_al.size(); i++) {
        v_al[i].print();
    }

    cout<<"El head es : "<<fr.get_header()<<end;

    
}



int main() {
    
    test2();
    
    return 0;
}
