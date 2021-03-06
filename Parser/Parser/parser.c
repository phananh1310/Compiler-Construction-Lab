/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void) {
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}
/*
void compileSubDecls(void) {
  assert("Parsing subtoutines ....");
  while (lookAhead->tokenType==KW_FUNCTION||lookAhead->tokenType==KW_PROCEDURE){
    if (lookAhead->tokenType==KW_FUNCTION){
      compileFuncDecl();
    }
    else if(lookAhead->tokenType==KW_PROCEDURE){
      compileProcDecl();
    }
  }
  assert("Subtoutines parsed ....");
}
*/

// block 4 = funDecls block 5 || block 5
void compileBlock4(void) {
  if (lookAhead->tokenType==KW_FUNCTION)
    compileFuncDecls();
  compileBlock5();
}

// block 5 = prodecls block 6 || block 6
void compileBlock5(void){
  if (lookAhead->tokenType==KW_PROCEDURE)
    compileProcDecls();
  compileBlock6();
}

void compileFuncDecls(void) {
  assert("Parsing FunctionDecls ....");
  while (lookAhead->tokenType==KW_FUNCTION)
      compileFuncDecl();
  assert("FunctionDecls parsed ....");
}

void compileProcDecls(void) {
  assert("Parsing ProcedureDecls ....");
  while (lookAhead->tokenType==KW_PROCEDURE)
      compileProcDecl();
  assert("ProcedureDecls parsed ....");
}

void compileBlock6(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) {
while (lookAhead->tokenType==TK_IDENT) 
compileConstDecl(); 
}

void compileConstDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void) {
  while (lookAhead->tokenType==TK_IDENT) 
    compileTypeDecl(); 
}

void compileTypeDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void) {
  while (lookAhead->tokenType==TK_IDENT)
    compileVarDecl(); 
}

void compileVarDecl(void) {
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}



void compileFuncDecl(void) {
  assert("Parsing a function ....");
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Function parsed ....");
}

void compileProcDecl(void) {
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

// error handling using invalid const
void compileUnsignedConstant(void) {
  if(lookAhead->tokenType==TK_NUMBER)
    eat(TK_NUMBER);
  else if(lookAhead->tokenType==TK_IDENT)
    eat(TK_IDENT);
  else if(lookAhead->tokenType==TK_CHAR)
    eat(TK_CHAR);
  else
    error(ERR_INVALIDCONSTANT,lookAhead->lineNo,lookAhead->colNo);
}

// error handling using invalid const
void compileConstant(void) {
  if (lookAhead->tokenType==SB_MINUS){
    eat(SB_MINUS);
    compileConstant2();
  }
  else if (lookAhead->tokenType==SB_PLUS){
    eat(SB_PLUS);
    compileConstant2();
  }
  else if (lookAhead->tokenType==TK_CHAR){
    eat(TK_CHAR);
  }
  else {
    compileConstant2();
  }
}

void compileConstant2(void) {
  if (lookAhead->tokenType==TK_IDENT){
    eat(TK_IDENT);
  }
  else if (lookAhead->tokenType==TK_NUMBER){
    eat(TK_NUMBER);
  }
  else 
    error(ERR_INVALIDCONSTANT,lookAhead->lineNo,lookAhead->colNo);
}

// error handling using invalid type
void compileType(void) {
  switch(lookAhead->tokenType){
    case KW_INTEGER:
      eat(KW_INTEGER);
      break;
    case KW_CHAR:
      eat(KW_CHAR);
      break;
    case TK_IDENT:
      eat(TK_IDENT);
      break;
    case KW_ARRAY:
      eat(KW_ARRAY);
      eat(SB_LSEL);
      eat(TK_NUMBER);
      eat(SB_RSEL);
      eat(KW_OF);
      compileType();
      break;
    default:
       error(ERR_INVALIDTYPE,lookAhead->lineNo,lookAhead->colNo);
  }
}

// error handling using invalid basic type
void compileBasicType(void) {
  if (lookAhead->tokenType==KW_INTEGER)
    eat(KW_INTEGER);
  else if (lookAhead->tokenType==KW_CHAR)
    eat(KW_CHAR);
  else
    error(ERR_INVALIDBASICTYPE,lookAhead->lineNo,lookAhead->colNo);
}

void compileParams(void) {
  if (lookAhead->tokenType==SB_LPAR){
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
  }
}
// error invalid para when between 2 paras is not a semicolon
void compileParams2(void) {
  if (lookAhead->tokenType==SB_SEMICOLON){
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
  }
  // follow
  else if (lookAhead->tokenType==SB_RPAR){
  }
  else 
    error(ERR_INVALIDPARAM,lookAhead->lineNo,lookAhead->colNo);
}
// error invalid para when no kw var or ident in a ()
void compileParam(void) {
  if (lookAhead->tokenType==KW_VAR||lookAhead->tokenType==TK_IDENT){ 
    if(lookAhead->tokenType==KW_VAR){
    eat(KW_VAR);
    }
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  }
  else 
    error(ERR_INVALIDPARAM,lookAhead->lineNo,lookAhead->colNo);
} 

void compileStatements(void) {
  compileStatement();
  compileStatements2();
}

// invalid statement happend here
void compileStatements2(void) {
  if (lookAhead->tokenType==SB_SEMICOLON){
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
  }
  // check follow
  else if (lookAhead->tokenType==KW_END);
  else error(ERR_INVALIDSTATEMENT,lookAhead->lineNo,lookAhead->colNo);

}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  eat(TK_IDENT); // Variable ??
  compileIndexes();
  eat(SB_ASSIGN);
  compileExpression();
  assert("Assign statement parsed ....");
}

