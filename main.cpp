#include <iostream>
#include <cctype>
#include <fstream>
using namespace std;
const int ACP = 99;
const int ERR=-1;
string token="";
string entrada="";
string lex="";
int idx;
int errors;
int line=1;
int column=1, lastColumn=0;
const int trans[22][15]={
    //   let dig  _  <    >   =   /   .   +,-,%,^  "   :  ;,( ) [ ]     *  sp,tab  nl
/*00*/ {  1,  2,  1,  7, 10, 12, 17, 15,       16,  5, 13,      15,    16,     0,  0},
/*01*/ {  1,  1,  1,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*02*/ {ACP,  2,ACP,ACP,ACP,ACP,ACP,  3,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*03*/ {ERR,  4,ERR,ERR,ERR,ERR,ERR,ERR,      ERR,ERR,ERR,     ERR,   ERR,   ERR,ERR},
/*04*/ {ACP,  4,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*05*/ {  5,  5,  5,  5,  5,  5,  5,  5,        5,  6,  5,       5,     5,     5,ERR},
/*06*/ {ACP,ACP,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*07*/ {ACP,ACP,ACP,ACP,  9,  8,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*08*/ {ACP,ACP,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*09*/ {ACP,ACP,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*10*/ {ACP,ACP,ACP,ACP,ACP, 11,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*11*/ {ACP,ACP,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*12*/ {ACP,ACP,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*13*/ {ACP,ACP,ACP,ACP,ACP, 14,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*14*/ {ACP,ACP,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*15*/ {ACP,ACP,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*16*/ {ACP,ACP,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP},
/*17*/ {ACP,ACP,ACP,ACP,ACP,ACP, 18,ACP,      ACP,ACP,ACP,     ACP,    19,   ACP,ACP},
/*18*/ { 18, 18, 18, 18, 18, 18, 18, 18,       18, 18, 18,      18,    18,    18,ACP},
/*19*/ { 19, 19, 19, 19, 19, 19, 19, 19,       19, 19, 19,      19,    20,    19, 19},
/*20*/ { 19, 19, 19, 19, 19, 19, 21, 19,       19, 19, 19,      19,    20,    19, 19},
/*21*/ {ACP,ACP,ACP,ACP,ACP,ACP,ACP,ACP,      ACP,ACP,ACP,     ACP,   ACP,   ACP,ACP}
};
const string palres[]={"constante", "decimal", "entero", "alfabetico", "logico", "funcion", "si", "regresa", "sino", "fin", "inicio", "para", "en", "rango",
"a", "incr", "decr", "iterar", "mientras", "haz", "opcion", "caso", "procedimiento", "imprime", "imprimenl", "lee", "programa", "interrumpe", "continua", "otro"};
const string opelog[]={"no", "o", "y"};
const string ctelog[]={"falso", "verdadero"};
bool esPalRes();
bool esOpelog();
bool esCtelog();
int colChar();
string lexico();
bool varDefinition();
void constant();
void var();
void proc();
void func();
void bloque();
void identi();
void error();
void prgm();
int varType();
string varType(int);
void llamadaFuncion();
void instruccion();
void expresionAritmetica();
void suma();
void multiplicacion();
void parentesis();
void potencia();
void signo();
void term();
void si();
void expresionLogica();
void y();
void o();
void no();
void iterar();
void para();
void haz();
void readFile(string);
bool esPalres(string p){
	for(int i=0; i<30; i++){
		if(palres[i]==p) return true;
	}
	return false;
}
bool esOpelog(string p){
	for(int i=0; i<3; i++){
		if(opelog[i]==p) return true;
	}
	return false;
}
bool esCtelog(string p){
	for(int i=0; i<2; i++){
		if(palres[i]==p) return true;
	}
	return false;
}
int colChar(char c){
    if( isalpha(c) ) return 0;
    if( isdigit(c) ) return 1;
    if( c=='_') return 2;
    if( c=='<') return 3;
    if( c=='>') return 4;
    if( c=='=') return 5;
    if( c=='/') return 6;
    if( c=='.') return 7;
    if( c=='+' || c=='-' || c=='%' || c=='^') return 8;
    if( c=='"') return 9;
    if( c==':') return 10;
    if( c==',' || c==';' || c=='(' || c==')' || c=='[' || c==']') return 11;
    if( c=='*') return 12;
    if( c==' ' || c=='\t') return 13;
    if( c=='\n') return 14;
    return -1;
}
string lexico(){
    string lexema="";
    int estado=0, estAnt=0;
    while(estado != ERR && estado != ACP && (unsigned int)idx < entrada.length() ){
		char c = entrada[idx++];
		column++;
		int col = colChar(c);
		if(col >= 0 && col <= 14) {
			estAnt = estado;
			estado=trans[estado][col];
			if(col==14){
                line++;
                lastColumn=column;
                column=1;
			}
			if(estado!=ERR && estado!=ACP && estado!=0) lexema+=c;
		}
	}
	if((estAnt==21 || estAnt==18) && estado==ACP){
		idx--;
		if(entrada[idx]=='\n'){
            line--;
            column=lastColumn;
		}
		else{
            column--;
		}
		estado=0;
		lexema="";
	}
	if(estado==ERR || estado==ACP){
        idx--;
        if(entrada[idx]=='\n'){
            line--;
            column=lastColumn;
        }
        else{
            column--;
        }
	}
	else estAnt=estado;
	token="";
	switch(estAnt){
		case 1:
		if(esPalres(lexema))
			token="PalRes";
		else if(esOpelog(lexema))
			token="OpeLog";
		else if(esCtelog(lexema))
			token="CteLog";
		else
			token="Identi";
		break;
		case 2: token="CteEnt"; break;
		case 4: token="CteDec"; break;
		case 6: token="CteAlf"; break;
		case 7: case 8: case 9: case 10: case 11: case 12: token="OpeRel"; break;
		case 13: case 15: token="Delimi"; break;
		case 14: token="OpeAsi"; break;
		case 16: case 17: token="OpeAri"; break;
		default: break;
	}
	if(estado==0) return lexico();
	return lexema;
}
bool varDefinition(){
    return lex=="entero" || lex == "decimal" || lex == "alfabetico" || lex == "logico";
}
void error(string type, string errDesc){
    cout << type << " (Linea " << line << ", Columna " << column << "): " << errDesc << endl;
    errors++;
}
void prgm(){
    lex=lexico();
    while(lex != "programa"){
        while(lex == "constante" || varDefinition()){
            if(lex=="constante"){
                lex = lexico();
                constant();
            }
            else{
                lex = lexico();
                if(lex != "funcion")
                    var();
                else break;
            }
        }
        while(lex == "procedimiento" || lex == "funcion"){
            if(lex == "procedimiento"){
                lex = lexico();
                proc();
            }
            else{
                lex = lexico();
                func();
            }
        }
    }
    if(lex != "programa"){
        error("Sintaxis", "Se esperaba 'programa', se detecto '" + lex + "'");
    }
    lex = lexico();
    if(lex != "inicio"){
        error("Sintaxis", "Se esperaba 'inicio', se detecto '" + lex + "'");
    }
    lex = lexico();
    bloque();
    if(lex != "fin")
        error("Sintaxis", "Se esperaba 'fin', se detecto '" + lex + "'");
    lex=lexico();
    if(lex != "programa")
        error("Sintaxis", "Se esperaba 'programa', se detecto '" + lex + "'");
    lex=lexico();
    if(lex != ".")
        error("Sintaxis", "Se esperaba '.', llego: " + lex);
}
int varType(){
    if(token == "CteEnt" || lex == "entero") return 1;
    if(token == "CteDec" || lex == "decimal") return 2;
    if(token == "CteAlf" || lex == "alfabetico") return 3;
    if(token == "CteLog" || lex == "logico") return 4;
    return 0;
}
string varType(int type){
    if(type == 1) return "entero";
    if(type == 2) return "decimal";
    if(type == 3) return "alfabetico";
    if(type == 4) return "logico";
    return "";
}
void constant(){
    int tipo, tipoVar;
    if(varDefinition()){
        tipo=varType();
    }
    else{
        error("Sintaxis", "'" + lex + "' no es un tipo");
    }
    lex = lexico();
    if(token != "Identi"){
        error("Sintaxis", "Se esperaba un identificador");
    }
    lex=lexico();
    if(token != "OpeAsi"){
        error("Sintaxis", "Se esperaba un inicializador, se detecto '"+ lex +"'");
    }
    lex=lexico();
    if(token=="CteEnt" || token=="CteDec" || token=="CteLog" || token=="CteAlf" || token=="Identi"){
        tipoVar=varType();
        if(tipo != tipoVar){
            string tipoDefinido, var;
            tipoDefinido=varType(tipo);
            var=varType(tipoVar);
            error("Semantico", "No se puede convertir de "+var+ " a "+tipoDefinido);
        }
        lex=lexico();
    }
    else{
        lex=lexico();
        error("Sintaxis", "Se esperaba una expresion antes de '"+lex+"'");
    }
    while(lex==","){
        lex=lexico();
        if(token != "Identi"){
            error("Sintaxis", "Se esperaba un identificador");
        }
        lex=lexico();
        if(token != "OpeAsi"){
            error("Sintaxis", "Se esperaba '=', se detecto '"+ lex +"'");
        }
        lex=lexico();
        if(token=="CteEnt" || token=="CteDec" || token=="CteLog" || token=="CteAlf" || token=="Identi"){
            tipoVar=varType();
            if(tipo != tipoVar){
                string tipoDefinido, var;
                tipoDefinido=varType(tipo);
                var=varType(tipoVar);
                error("Semantico", "No se puede convertir de "+var+ " a "+tipoDefinido);
            }
            lex=lexico();
        }
        else{
            error("Sintaxis", "Se esperaba una expresion antes de '"+lex+"'");
        }
    }
    if(lex!=";"){
        error("Sintaxis", "Se esperaba ';', se detecto '"+lex+"'");
    }
    lex=lexico();
}
void var(){
    if(token != "Identi"){
        error("Sintaxis", "Se esperaba un identificador, se detecto '"+lex+"'");
    }
    lex=lexico();
    while(lex=="["){
        lex=lexico();
        if(token != "Identi" && token != "CteEnt"){
            error("Sintaxis", "Se esperaba un identificador, se detecto '"+lex+"'");
        }
        lex=lexico();
        if(lex != "]"){
            error("Sintaxis", "Se esperaba ']', se detecto '"+lex+"'");
        }
        lex=lexico();
    }
    if(token=="OpeAsi"){
        lex=lexico();
        if(token!="CteEnt" && token!="CteDec" && token!="CteLog" && token!="CteAlf" && token!="Identi"){
            lex = lexico();
            error("Sintaxis", "Se esperaba una expresion antes de '"+lex+"'");
        }
        else
            lex=lexico();
    }
    while(lex==","){
        lex=lexico();
        if(token != "Identi"){
            error("Sintaxis", "Se esperaba un identificador, se detecto '"+lex+"'");
        }
        lex=lexico();
        while(lex=="["){
            lex=lexico();
            if(token != "Identi"){
                error("Sintaxis", "Se esperaba un identificador, se detecto '"+lex+"'");
            }
            lex=lexico();
            if(lex != "]"){
                error("Sintaxis", "Se esperaba ']', se detecto '"+lex+"'");
            }
            lex=lexico();
        }
        if(token=="OpeAsi"){
            lex=lexico();
            if(token!="CteEnt" && token!="CteDec" && token!="CteLog" && token!="CteAlf" && token!="Identi"){
                lex = lexico();
                error("Sintaxis", "Se esperaba una expresion antes de '"+lex+"'");
            }
            else lex=lexico();
        }
    }
    if(lex!=";"){
        error("Sintaxis", "Se esperaba ';', se detecto '"+lex+"'");
    }
    lex=lexico();
}
void proc(){
    if(token != "Identi"){
        error("Sintaxis", "Se esperaba un identificador, se detecto '"+lex+"'");
    }
    lex=lexico();
    if(lex != "("){
        error("Sintaxis", "Se esperaba '(', se detecto '"+lex+"'");
    }
    lex=lexico();
    while(lex!=")"){
        lex=lexico();
        if(token != "Identi"){
            error("Sintaxis", "Se esperaba un identificador, se detecto '"+lex+"'");
        }
        lex=lexico();
        if(lex!="," && lex!=")")
            error("Sintaxis", "Se esperaba ',', se detecto '"+lex+"'");
        else if(lex!=")")
            lex=lexico();
    }
    if(lex!=")"){
        error("Sintaxis", "Se esperaba ')', se detecto '"+lex+"'");
    }
    lex=lexico();
    while(varDefinition()){
        lex=lexico();
        var();
    }
    if(lex=="inicio"){
        lex=lexico();
        bloque();
        if(lex!="fin"){
            error("Sintaxis", "Se esperaba 'fin', se detecto '"+lex+"'");
        }
        lex=lexico();
        if(lex!=";"){
            error("Sintaxis", "Se esperaba ';', se detecto '"+lex+"'");
        }
        lex=lexico();
    }
}
void func(){
    if(token != "Identi"){
        error("Sintaxis", "Se esperaba un identificador, se detecto '"+lex+"'");
    }
    lex=lexico();
    if(lex != "("){
        error("Sintaxis", "Se esperaba '(', se detecto '"+lex+"'");
    }
    lex=lexico();
    while(lex!=")"){
        lex=lexico();
        if(token != "Identi"){
            error("Sintaxis", "Se esperaba un identificador, se detecto '"+lex+"'");
        }
        lex=lexico();
        if(lex!="," && lex!=")") error("Sintaxis", "Se esperaba ',', se detecto '"+lex+"'");
        else if(lex!=")") lex=lexico();
    }
    if(lex!=")"){
        error("Sintaxis", "Se esperaba ')', se detecto '"+lex+"'");
    }
    lex=lexico();
    while(varDefinition()){
        lex=lexico();
        var();
    }
    if(lex!="inicio"){
        error("Sintaxis", "Se esperaba 'inicio', se detecto '"+lex+"'");
    }
    lex=lexico();
    bloque();
    if(lex!="fin"){
        error("Sintaxis", "Se esperaba 'fin', se detecto '"+lex+"'");
    }
    lex=lexico();
    if(lex!=";"){
        error("Sintaxis", "Se esperaba ';', se detecto '"+lex+"'");
    }
    lex=lexico();
}
void identi(){
    if(lex == "["){
        lex=lexico();
        if(token != "Identi" && token!="CteEnt"){
            error("Sintaxis", "Se esperaba un identificador, se detecto " + lex);
        }
        lex=lexico();
        if(lex != "]"){
            error("Sintaxis", "Se esperaba un ']', se detecto " + lex);
        }
        lex=lexico();
    }
    else if(lex == "("){
        lex=lexico();
        llamadaFuncion();
    }
}
void instruccion(){
    if(token=="Identi"){
        lex=lexico();
        if(lex=="("){
            lex=lexico();
            llamadaFuncion();
        }
        else{
            while(lex=="["){
                lex=lexico();
                if(token=="Identi"){
                    lex=lexico(),
                    identi();
                }
                //if()
            }
            lex=lexico();
            if(token!="CteEnt" && token!="CteDec" && token!="CteLog" && token!="CteAlf" && token!="Identi"){
                error("Sintaxis", "Se esperaba una expresion, se detecto " + lex);
            }
            lex=lexico();
        }
    }
    else if(lex=="interrumpe"||lex=="continua"){
        lex=lexico();
        if(lex!=";"){
            error("Sintaxis", "Se esperaba ';', se detecto " + lex);
        }
        lex=lexico();
    }
    else if(lex=="regresa"){
        lex=lexico();
        expresionLogica();
        if(lex!=";"){
            error("Sintaxis", "Se esperaba ';', se detecto " + lex);
        }
        lex=lexico();
    }
}
void llamadaFuncion(){
    if(token!="Identi"){
        error("Sintactico", "Se esperaba un identificador, se detecto "+lex);
    }
    lex=lexico();
    while(lex == ","){
        lex=lexico();
        if(token!="Identi"){
            error("Sintaxis", "Se esperaba un identificador, se detecto " + lex);
        }
        lex=lexico();
    }
    if(lex != ")"){
        error("Sintaxis", "Se esperaba ')', se detecto " + lex);
    }
    lex=lexico();
}
void suma(){
    multiplicacion();
    while(lex=="+" || lex=="-"){
        lex=lexico();
        multiplicacion();
    }
}
void multiplicacion(){
    potencia();
    while(lex=="*" || lex=="/"){
        lex=lexico();
        potencia();
    }
}
void potencia(){
    signo();
    while(lex=="^"){
        lex=lexico();
        signo();
    }
}
void signo(){
    if(lex=="-"){
        lex=lexico();
    }
    term();
}
void term(){
    if(lex=="("){
        lex=lexico();
        suma();
        if(lex!=")"){
            error("Sintaxis", "Se esperaba ')', se detecto " + lex);
        }
        lex=lexico();
    }
    else if(token == "CteEnt" || token=="CteDec" || token=="CteAlf" || token=="CteLog"){
        lex=lexico();
    }
    else if(token=="Identi"){
        lex=lexico();
        identi();
    }
}
void si(){
    if(lex!="("){
        error("Sintaxis", "Se esperaba '(', se detecto" + lex);
    }
    lex=lexico();
    expresionLogica();
    if(lex!=")"){
        error("Sintaxis", "Se esperaba ')', se detecto "+lex);
    }
    lex=lexico();
    if(lex=="inicio"){
        lex=lexico();
        bloque();
        if(lex!="fin"){
            error("Sintaxis", "Se esperaba 'fin', se detecto "+lex);
        }
        lex=lexico();
        if(lex!=";"){
            error("Sintaxis", "Se esperaba ';', se detecto "+lex);
        }
        lex=lexico();
    }
    else{
        // instruccion(); Pendiente
        while(lex!=";")
            lex=lexico();
        lex=lexico();
    }
    if(lex=="sino"){
        lex=lexico();
        if(lex=="inicio"){
            lex=lexico();
            bloque();
            if(lex!="fin"){
                error("Sintaxis", "Se esperaba 'fin', se detecto "+lex);
            }
            lex=lexico();
            if(lex!=";"){
                error("Sintaxis","Se esperaba ';', se detecto " + lex);
            }
            lex=lexico();
        }
        else{
            while(lex!=";"){
                lex=lexico();
            }
            lex=lexico();
        }
    }
}
void expresionAritmetica(){
    suma();
}
void expresionLogica(){
    o();
}
void o(){
    y();
    while(lex=="o"){
        lex=lexico();
        y();
    }
}
void y(){
    no();
    while(lex=="y"){
        lex=lexico();
        no();
    }
}
void no(){
    if(lex=="no"){
        lex=lexico();
    }
    expresionAritmetica();
    if(token=="OpeRel"){
        lex=lexico();
        expresionAritmetica();
    }
}
void iterar(){
    if(lex!="mientras"){
        error("Sintaxis", "Se esperaba 'mientras', se detecto "+ lex);
    }
    lex=lexico();
    if(lex!="("){
        error("Sintaxis", "Se esperaba '(', se detecto "+ lex);
    }
    lex=lexico();
    expresionLogica();
    if(lex!=")"){
        error("Sintaxis", "Se esperaba ')', se detecto "+ lex);
    }
    lex=lexico();
    if(lex=="inicio"){
        lex=lexico();
        bloque();
        if(lex!="fin"){
            error("Sintaxis", "Se esperaba 'fin', se detecto "+ lex);
        }
        lex=lexico();
        if(lex!=";"){
            error("Sintaxis", "Se esperaba ';', se detecto "+ lex);
        }
        lex=lexico();
    }
}
void para(){
    if(token!="Identi"){
        error("Sintaxis", "Se esparaba un identificador, se detecto "+ lex);
    }
    lex=lexico();
    if(lex!="en"){
        error("Sintaxis", "Se esparaba 'en', se detecto "+ lex);
    }
    lex=lexico();
    if(lex!="rango"){
        error("Sintaxis", "Se esparaba 'en', se detecto "+ lex);
    }
    lex=lexico();
    expresionAritmetica();
    if(lex!="a"){
        error("Sintaxis", "Se esparaba 'a', se detecto "+ lex);
    }
    lex=lexico();
    expresionAritmetica();
    if(lex=="incr" || lex=="decr"){
        lex=lexico();
        if(token!="CteEnt" && token!="CteDec" && token!="Identi"){
            error("Sintaxis", "Se esperaba un identificador, se detecto " + lex);
        }
        else if(token=="Identi"){
            lex=lexico();
            identi();
        }
        else
            lex=lexico();
    }
    if(lex=="inicio"){
        lex=lexico();
        bloque();
        if(lex!="fin"){
            error("Sintactico", "Se esperaba fin, se detecto " + lex);
        }
        lex=lexico();
        if(lex!=";"){
            error("Sintactico", "Se esperaba fin, se detecto " + lex);
        }
        lex=lexico();
    }
    else if(lex=="para"){
        lex=lexico();
        para();
    }
    else if(lex=="si"){
        lex=lexico();
        si();
    }
}
void bloque(){
    while(lex!="fin"){
        if(lex=="si"){
            lex=lexico();
            si();
        }
        else if(lex=="inicio"){
            lex=lexico();
            bloque();
        }
        else if(lex=="para"){
            lex=lexico();
            para();
        }
        else if(lex=="iterar"){
            lex=lexico();
            iterar();
        }
        else if(lex=="haz"){
            lex=lexico();
            haz();
        }
        else lex=lexico();
    }
}
void haz(){
    if(lex!="opcion"){
        error("Sintaxis", "Se esperaba 'opcion', se detecto " + lex);
    }
    lex=lexico();
    if(lex!="("){
        error("Sintaxis", "Se esperaba '(', se detecto "+lex);
    }
    lex=lexico();
    expresionLogica();
    if(lex!=")"){
        error("Sintaxis", "Se esperaba ')', se detecto "+lex);
    }
    lex=lexico();
    if(lex!="inicio"){
        error("Sintaxis", "Se esperaba 'inicio', se detecto "+lex);
    }
    lex=lexico();
    while(lex=="caso"){
        lex=lexico();
        expresionLogica();
        if(lex!=":"){
            error("Sintaxis", "Se esperaba ':', se detecto " + lex);
        }
        lex=lexico();
        while(lex!="caso" && lex!="otro"){
            if(lex=="interrumpe"){
                lex=lexico();
                if(lex!=";"){
                    error("Sintaxis", "Se esperaba ':', se detecto " + lex);
                }
                lex=lexico();
            }
            else if(lex=="regresa"){
                lex=lexico();
                expresionLogica();
                if(lex!=";"){
                    error("Sintaxis", "Se esperaba ':', se detecto " + lex);
                }
                lex=lexico();
            }
            else{
                while(lex!=";"){
                    lex=lexico();
                }
                lex=lexico();
            }
        }
    }
    if(lex=="otro"){
        lex=lexico();
        if(lex!="caso"){
            error("Sintaxis", "Se esperaba 'caso', se detecto " + lex);
        }
        lex=lexico();
        if(lex!=":"){
            error("Sintaxis", "Se esperaba ':', se detecto " + lex);
        }
        lex=lexico();
        while(lex!="fin"){
            lex=lexico();
        }
    }
    if(lex!="fin"){
        error("Sintaxis", "Se esperaba 'fin', se detecto "+lex);
    }
    lex=lexico();
    if(lex!=";"){
        error("Sintaxis", "Se esperaba ';', se detecto "+lex);
    }
    lex=lexico();
}
void readFile(string filename){
    char c;
    ifstream f;
    f.open(filename);
    entrada="";
    while(!f.eof()){
        c=f.get();
        entrada+=c;
    }
    f.close();
}
int main(){
    string file;
    cout << "Archivo a compilar: ";
    getline(cin, file);
    readFile(file);
    prgm();
    if(!errors){
        cout << "Compilacion exitosa" << endl;
    }
    else cout << "Se detectaron " << errors << " errores" << endl;
}
