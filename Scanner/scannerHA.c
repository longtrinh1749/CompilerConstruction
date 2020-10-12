/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen - Nguyen Thi Hong Anh 20176679
 * @version 2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "charcode.h"
#include "error.h"
#include "reader.h"
#include "token.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE) {
    readChar();
  }
}

// skip (* ... *)
void skipComment() {
  if (currentChar == EOF) {
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
  } else if (charCodes[currentChar] == CHAR_TIMES) {
    readChar();
    if (currentChar == EOF) {
      error(ERR_ENDOFCOMMENT, lineNo, colNo);
    } else if (charCodes[currentChar] == CHAR_RPAR) {
      // *) reached, end of comment
      readChar();
      return;
    } else {
      // normal char
      skipComment();
    }
  } else {
    currentChar = readChar();
    skipComment();
  }
}

Token *readIdentKeyword() {
  Token *token = makeToken(TK_IDENT, lineNo, colNo);
  int cn = colNo;
  int ln = lineNo;
  int count = 0;
  int cnt = count;
  while (currentChar != EOF && (charCodes[currentChar] == CHAR_LETTER ||
                                charCodes[currentChar] == CHAR_DIGIT ||
                                charCodes[currentChar] == CHAR_UNDERSCORE)) {
    if (count < MAX_IDENT_LEN) {
      token->string[count] = (currentChar);
      count++;
      cnt++;
      readChar();
    } else {
      readChar();
      cnt++;
    }
  }
  token->string[count] = '\0';
  if (cnt > MAX_IDENT_LEN) {
    error(ERR_IDENTTOOLONG, ln, cn);
  } else {
    TokenType tempToken = checkKeyword(token->string);
    if (tempToken != TK_NONE) {
      token = makeToken(tempToken, ln, cn);
    }
  }
  return token;
}

// Does number has a limit in its number of digits
Token *readNumber(void) {
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int cn = colNo;
  int ln = lineNo;
  int count = 0;
  while (currentChar != EOF && (charCodes[currentChar] == CHAR_LETTER ||
                                charCodes[currentChar] == CHAR_DIGIT)) {
    if (count < MAX_IDENT_LEN && charCodes[currentChar] == CHAR_DIGIT) {
      token->string[count++] = currentChar;
    } else if (charCodes[currentChar] == CHAR_LETTER) {
      error(ERR_INVALIDNUMBER, lineNo, colNo);
    } else if (count >= MAX_IDENT_LEN) {
      error(ERR_NUMBERTOOLONG, ln, cn);
    }
    currentChar = readChar();
  }
  token->string[count] = '\0';
  token->value = atoi(token->string);
  return token;
}

Token *readOperator(TokenType tokenType, int lineNo, int colNo) {
  Token *token = makeToken(tokenType, lineNo, colNo);
  readChar();
  return token;
}
// syntax: '<a_character>'
Token *readConstChar(void) {
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  currentChar = readChar();
  if (currentChar == EOF || currentChar == '\n') {
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
  }

  token->string[0] = currentChar;
  token->string[1] = '\0';
  currentChar = readChar();

  // has to end with a single quote to confirm it is a constant char. Else
  // invalid
  if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    readChar();
    return token;
  } else {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
  }
}