void compileCallSt(void) {
  assert("Parsing a call statement ....");
  eat(KW_CALL);
  eat(TK_IDENT); // Procedure Ident ??

  compileArguments();
  assert("Call statement parsed ....");
}

void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}

void compileIfSt(void) {
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  assert("Parsing a while statement ....");
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}

void compileForSt(void) {
  assert("Parsing a for statement ....");
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

// invalid argument 
void compileArguments(void) {
  switch (lookAhead->tokenType){
    case SB_LPAR:
      eat(SB_LPAR);
      compileExpression();
      compileArguments2();
      if (lookAhead->tokenType==SB_RPAR){
        eat(SB_RPAR);
      } else {
        error(ERR_INVALIDARGUMENTS,lookAhead->lineNo,lookAhead->colNo); // error too  mayy arguments
      }
      break;
    // check follow
    case KW_ELSE: break;
    case SB_SEMICOLON: break;
    case KW_END: break;
    case SB_TIMES: break;
    case SB_SLASH: break;
    case SB_PLUS: break;
    case SB_MINUS: break;
    case KW_TO: break;
    case KW_DO: break;
    case SB_RPAR: break;
    case SB_EQ: break;
    case SB_NEQ: break;
    case SB_LE: break;
    case SB_LT: break;
    case SB_GE: break;
    case SB_GT: break;
    case SB_RSEL: break;
    case KW_THEN: break;
    case SB_COMMA: break;
    default:
      error(ERR_INVALIDARGUMENTS,lookAhead->lineNo,lookAhead->colNo);
  }
}

// invalid argument happens here
void compileArguments2(void) {
  if (lookAhead->tokenType==SB_COMMA){
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
  }
  // follow
  else if (lookAhead->tokenType==SB_RPAR){
  }
  else
    error(ERR_INVALIDARGUMENTS,lookAhead->lineNo,lookAhead->colNo);
}

void compileCondition(void) {
  compileExpression();
  compileCondition2();
}

// invalid comparator happened here
void compileCondition2(void) {
  switch(lookAhead->tokenType){
    case SB_EQ:
      eat(SB_EQ);
      break;
    case SB_NEQ:
      eat(SB_NEQ);
      break;
    case SB_LE:
      eat(SB_LE);
      break;
    case SB_LT:
      eat(SB_LT);
      break;
    case SB_GE:
      eat(SB_GE);
      break;
    case SB_GT:
      eat(SB_GT);
      break;
    default:
     error(ERR_INVALIDCOMPARATOR,lookAhead->lineNo,lookAhead->colNo);
  }
  compileExpression();
}

void compileExpression(void) {
  assert("Parsing an expression");
  if (lookAhead->tokenType==SB_PLUS){
    eat(SB_PLUS);
  }
  else if (lookAhead->tokenType==SB_MINUS){
    eat(SB_MINUS);
  }
  compileExpression2();
  assert("Expression parsed");
}

void compileExpression2(void) {
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  if (lookAhead->tokenType==SB_PLUS){
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
  }
  else if (lookAhead->tokenType==SB_MINUS){
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
  }
}

void compileTerm(void) {
  compileFactor();
  compileTerm2();
}

// invalid term
void compileTerm2(void) {
  switch(lookAhead->tokenType){
    case SB_TIMES:
      eat(SB_TIMES);
      compileFactor();
      compileTerm2();
      break;
    case SB_SLASH: 
      eat(SB_SLASH);
      compileFactor();
      compileTerm2();
      break;
    // check follow a term since there is a terminate symbol (by going to where compileterm is called and find out what's after a term if term is end)
    // term is in the end of a expression -> see when expression called and what 's after an expression
    // example after compile expression is condition / argument ,...
    case SB_PLUS: break;
    case SB_MINUS: break;
    case KW_TO: break;
    case KW_DO: break;
    case SB_RPAR: break;
    case SB_EQ: break;
    case SB_NEQ: break;
    case SB_LE: break;
    case SB_LT: break;
    case SB_GE: break;
    case SB_GT: break;
    case SB_RSEL: break;
    case SB_SEMICOLON: break;
    case KW_ELSE: break;
    case KW_END: break;
    case KW_THEN: break;
    case SB_COMMA: break;
    default:
      error(ERR_INVALIDTERM,lookAhead->colNo,lookAhead->lineNo);
  }
}

// invalid factor when there is no factor
void compileFactor(void) {
  switch(lookAhead->tokenType){
    case TK_NUMBER:
      eat(TK_NUMBER);
      break;
    case TK_CHAR:
      eat(TK_CHAR);
      break;
    case TK_IDENT:
      eat(TK_IDENT);
      if (lookAhead->tokenType==SB_LSEL)
        compileIndexes();
      else if (lookAhead->tokenType==SB_LPAR)
        compileArguments();
      break;
    case SB_LPAR:
      eat(SB_LPAR);
      compileExpression();
      eat(SB_RPAR);
      break;
    default:
      error(ERR_INVALIDFACTOR,lookAhead->lineNo,lookAhead->colNo);
  }
}

void compileIndexes(void) {
  while (lookAhead->tokenType==SB_LSEL){
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
    compileIndexes();
  }
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
