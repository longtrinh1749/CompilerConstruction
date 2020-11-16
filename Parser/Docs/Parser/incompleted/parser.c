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
// Prog ::= KW_PROGRAM TK_IDENT SB_SEMICOLON Block SB_PERIOD
void compileProgram(void) {
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}
// Block ::= KW_CONST ConstDecl ConstDecls Block2
// Block ::= Block2
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
// Block2 ::= KW_TYPE TypeDecl TypeDecl Block3
// Block2 ::= Block3
void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}
// Block3 ::= KW_VAR VarDecl VarDecls Block4
// Block3 ::= Block4
void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}
// Block4 ::= SubDecls Block5
void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}
// Block5 ::= KW_BEGIN Statements KW_END
void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}
// ConstDecls ::= ConstDecl ConstDecls
// ConstDecls ::= epsilon
void compileConstDecls(void) {
  while (lookAhead->tokenType==TK_IDENT) 
    compileConstDecl(); 
}
// ConstDecl ::= TK_IDENT SB_EQUAL Constant SB_SEMICOLON
void compileConstDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}
// TypeDecls ::= TypeDecl TypeDecls
// TypeDecls ::= epsilon
void compileTypeDecls(void) {
  // TODO
  while(lookAhead->tokenType==TK_IDENT) {
    compileTypeDecl();
  }
}
// TypeDecl ::= TK_IDENT SB_EQUAL Type SB_SEMICOLON
void compileTypeDecl(void) {
  // TODO
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}
// VarDecls ::= VarDecl VarDecls
// VarDecls ::= epsilon
void compileVarDecls(void) {
  // TODO
  while(lookAhead->tokenType == TK_IDENT) {
    compileVarDecl();
  }
  // if(lookAhead->tokenType == TK_IDENT) {
  //   compileVarDecl();
  //   compileVarDecls();
  // }
}
// VarDecl ::= TK_IDENT SB_COLON Type SB_SEMICOLON
void compileVarDecl(void) {
  // TODO
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}
// SubDecls ::= FunDecl SubDecls
// SubDecls ::= ProDecl SubDecls
void compileSubDecls(void) {
  assert("Parsing subtoutines ....");
  // TODO
  while(lookAhead->tokenType == KW_FUNCTION || lookAhead->tokenType == KW_PROCEDURE) {
    if(lookAhead->tokenType == KW_FUNCTION) {
      compileFuncDecl();
    } else if(lookAhead->tokenType == KW_PROCEDURE) {
      compileProcDecl();
    }
  }
  assert("Subtoutines parsed ....");
}
// FunDecl ::= KW_FUNCTION TK_IDENT Params SB_COLON BasicType SB_SEMICOLON Block SB_SEMICOLON
void compileFuncDecl(void) {
  assert("Parsing a function ....");
  // TODO
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
// ProcDecl ::= KW_PROCEDURE TK_IDENT Params SB_SEMICOLON Block SB_SEMICOLON
void compileProcDecl(void) {
  assert("Parsing a procedure ....");
  // TODO
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}
// UnsignedConstant ::= TK_NUMBER
// UnsignedConstant ::= TK_IDENT
// UnsignedConstante ::= TK_CHAR
void compileUnsignedConstant(void) {
  // TODO
  switch(lookAhead->tokenType) {
    case TK_NUMBER:
      eat(TK_NUMBER);
      break;
    case TK_IDENT:
      eat(TK_IDENT);
      break;
    case TK_CHAR:
      eat(TK_CHAR);
      break;
    default:
      error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }
}
// Constant ::= SB_PLUS Constant2
// Constant ::= SB_MINUS Constant2
// Constant ::= Constant2
// Constant ::= TK_CHAR
void compileConstant(void) {
  // TODO
  switch(lookAhead->tokenType) {
    case SB_PLUS:
      eat(SB_PLUS);
      compileConstant2();
      break;
    case SB_MINUS:
      eat(SB_MINUS);
      compileConstant2();
      break;
    case TK_CHAR:
      eat(TK_CHAR);
      break;
    case TK_IDENT:
      compileConstant2(); 
      break;
    case TK_NUMBER:
      compileConstant2(); 
      break;
    default:
      error(ERR_INVALIDCONSTANT, lookAhead -> lineNo, lookAhead -> colNo);
  }
}
// Constant2 ::= TK_IDENT
// Constant2 ::= TK_NUMBER
void compileConstant2(void) {
  // TODO
  switch (lookAhead->tokenType) {
    case TK_IDENT:
      eat(TK_IDENT); 
      break;
    case TK_NUMBER:
      eat(TK_NUMBER); 
      break;
    default:
      error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }
}
// Type ::= KW_INTEGER
// Type ::= KW_CHAR
// Type ::= TK_IDENT
// Type ::= KW_ARRAY SB_LSEL TK_NUMBER SB_RSEL KW_OF Type
void compileType(void) {
  // TODO
  switch(lookAhead->tokenType) {
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
    case SB_SEMICOLON:
      break;
    default:
      error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
  }
}
// BasicType ::= KW_INTEGER
// BasicType ::= KW_CHAR
void compileBasicType(void) {
  // TODO
  if(lookAhead->tokenType == KW_INTEGER) {
    eat(KW_INTEGER);
  } else if(lookAhead->tokenType == KW_CHAR) { 
    eat(KW_CHAR);
  } else {
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
  }
}
// Params ::= SB_PLPAR Param Params2 SB_RPAR
// Params ::= epsilon
void compileParams(void) {
  // TODO
  if(lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
  }
}
// Params2 ::= SB_SEMICOLON Param Params2
// Params2 ::= epsilon
void compileParams2(void) {
  // TODO
  while(lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileParam();
  }
  // if(lookAhead->tokenType == SB_SEMICOLON) {
  //   eat(SB_SEMICOLON);
  //   compileParam();
  //   compileParams2(); 
  // }
}
// Param ::= TK_IDENT SB_COLON BasicType
// Param ::= KW_VAR TK_IDENT SB_COLON BasicType
void compileParam(void) {
  // TODO
  switch(lookAhead->tokenType) {
    case TK_IDENT:
      eat(TK_IDENT);
      eat(SB_COLON);
      compileBasicType();
      break;
    case KW_VAR:
      eat(KW_VAR);
      eat(TK_IDENT);
      eat(SB_COLON);
      compileBasicType();
      break;
  }
}
// Statements ::= Statement Statements2
void compileStatements(void) {
  // TODO
  compileStatement();
  compileStatements2();
}
// Statements2 ::= KW_SEMICOLON Statement Statements2
// Statements ::= epsilon
void compileStatements2(void) {
  // TODO
  // printf("statements2\n");
  while(lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
  }
  // if(lookAhead->tokenType == SB_SEMICOLON) {
  //   eat(SB_SEMICOLON);
  //   compileStatement();
  //   compileStatements2();
  // }
  // compileStatements2();
}
// Statement ::= AssignSt
// Statement ::= CallSt
// Statement ::= GroupSt
// Statement ::= IfSt
// Statement ::= WhileSt
// Statement ::= ForSt
// Statement ::= epsilon
void compileStatement(void) {
  // printf("statement\n");
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
// AssignSt ::= Variable SB_ASSIGN Expression
void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  // TODO
  compileVariable();
  eat(SB_ASSIGN);
  compileExpression();
  assert("Assign statement parsed ....");
}
// CallSt ::= KW_CALL ProcedureIdent Arguments
void compileCallSt(void) {
  assert("Parsing a call statement ....");
  // TODO
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  assert("Call statement parsed ....");
}
// GroupSt ::= KW_BEGIN Statements KW_END
void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  // TODO
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}
// IfSt ::= KW_IF Condition KW_THEN Statement ElseSt
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
// ElseSt ::= KW_ELSE Statement
// ElseSt ::= epsilon
void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}
// WhileSt ::= KW_WHILE Condition KW_DO Statement
void compileWhileSt(void) {
  assert("Parsing a while statement ....");
  // TODO
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}
// ForSt ::= KW_FOR TK_IDENT SB_ASSIGN Expression KW_TO Expression KW_DO Statement
void compileForSt(void) {
  assert("Parsing a for statement ....");
  // TODO
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
// Arguments ::= SB_LPAR Expression Arguments2 SB_RPAR
// Arguments ::= epsilon
void compileArguments(void) {
  // TODO
  if(lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileExpression();
    compileArguments2();
    eat(SB_RPAR);
  }
}
// Arguments2 ::= SB_COMMA Expression Arguments2
// Arguments2 ::= epsilon
void compileArguments2(void) {
  // TODO
  while(lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
  }
  // if(lookAhead->tokenType == SB_COMMA) {
  //   eat(SB_COMMA);
  //   compileExpression();
  //   compileArguments2();
  // }
}
// Condition ::= Expression Condition2
void compileCondition(void) {
  // TODO
  compileExpression();
  compileCondition2();
}
// Condition2 ::= SB_EQ Expression
// Condition2 ::= SB_NEQ Expression
// Condition2 ::= SB_LE Expression
// Condition2 ::= SB_LT Expression
// Condition2 ::= SB_GE Expression
// Condition2 ::= SB_GT Expression
void compileCondition2(void) { // *************************
  // TODO
  switch(lookAhead->tokenType) {
    case SB_EQ:
    case SB_NEQ:
    case SB_LE:
    case SB_LT:
    case SB_GE:
    case SB_GT:
      eat(lookAhead->tokenType);
      break;
  }
  compileExpression();
}
// Expression ::= SB_PLUS Expression2
// Expression ::= SB_MINUS Expression2
// Expression ::= Expression2
void compileExpression(void) {
  assert("Parsing an expression");
  // TODO
  switch(lookAhead->tokenType) {
    case SB_PLUS:
    case SB_MINUS:
      eat(lookAhead->tokenType);
  }
  compileExpression2();
  assert("Expression parsed");
}
// Expression2 ::= Term Expression3
void compileExpression2(void) {
  // TODO
  // printf("expression2\n");
  compileTerm();
  compileExpression3();
}
// Expression3 ::= SB_PLUS Term Expression3
// Expression3 ::= SB_MINUS Term Expression3
// Expression3 ::= epsilon
void compileExpression3(void) {
  // TODO
  // printf("expression3\n");
  switch(lookAhead->tokenType) {
    case SB_PLUS:
    case SB_MINUS:
      eat(lookAhead->tokenType);
      compileTerm();
      compileExpression3();
      break;
      // Follow (statement)
    case SB_SEMICOLON:
    case KW_END:
    case KW_ELSE:
    // Follow (For statement)
    case KW_TO:
    case KW_DO:
    // Follow (arguments2)
    case SB_COMMA:
      printf("comma");
    // Follow (condition2)
    case SB_EQ:
    case SB_NEQ:
    case SB_LE:
    case SB_LT:
    case SB_GE:
    case SB_GT:
    // Follow (factor)
    case SB_RPAR:
    // Follow (indexes)
    case SB_RSEL:
    // Follow (if statement)
    case KW_THEN:
      break;
  // Error
    default:
      error(ERR_INVALIDEXPRESSION, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}
// Term ::= Factor Term2
void compileTerm(void) {
  // TODO
  // printf("term\n");
  compileFactor();
  compileTerm2();
}
// Term2 ::= SB_TIMES Factor Term2
// Term2 ::= SB_SLASH Factor Term2
// Term2 ::= epsilon
void compileTerm2(void) { // **********************
  // TODO
  // printf("term2\n");
  switch(lookAhead->tokenType) {
    case SB_TIMES:
    case SB_SLASH:
      eat(lookAhead->tokenType);
      compileFactor();
      compileTerm2();
    case SB_PLUS:
    case SB_MINUS:
    case KW_TO:
    case KW_DO:
    case SB_RPAR:
    case SB_COMMA:
    case SB_EQ:
    case SB_NEQ:
    case SB_LE:
    case SB_LT:
    case SB_GE:
    case SB_GT:
    case SB_RSEL:
    case SB_SEMICOLON:
    case KW_END:
    case KW_ELSE:
    case KW_THEN:
      break;
    default:
      error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}
// Factor ::= TK_NUMBER
// Factor ::= TK_CHAR
// Factor ::= TK_IDENT Indexes
// Factor ::= TK_IDENT Arguments
// Factor ::= SB_LPAR Expression SB_RPAR
void compileFactor(void) {
  // TODO
  // printf("factor\n");
  switch(lookAhead->tokenType) {
    case TK_NUMBER:
      eat(TK_NUMBER);
      break;
    case TK_CHAR:
      eat(TK_CHAR);
      break;
    case TK_IDENT:
      eat(TK_IDENT);
      compileIndexes();
      compileArguments();
      break;
    case SB_LPAR:
      eat(SB_LPAR);
      compileExpression();
      eat(SB_RPAR);
      break;
    default:
      error(ERR_INVALIDFACTOR, lookAhead -> lineNo, lookAhead -> colNo);
      break;
  }
}
// Indexes ::= SB_LSEL Expression SB_RSEL Indexes
// Indexes ::= epsilon
void compileIndexes(void) {
  // TODO
  while(lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
  }
  // if(lookAhead->tokenType == SB_LSEL) {
  //   eat(SB_LSEL);
  //   compileExpression();
  //   eat(SB_RSEL);
  //   compileIndexes();
  // }
}

//more function
void compileVariable(void) {
  eat(TK_IDENT);
  compileIndexes();
}

void compileFunctionApplication(void) {
  eat(TK_IDENT);
  compileArguments();
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
