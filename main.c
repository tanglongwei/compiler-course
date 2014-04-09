#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*
author:tanglongwei
Date:2014-4-9
*/
//记号的结构体定义
struct Token {
char * tokenName;
char * tokenAttr;
};

//从文件中取得在位置s与e之间的字符串，主要用于取得识别出的标识符,暂定保存在全局变量中
char m[20]={0};
char * getS(FILE *fp,int s,int e){
fseek(fp,s,0);
char a[e-s+1];
int i=0;
for(int m=1;m<=e-s;m++) {
    a[i++] = fgetc(fp);
}
a[e-s] = '\0';
strcpy(m,a);
return m;
}

//在识别出identifier之后，进一步判断是否为keyword,利用工具函数转化为小写之后比较
void translateToLowercase(char * origin) {
char *a = origin;
int i = 0;
while(a[i] != '\0'){
if(a[i]<97){
a[i] = a[i] + 32;
}
i++;
}
}
int isKeyword(struct Token temp) {
char b[20]={0};
char * name = temp.tokenAttr;
strcpy(b,name);
translateToLowercase(b);
if(!strcmp(b,"val") || !strcmp(b,"begin") || !strcmp(b,"else") || !strcmp(b,"end") ||
!strcmp(b,"if") || !strcmp(b,"then") || !strcmp(b,"real") || !strcmp(b,"integer")) {

return 1;

}else
return 0;



}



//判断是否为字母
int isLetter(char c) {
if(('A'<=c && c<='Z') || ('a'<=c && c<='z') )
    return 1;
else
    return 0;
}

//工具函数判断是否为0-9的数字
int isDigit(char c) {
if(c>='1' && c<='9')
    return 1;
else
    return 0;
}

//判断是否是'_'
int is_(char c) {
if( c == '_') {
    return 1;
}
    return 0;
}

int isArithmeticOperator(char c){
if(c=='+' || c=='-' || c=='*' || c=="/")
    return 1;
else
    return 0;
}


//判断记号是否被识别的工具函数
int IsTokenRecognized(struct Token temp) {
if(temp.tokenName == "other") return 0;
else
return 1;
}

//判断输入文件结束的函数
int fileEnd(FILE * fp,int position){
int current = ftell(fp);
if(current == position) {
    return 1;
}
    return 0;
}



//判断元素是否在整形数组中，返回为1在，返回0为不在
 int in(int element,int * array,int length) {
    for(int i=0;i<length;i++) {
      if( array[i] == element)
        return 1;
    }
    return 0;
  }

//赋值的move函数
int assignMove(int s,char c) {
    switch(s) {
        case 1 : switch(c) {
            case ':': return  2;
            default : return -1;

        }
       case 2 : switch(c) {
            case '=':return 3;
            default : return -1;

       }
    }
}
struct Token getAssign(FILE *);

//空格的move函数,似乎还要考虑换行的情况
int spacingMove(int s,char c) {
    switch(s) {
        case 1: { switch(c) {
            case ' ': return 2;break;
            case '\n': return 2;break;
            default : return -1;break;
        }break;}

        case 2 : { switch(c) {
            case ' ': return 2;break;
            case '\n': return 2;break;
            default : return 3;break;
        }break;}
        default: return -1;break;

    }

}
//判断spacing函数的声明
struct Token getSpacing(FILE *);

//只考虑了最简单的数字情形即123,89...，没考虑小数，指数的情况
int numberMove(int s,char c) {
switch(s) {
        case 1: {
            if(isDigit(c)) return 2;
            else return -1;
        }

        case 2 : {
            if(isDigit(c))return 2;
            else return 3;
        }}
    }

struct Token getNumber(FILE *);

//arithmetic operator move function算术运算符+-*/
int operatorMove(int s,char c) {
switch(s) {
        case 1: {
            if(isArithmeticOperator(c)) return 2;
            else return -1;
        }

        case 2 : {
            if(!isArithmeticOperator(c))return 3;
            else return -1;
        }}
    }

struct Token getOperator(FILE *);

