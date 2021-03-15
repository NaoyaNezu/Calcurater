#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <setjmp.h>

#define SIZE 1024

int ch; //読み取る文字
double value; //数値データだったときの値
int token; //トークンの種類を格納
jmp_buf err;  // エラー脱出用

//トークンの種類
enum {Number,Add,Sub,Mul,Div,Lpar,Rpar,End,Others};

// トークン名
char *token_name[] = {
  "Number",
  "+",
  "-",
  "*",
  "/",
  "(",
  ")",
  "End",
  "Others",
};

void error(char *mes)
{
  fprintf(stderr, "%s, %s\n", mes, token_name[token]); //error出力
  longjmp(err, 1);
}


//次の文字を取得する関数
void nextch(void){
  ch = getchar();
}

//現在の参照文字を返す関数
int getch(void){
  return ch;
}


//文字データを数字データに変更
double get_number(){
  char buff[SIZE+1];
  char *err;
  int i = 0;
  //数字が続く限りbuffに代入
  while (isdigit(getch())) {
    buff[i++] = getch();
    nextch();
  }
  buff[i]='\0';

  //数字の文字列を数値に変換
  double val = strtod(buff,&err);
  if(*err != '\0')
    error("get_number:not Number\n");
  return val;
}

//トークンを取得
void get_token(void){
  while(isspace(getch())) nextch(); //現在が空白の場合次の文字を読み取る
  if(isdigit(getch())){
    token = Number;
    value = get_number();
  }
  else{
    switch(getch()){
    case '+':
      token=Add;
      nextch();
      break;
    case '-':
      token=Sub;
      nextch();
      break;
    case '*':
      token=Mul;
      nextch();
      break;
    case '/':
    case '%':
      token = Div;
      nextch();
      break;
    case '(':
      token = Lpar;
      nextch();
      break;
    case ')':
      token = Rpar;
      nextch();
      break;
    case '\n':
      token = End;
      break;
    default:
      token = Others;
      break;
    }
  }
}

double expression(void); //式
double term(void); //項
double factor(void); //因子

//式
double expression(void){
  double val = term(); //項の関数を呼び出し代入
  while(true){
    switch(token){
      case Add:
        get_token();
        val += term();
        break;
      case Sub:
        get_token();
        val -= term();
        break;
      default:
        return val;
    }
  }
}

//項
double term(void){
  double val = factor();
  while(true){
    switch(token){
      case Mul:
        get_token();
        val *= factor();
        break;
      case Div:
        get_token();
        val/= factor();
        break;
      default:
        return val;
    }
  }
}

//因子
double factor(void){
  if(ch == '\n' ) return value;
  switch(token){
    case Lpar:
      get_token();
      double val = expression();
      if(token == Rpar){
        if(ch != '\n')  get_token();
      }
      else
        error("')' expected");
      return val;
    case Number:
      get_token();
      return value;
    case Add:
      get_token();
      return factor();
    case Sub:
      get_token();
      return -factor();
    default:
      error("unexpected token");
      break;
  }
}

void parsing(void)
{
  double val = expression();
  if (ch == '\n' ) {
    printf("= %.14g\n> ", val);
  }
  else {
    error("invalid token");
  }
}

void main(){
  printf(">");
  nextch();
  while(true){
    get_token();
    if (token == End) break;
    parsing();
  }

}
