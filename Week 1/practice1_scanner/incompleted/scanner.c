/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

 

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

// SKIP ALL ERRORs
// TO-DO

void skipBlank() {
  while (currentChar!=EOF &&charCodes[currentChar]==CHAR_SPACE)
    readChar();
}

void skipComment() {
  while (1){
    readChar();
    if (currentChar == EOF)
      error(ERR_ENDOFCOMMENT,lineNo,colNo);

    if (charCodes[currentChar]==CHAR_TIMES){
      // If EOF -> error
      readChar();
      if (currentChar == EOF)   
          error(ERR_ENDOFCOMMENT,lineNo,colNo);
      if (charCodes[currentChar]==CHAR_SLASH){     
        readChar();
        return;
      }
    }

  }
  
}

Token* readIdentKeyword(void) {
  int i=0;
  Token* token = makeToken(TK_IDENT,lineNo,colNo);
  char string[255]; // this will read a maximum 255 letters
  while(charCodes[currentChar]==CHAR_LETTER||charCodes[currentChar]==CHAR_UNDERSCORE||charCodes[currentChar]==CHAR_DIGIT){
    // add to string of token if it's a letter / digit / underscore
    string[i]= currentChar;
    i+=1;
    readChar();
  }
  //end of string but take only the first 15
  if (i>15)
    string[15]='\0';
  else
    string[i]='\0';
  
  strcpy(token->string,string); 

  //check keyword
  TokenType tokenType = checkKeyword(token->string);

  int check = 0; // check = 1 means there is a letter that is not in UPPERCASE 
  if (tokenType != TK_NONE){
    // check if all keywords are in upper 
    for (int i=0;i<strlen(token->string);i++)
      if (!isupper(token->string[i])){
        check=1;
        break;
      }    
    if (check == 0)
      token->tokenType=tokenType; // keyword
    else {
      // ident
      // case insensitive
      for (int i=0;i<strlen(token->string);i++){
        token->string[i] = tolower(token->string[i]); 
      } // all identifer are in lower 
    }
  }
  return token;
}

Token* readNumber(void) {
  int i=0;
  Token* token = makeToken(TK_NUMBER,lineNo,colNo);
  
  while (charCodes[currentChar]==CHAR_DIGIT){
    // add to string if currentChar is a digit
    token->string[i]=currentChar;
    i+=1;
    readChar();
    if (currentChar == EOF) break;
    if (i>10){
      error(ERR_NUMBERTOOLARGE,lineNo,colNo);
    }
  }
  // end of string
  token->string[i]='\0';
  // string to value in Token
  int value = atoi(token->string);
  if (value < INT_MAX) {
    token->value=value;
  }
  else {
      // ERROR WHEN NUMBER TO LARGE
      token->tokenType = TK_NONE;
      error(ERR_NUMBERTOOLARGE,lineNo,colNo);
      token->value = -1;
  }
  return token;
}

Token* readConstChar(void) {
  int i=0;
  Token* token = makeToken(TK_CHAR,lineNo,colNo);
  
  readChar();
  while (1){
    if (charCodes[currentChar]==CHAR_SINGLEQUOTE){
      readChar();
      // case 1: next is a another single quote, we have ''
      if (charCodes[currentChar]==CHAR_SINGLEQUOTE){
        token->string[i]=currentChar;
      }
      // case 2: next is not a single quote, break the loop
      else{
        break;
      }
    }

    if (isprint(currentChar)){
      // add to string if printable
        token->string[i]=currentChar;
      } 
      else 
        error(ERR_INVALIDCHARCONSTANT,lineNo,colNo);
    
      i+=1;
      readChar();
      if ( currentChar ==EOF)
        break;

  }
  token->string[i] = '\0';
  if (i>255)
    error(ERR_INVALIDCHARCONSTANT,lineNo,colNo);
  // now we have a string begin with ' and end with ' || string not complete due to end of file 

  // if length > 1 it's a token string
  if (i>1){
    token->tokenType=TK_STRING;
  }
  readChar();
  return token;
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar(); 
    if (charCodes[currentChar]== CHAR_TIMES){
      free(token);
      skipComment();
      return getToken();
    }
    else
      return token;
  case CHAR_LT:
    token = makeToken(TK_NONE, lineNo,colNo);
    readChar();
    if (charCodes[currentChar]==CHAR_EQ){
      token->tokenType=SB_LE;
      readChar();
      return token;
    }
    else {
      token->tokenType=SB_LT;
      return token;
    }
  case CHAR_GT:
    token = makeToken(TK_NONE, lineNo,colNo);
    readChar();
    if (charCodes[currentChar]==CHAR_EQ){
      token->tokenType=SB_GE;
      readChar();
      return token;
    }
    else {
      token->tokenType=SB_GT;
      return token;
    }
  case CHAR_EXCLAIMATION:
    token = makeToken(TK_NONE,lineNo,colNo);
    readChar();
    if (charCodes[currentChar]==CHAR_EQ){
      token->tokenType=SB_NEQ;
    }
    else {
       error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
    }
    return token;
  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_PERIOD:
    token = makeToken(TK_NONE, lineNo,colNo);
    readChar();
    
    token->tokenType=SB_PERIOD;
    return token;
  case CHAR_COLON:
  token = makeToken(TK_NONE, lineNo,colNo);
    readChar();
    if (charCodes[currentChar]==CHAR_EQ){
      token->tokenType=SB_ASSIGN;
      readChar();
      return token;
    }
    else {
      token->tokenType=SB_COLON;
      return token;
    }
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_SINGLEQUOTE:
  
    return readConstChar();
  case CHAR_LPAR:
    token = makeToken(TK_NONE,lineNo,colNo);
    readChar();
    
    token->tokenType=SB_LPAR;
    return token;
    
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_UNDERSCORE: 
    return readIdentKeyword();
  case CHAR_LBRACKET:
    token = makeToken(SB_LSEL, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_RBRACKET:
    token = makeToken(SB_RSEL, lineNo, colNo);
    readChar(); 
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;
  case TK_STRING: printf("TK_STRING(\'%s\')\n",token->string); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