//move函数的定义，参考transistion table,比较符的move函数
int comparisonMove(int s,char c) {
  switch(s) {
  case 0: { switch(c) {
    case '>' : return 6;break;
    case '=' : return 5;break;
    case '<' : return 1;break;
    default  : return -1;break;

      }break;}
  case 1: { switch(c) {
      case '>' : return 3; break;
      case '=' : return 2; break;
      default : return  4; break;
      }break;}

  case 6: { switch(c) {
      case '=' : return 7;break;

      default  : return 8;break;
      }
  }

    default: return -1;break;
  }
}
//判断比较符函数的声明
struct Token getComparison(FILE * );


//标识符的move函数
int idMove(int s,char c) {
    switch(s) {
        case 1: {
            if(isLetter(c)) return 2;
            else return -1;

        }
        case 2: {
            if(isLetter(c) || is_(c) || isDigit(c)) return 2;
            else return 3;

        }

    }
}

struct Token getId(FILE *);


int seperatorMove(int s,char c) {
  switch(s) {
  case 1: { switch(c) {
    case ';' : return 2;break;
    case '.' : return 3;break;
    default  : return -1;break;

      }break;}

  }
}
struct Token getSeperator(FILE * );

void main() {


  FILE * fp;//源文件指针
  FILE * output;

  fp=fopen("source-2","r");
  output = fopen("out","w+");
  if(fp == NULL) {
      printf("can't open this file \n");
      exit(0);
    }

    fseek(fp,-1L,2);
    int j = ftell(fp);

    rewind(fp);     //求出最后第二个字节的位置,eof占两个字节

    struct Token get={"",""};
  while(!fileEnd(fp,j)){  //feof()函数会在识别最后两个字节之后才返回1，而我不希望去解析那两个字节,便自己写了个函数

     int recognized = (IsTokenRecognized(get=getAssign(fp)) || IsTokenRecognized(get=getComparison(fp)) || IsTokenRecognized(get=getId(fp))
     || IsTokenRecognized(get=getNumber(fp)) || IsTokenRecognized(get=getOperator(fp)) || IsTokenRecognized(get=getSeperator(fp)) ||
     IsTokenRecognized(get=getSpacing(fp)));
     //如果无法识别的话，我希望文件位置指针前进一位(fgetc)，这样就不会产生因为无法识别之后回退而造成的无限循环的问题
     if(!recognized)
     {
        char a[2];
        a[0] = fgetc(fp);
        a[1] = '\0';
        get.tokenName = "unrecognized word";
        get.tokenAttr = a;
     }
     if(get.tokenName=="identifier" && isKeyword(get)){get.tokenName = "keyword";}
      fputs("<",output);
      fputs(get.tokenName,output);
      fputs(",",output);
      fputs(get.tokenAttr,output);
      fputs("> ",output);
      fputc('\n',output);
    }


  fclose(output);
  fclose(fp);



    return 0;
}

//判断是否是比较符函数的实现
struct Token getComparison(FILE * fp){
  int i = ftell(fp);//记录lexemeBegin位置
  int acceptingS[] = {2,3,4,5,7,8,-1};
  int s=0;
  while(!in(s,acceptingS,7)) {
    char c=fgetc(fp);
    s=comparisonMove(s,c);
  }
  switch(s){
  case -1:
    fseek(fp,i,0);   //重置文件位置指针为lexemeBegin
    struct Token a ={"other",""};
    return a;

  case 2 :{
     struct Token f ={"comparison","<="};
    return f;
    }
  case 3 : {
    struct Token a = {"comparison","<>"};
    return a;
    }
  case 4: {
    fseek(fp,-1L,1);//回退一个位置，便于下一个词素分析
    struct Token a = {"comparison","<"};
    return a;
  }

  case 5: {
    struct Token a ={"comparison","="};
    return a ;
    }
  case 7:{
    struct Token a ={"comparison",">="};
    return a ;
    }
  case 8: {
    fseek(fp,-1L,1);//回退一个位置，便于下一个词素分析
    struct Token a ={"comparison",">"};
    return a;
    }

  }

}