int state;
Token *getToken(void) {
  Token *token;
  int ln, cn;
  int noError = 1;
  while (noError) {
    ln = lineNo;
    cn = colNo;
    switch (state) {
      case 0: 
        if (currentChar == EOF) {
          state = 37;
          break;
        }
        switch (charCodes[currentChar]) {
          case CHAR_SPACE:
            state = 1;
            break;
          case CHAR_LETTER:
            state = 8;
            break;
          case CHAR_DIGIT:
            state = 10;
            break;
          case CHAR_PLUS:
            state = 12;
            break;
          case CHAR_MINUS:
            state = 13;
            break;
          case CHAR_TIMES:
            state = 14;
            break;
          case CHAR_SLASH:
            state = 15;
            break;
          case CHAR_EQ:
            state = 16;
            break;
          case CHAR_SEMICOLON:
            state = 18;
            break;
          case CHAR_RPAR:
            state = 39;
            break;
          case CHAR_COMMA:
            state = 17;
            break;
          case CHAR_SINGLEQUOTE:
            state = 34;
            break;
          case CHAR_LT:
            state = 25;
            break;
          case CHAR_GT:
            state = 22;
            break;
          case CHAR_EXCLAIMATION:
            state = 28;
            break;
          case CHAR_PERIOD:
            state = 19;
            break;
          case CHAR_COLON:
            state = 31;
            break;
          case CHAR_LPAR:
            state = 2;
            break;
          default:
            state = 38;
            break;
        }
        break;
      case 1:
        skipBlank();
        state = 0;
        break;
      case 2:
        readChar();
        switch (charCodes[currentChar]) {
          case CHAR_PERIOD:
            state = 6;
            break;
          case CHAR_TIMES:
            state = 3;
            break;
          default:
            state = 7;
            break;
        }
        break;
      case 3:
        skipComment();
        state = 0;
        break;
      case 6:
        state = 0;
        return makeToken(SB_LSEL, ln, cn);
      case 7:
        state = 0;
        return makeToken(SB_LPAR, ln, cn);
      // do not need case 4 because it is already included as a part of case 3
      // case 5:
      //   state = 0 ;
      //   return makeToken(TK_EOF, lineNo, colNo);
      case 8:
        state = 0;
        return readIdentKeyword();
      case 10: 
        state = 0;
        return readNumber();
      case 12:
        state = 0;
        return readOperator(SB_PLUS, ln, cn);
      case 13:
        state = 0;
        return readOperator(SB_MINUS, ln, cn);
      case 14:
        state = 0;
        return readOperator(SB_TIMES, ln, cn);
      case 15: 
        state = 0;
        return readOperator(SB_SLASH, ln, cn);
      case 16:
        state = 0;
        return readOperator(SB_EQ, ln, cn);
      case 17:
        state = 0;
        return readOperator(SB_COMMA, ln, cn);
      case 18:
        state = 0;
        return readOperator(SB_SEMICOLON, ln, cn);
      case 19:
        readChar();
        if(charCodes[currentChar] == CHAR_RPAR){
          state = 0;
          return makeToken(SB_RSEL, ln, cn);
        } else {
          state = 0;
          return makeToken(SB_PERIOD, ln, cn);
        }
      case 22:
        readChar();
        if(charCodes[currentChar] == CHAR_EQ){  
          state = 0;
          return makeToken(SB_GE, ln, cn);
        } else {
          state = 0;
          return makeToken(SB_GT, ln, cn);
        }
      case 25:
       readChar();
        if(charCodes[currentChar] == CHAR_EQ){  
          state = 0;
          return makeToken(SB_LE, ln, cn);
        } else {
          state = 0;
          return makeToken(SB_LT, ln, cn);
        }
      case 28: 
        readChar();
        if(charCodes[currentChar] == CHAR_EQ){
          state = 0;
          return makeToken(SB_NEQ, ln, cn);
        } else {
          state = -1;   //error
          noError = 0;
        }
        break;
      case 31:
        readChar();
        state = 0;
        readChar();
        if(charCodes[currentChar] == CHAR_EQ){
          return makeToken(SB_ASSIGN, ln, cn);
        } else {
          return makeToken(SB_COLON, ln, cn);
        }
      case 34:
        state = 0;
        return readConstChar();
      case 37:
        state = 0;
        return makeToken(TK_EOF, ln, cn);
      case 39:
        state = 0;
        readChar();
        return makeToken(SB_RPAR, ln, cn);
      case 38:
        state = -1;
        noError = 0;
        break;
      
    }
  }
}
  /******************************************************************/

  void printToken(Token * token) {
    printf("%d-%d:", token->lineNo, token->colNo);

    switch (token->tokenType) {
      case TK_NONE:
        printf("TK_NONE\n");
        break;
      case TK_IDENT:
        printf("TK_IDENT(%s)\n", token->string);
        break;
      case TK_NUMBER:
        printf("TK_NUMBER(%s)\n", token->string);
        break;
      case TK_CHAR:
        printf("TK_CHAR(\'%s\')\n", token->string);
        break;
      case TK_EOF:
        printf("TK_EOF\n");
        break;
      case KW_PROGRAM:
        printf("KW_PROGRAM\n");
        break;
      case KW_CONST:
        printf("KW_CONST\n");
        break;
      case KW_TYPE:
        printf("KW_TYPE\n");
        break;
      case KW_VAR:
        printf("KW_VAR\n");
        break;
      case KW_INTEGER:
        printf("KW_INTEGER\n");
        break;
      case KW_CHAR:
        printf("KW_CHAR\n");
        break;
      case KW_ARRAY:
        printf("KW_ARRAY\n");
        break;
      case KW_OF:
        printf("KW_OF\n");
        break;
      case KW_FUNCTION:
        printf("KW_FUNCTION\n");
        break;
      case KW_PROCEDURE:
        printf("KW_PROCEDURE\n");
        break;
      case KW_BEGIN:
        printf("KW_BEGIN\n");
        break;
      case KW_END:
        printf("KW_END\n");
        break;
      case KW_CALL:
        printf("KW_CALL\n");
        break;
      case KW_IF:
        printf("KW_IF\n");
        break;
      case KW_THEN:
        printf("KW_THEN\n");
        break;
      case KW_ELSE:
        printf("KW_ELSE\n");
        break;
      case KW_WHILE:
        printf("KW_WHILE\n");
        break;
      case KW_DO:
        printf("KW_DO\n");
        break;
      case KW_FOR:
        printf("KW_FOR\n");
        break;
      case KW_TO:
        printf("KW_TO\n");
        break;

      case SB_SEMICOLON:
        printf("SB_SEMICOLON\n");
        break;
      case SB_COLON:
        printf("SB_COLON\n");
        break;
      case SB_PERIOD:
        printf("SB_PERIOD\n");
        break;
      case SB_COMMA:
        printf("SB_COMMA\n");
        break;
      case SB_ASSIGN:
        printf("SB_ASSIGN\n");
        break;
      case SB_EQ:
        printf("SB_EQ\n");
        break;
      case SB_NEQ:
        printf("SB_NEQ\n");
        break;
      case SB_LT:
        printf("SB_LT\n");
        break;
      case SB_LE:
        printf("SB_LE\n");
        break;
      case SB_GT:
        printf("SB_GT\n");
        break;
      case SB_GE:
        printf("SB_GE\n");
        break;
      case SB_PLUS:
        printf("SB_PLUS\n");
        break;
      case SB_MINUS:
        printf("SB_MINUS\n");
        break;
      case SB_TIMES:
        printf("SB_TIMES\n");
        break;
      case SB_SLASH:
        printf("SB_SLASH\n");
        break;
      case SB_LPAR:
        printf("SB_LPAR\n");
        break;
      case SB_RPAR:
        printf("SB_RPAR\n");
        break;
      case SB_LSEL:
        printf("SB_LSEL\n");
        break;
      case SB_RSEL:
        printf("SB_RSEL\n");
        break;
    }
  }

  int scan(char *fileName) {
    Token *token;

    if (openInputStream(fileName) == IO_ERROR) return IO_ERROR;

    token = getToken();
    state = 0;
    while (token->tokenType != TK_EOF && state != -1) {
      printToken(token);
      free(token);
      token = getToken();
      if (token->tokenType == TK_EOF) {
        printToken(token);
        break;
      }
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
