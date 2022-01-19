%{
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
  
using namespace std;  
  
int token;
int linha = 1, coluna_atual = 1, coluna_anterior = 0;
string lexema;

void A();
void E();
void E_linha();
void T();
void T_linha();
void F();
void casa( int );
void print();

enum { tk_int = 256, tk_char, tk_double, ID, 
       NUM, STRING };

map<int,string> nome_tokens = {
  { tk_int, "int" },
  { tk_char, "char" },
  { tk_double, "double" },
  { ID, "nome de identificador" },
  { NUM, "constante inteira" },
  { STRING, "string" }
};

int tk( int token );

%}

DIGITO	[0-9]
LETRA	[A-Za-z_]

NUM	{DIGITO}+
ID	{LETRA}({LETRA}|{DIGITO})*

ASPAS_DUPLAS    (\"((\"{2})*(\\.|[^"\\\n])*)*\")
ASPAS_SIMPLES   (\'((\'{2})*(\\.|[^'\\\n])*)*\')
STRING  {ASPAS_SIMPLES}|{ASPAS_DUPLAS}

%%

" "  		{ coluna_anterior = coluna_atual++; }
"\t"		{ coluna_anterior = coluna_atual; 
                  coluna_atual += 2; }
"\n"            { linha++; 
                  coluna_anterior = coluna_atual;
                  coluna_atual = 1; }
{NUM} 		{ return tk( NUM ); }
"char"		{ return tk( tk_char ); }
"int"		{ return tk( tk_int ); }
"double"	{ return tk( tk_double ); }

{ID}		{ return tk( ID ); }

.		{ return tk( yytext[0] ); }

%%

int tk( int token ) {
  coluna_anterior = coluna_atual;
  coluna_atual += strlen( yytext ); 
  return token;
}

int next_token() {
  
  static int look_ahead = ' ';
  
  while( look_ahead == ' ' || look_ahead == '\n' || look_ahead == '\r' || look_ahead == '\t' )
     look_ahead = getchar();
  
  //apagar
  //cout << "vou analisar" << look_ahead << endl;

  if( isdigit( look_ahead ) ) {
    //apagar
    //cout << "é digito" << endl;
    lexema = (char) look_ahead;
    //apagar
    //cout << lexema << endl;
    look_ahead = getchar();
    return NUM;
  }
  
  if( isalpha( look_ahead ) ) {
    //apagar
    //cout << "é alpha" << endl;
    lexema = (char) look_ahead;
    
    look_ahead = getchar();
    while( isalpha( look_ahead ) ) {
      lexema += (char) look_ahead;
    
      look_ahead = getchar();   
    }
    //apagar
    //cout << "primeira parte" << lexema << endl;
    return ID;
  }

  if(look_ahead == '\"'){
    //apagar
    //cout << "é string" << endl;
    lexema = (char) look_ahead;
    
    look_ahead = getchar();
    while( look_ahead != '\"' ) {
      lexema += (char) look_ahead;
    
      look_ahead = getchar();   
    }
    lexema += (char) look_ahead;
    return STRING;
  }

  switch( look_ahead ) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '=':
    case '@':
    case '#':
      int temp = look_ahead;
      look_ahead = getchar();
      return temp;
  }
  
  return EOF;
}

void erro( string msg ) {
  cout << "*** Erro: ***" << endl
       << "Linha: " << linha << ", coluna: " << coluna_anterior << endl
       << msg << endl;
  exit( 1 );
}

void print( string msg ) {
  cout << msg << " ";
}

string nome_token( int token ) {
  if( nome_tokens.find( token ) != nome_tokens.end() )
    return nome_tokens[token];
  else {
    string r;
    r = (char) token;
    
    return "'" + r + "'";
  }
}

void A() {
// Guardamos o lexema pois a função 'casa' altera o seu valor.
//apagar
  //cout << "cheguei no A" << endl;
  string temp = lexema; 
  casa( ID );
  print( temp );
  casa( '=' );
  E();
  print( "=" );
}

void E() {
  T();
  E_linha();
}

void E_linha() {
  switch( token ) {
    case '+' : casa( '+' ); T(); print( "+"); E_linha(); break;
    case '-' : casa( '-' ); T(); print( "-"); E_linha(); break;
  }
}

void T() {
  F();
  T_linha();
}

void T_linha() {
  switch( token ) {
    case '*' : casa( '*' ); F(); print( "*"); T_linha(); break;
    case '/' : casa( '/' ); F(); print( "/"); T_linha(); break;
  }
}

void F() {
  //cout << "cheguei no F com o token: " << token << endl; 
  switch( token ) {
    case ID : {
      string temp = lexema;
      casa( ID ); print( temp + "@" ); } 
      break;
    case NUM : {
      string temp = lexema;
      casa( NUM ); print( temp ); }
      break;
    case STRING : {
      string temp = lexema;
      casa( STRING ); print( temp ); }
      break;
    case '(': 
      casa( '(' ); E(); casa( ')' ); break;
    default:
      erro( "Operando esperado, encontrado " + lexema );
  }
}

void casa( int esperado ) {
  if( token == esperado )
    token = next_token();
  else 
    erro( "Esperado " + nome_token( esperado ) +
	  ", encontrado: " + nome_token( token ) );
}


auto f = &yyunput;

int main() {
  //cout << "teste isalpha: " << isalpha('"') << endl;
  token = next_token();
  A();
  // if( token == 0 )
  //   cout << "Sintaxe ok!" << endl;
  // else
  //   erro( "Caracteres encontrados após o final do programa" );
  
  return 0;
}