//判断spacing的函数实现
struct Token getSpacing(FILE *fp){
int i = ftell(fp);
int acceptingS[] = {3,-1};
int s=1;
  while(!in(s,acceptingS,2)) {
    char c=fgetc(fp);
    s=spacingMove(s,c);
  }
  switch(s){
    case 3:{
        fseek(fp,-1L,1);
        struct Token a ={"spacing","null"};
        return a;
        }

    case -1:
    fseek(fp,i,0);   //重置文件位置指针为lexemeBegin
    struct Token a ={"other",""};
    return a;
}
}


struct Token getId(FILE *fp) {

int i = ftell(fp);
int acceptingS[] = {3,-1};
int s=1;
  while(!in(s,acceptingS,2)) {
    char c=fgetc(fp);
    s=idMove(s,c);
  }
  switch(s){
    case 3:{
        fseek(fp,-1L,1);
        int kk = ftell(fp);
        getS(fp,i,ftell(fp)); //在全局变量m中保存标识符,是否还需要在这加一个判断是否是关键字的步骤？
        struct Token a ={"identifier",m};
        return a;
        }

    case -1:
    fseek(fp,i,0);   //重置文件位置指针为lexemeBegin
    struct Token a ={"other",""};
    return a;
}
}



struct Token getNumber(FILE *fp){
int i = ftell(fp);
int acceptingS[] = {3,-1};
int s=1;
  while(!in(s,acceptingS,2)) {
    char c=fgetc(fp);
    s=numberMove(s,c);
  }
  switch(s){
    case 3:{
        fseek(fp,-1L,1);
        int kk = ftell(fp);  //便于调试的一步
        getS(fp,i,ftell(fp)); //在全局变量m中保存数字常量
        struct Token a ={"numberConstant",m};
        return a;
        }

    case -1:
    fseek(fp,i,0);   //重置文件位置指针为lexemeBegin
    struct Token a ={"other",""};
    return a;
}
}



struct Token getOperator(FILE *fp) {

int i = ftell(fp);
int acceptingS[] = {3,-1};
int s=1;
  while(!in(s,acceptingS,2)) {
    char c=fgetc(fp);
    s=operatorMove(s,c);
  }
  switch(s){
    case 3:{
        fseek(fp,-1L,1);
        int kk = ftell(fp);
        getS(fp,i,ftell(fp)); //在全局变量m中保存标识符,是否还需要在这加一个判断是否是关键字的步骤？
        struct Token a ={"arithmeticOperator",m};
        return a;
        }

    case -1:
    fseek(fp,i,0);   //重置文件位置指针为lexemeBegin
    struct Token a ={"other",""};
    return a;
}
}




struct Token getAssign(FILE *fp) {


int i = ftell(fp);
int acceptingS[] = {3,-1};
int s=1;
  while(!in(s,acceptingS,2)) {
    char c=fgetc(fp);
    s=assignMove(s,c);
  }
  switch(s){
    case 3:{

        int kk = ftell(fp);
        getS(fp,i,ftell(fp)); //在全局变量m中保存标识符,是否还需要在这加一个判断是否是关键字的步骤？
        struct Token a ={"assign",m};
        return a;
        }

    case -1:
    fseek(fp,i,0);   //重置文件位置指针为lexemeBegin
    struct Token a ={"other",""};
    return a;
}





}

struct Token getSeperator(FILE *fp) {

int i = ftell(fp);
int acceptingS[] = {2,3,-1};
int s=1;
  while(!in(s,acceptingS,3)) {
    char c=fgetc(fp);
    s=seperatorMove(s,c);
  }
  switch(s){
    case 2:{
        struct Token a ={"seperator",";"};
        return a;
        }
    case 3:{
        struct Token a ={"seperator","."};
        return a;
        }
    case -1:
    fseek(fp,i,0);   //重置文件位置指针为lexemeBegin
    struct Token a ={"other",""};
    return a;
}
}






