/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

#define MAX_STRING_LEN 100

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

Token *getToken(void)
{
  Token *token;
  int ln, cn;
  char tmp_char;
  char tmp_string[MAX_STRING_LEN] = "";
  int len = 0;

  int state = 0;

  while (1)
  {
    switch (state)
    {
    case 0:
      if (currentChar == EOF)
      {
        state = 37;
        break;
      }
      switch (charCodes[currentChar])
      {
      case CHAR_SPACE:
        state = 1;
        break;
      case CHAR_LPAR:
        state = 2;
        ln = lineNo;
        cn = colNo;
        break;
      case CHAR_LETTER:
        state = 8;
        ln = lineNo;
        cn = colNo;
        break;
      case CHAR_DIGIT:
        state = 10;
        ln = lineNo;
        cn = colNo;
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
      case CHAR_COMMA:
        state = 17;
        break;
      case CHAR_SEMICOLON:
        state = 18;
        break;
      case CHAR_PERIOD:
        state = 19;
        ln = lineNo;
        cn = colNo;
        break;
      case CHAR_GT:
        state = 22;
        ln = lineNo;
        cn = colNo;
        break;
      case CHAR_LT:
        state = 25;
        ln = lineNo;
        cn = colNo;
        break;
      case CHAR_EXCLAIMATION:
        state = 28;
        ln = lineNo;
        cn = colNo;
        break;
      case CHAR_COLON:
        state = 31;
        ln = lineNo;
        cn = colNo;
        break;
      case CHAR_SINGLEQUOTE:
        state = 34;
        ln = lineNo;
        cn = colNo;
        break;
      case CHAR_RPAR:
        state = 39;
        break;
      default:
        state = 38;
        break;
      }
      break;
    case 1:
      readChar();
      if (charCodes[currentChar] != CHAR_SPACE)
        state = 0;
      break;
    case 2:
      readChar();
      if (charCodes[currentChar] == CHAR_PERIOD)
        state = 6;
      else if (charCodes[currentChar] == CHAR_TIMES)
        state = 3;
      else
        state = 7;
      break;
    case 3:
      readChar();
      if (charCodes[currentChar] == CHAR_TIMES)
        state = 4;
      else if (currentChar == EOF)
        state = 40;
      break;
    case 4:
      readChar();
      if (charCodes[currentChar] == CHAR_RPAR)
        state = 5;
      else if (charCodes[currentChar] == CHAR_TIMES)
        state = 4;
      else if (currentChar == EOF)
        state = 40;
      else
        state = 3;
      break;
    case 5:
      readChar();
      state = 0;
      break;
    case 6:
      token = makeToken(SB_LSEL, ln, cn);
      readChar();
      return token;
    case 7:
      token = makeToken(SB_LPAR, ln, cn);
      return token;
    case 8:
      tmp_char = currentChar;
      strncat(tmp_string, &tmp_char, 1);
      len += 1;
      readChar();
      if (charCodes[currentChar] != CHAR_DIGIT && charCodes[currentChar] != CHAR_LETTER)
        state = 9;
      break;
    case 9:
      if (len > MAX_IDENT_LEN)
      {
        error(ERR_IDENTTOOLONG, ln, cn);
        token = makeToken(TK_NONE, ln, cn);
        return token;
      }
      else
      {
        TokenType tk_type = checkKeyword(tmp_string);
        if (tk_type == TK_NONE)
          tk_type = TK_IDENT;
        token = makeToken(tk_type, ln, cn);
        strcpy(token->string, tmp_string);
        return token;
      }
    case 10:
      tmp_char = currentChar;
      strncat(tmp_string, &tmp_char, 1);
      readChar();
      if (charCodes[currentChar] != CHAR_DIGIT)
        state = 11;
      break;
    case 11:
      printf(" "); /*can't begin with declaration*/
      char *p;
      // string to long, base 10
      long num = strtol(tmp_string, &p, 10);

      if (num >= INT_MAX)
      {
        error(ERR_NUMBERTOOLARGE, ln, cn);
        token = makeToken(TK_NONE, ln, cn);
        return token;
      }
      else
      {
        token = makeToken(TK_NUMBER, ln, cn);
        strcpy(token->string, tmp_string);
        token->value = atoi(tmp_string);
        return token;
      }
    case 12:
      token = makeToken(SB_PLUS, lineNo, colNo);
      readChar();
      return token;
    case 13:
      token = makeToken(SB_MINUS, lineNo, colNo);
      readChar();
      return token;
    case 14:
      token = makeToken(SB_TIMES, lineNo, colNo);
      readChar();
      return token;
    case 15:
      token = makeToken(SB_SLASH, lineNo, colNo);
      readChar();
      return token;
    case 16:
      token = makeToken(SB_EQ, lineNo, colNo);
      readChar();
      return token;
    case 17:
      token = makeToken(SB_COMMA, lineNo, colNo);
      readChar();
      return token;
    case 18:
      token = makeToken(SB_SEMICOLON, lineNo, colNo);
      readChar();
      return token;
    case 19:
      readChar();
      if (charCodes[currentChar] == CHAR_RPAR)
        state = 20;
      else
        state = 21;
      break;
    case 20:
      token = makeToken(SB_RSEL, ln, cn);
      readChar();
      return token;
    case 21:
      token = makeToken(SB_PERIOD, ln, cn);
      return token;
    case 22:
      readChar();
      if (charCodes[currentChar] == CHAR_EQ)
        state = 23;
      else
        state = 24;
      break;
    case 23:
      token = makeToken(SB_GE, ln, cn);
      readChar();
      return token;
    case 24:
      token = makeToken(SB_GT, ln, cn);
      return token;
    case 25:
      readChar();
      if (charCodes[currentChar] == CHAR_EQ)
        state = 26;
      else
        state = 27;
      break;
    case 26:
      token = makeToken(SB_LE, ln, cn);
      readChar();
      return token;
    case 27:
      token = makeToken(SB_LT, ln, cn);
      return token;
    case 28:
      readChar();
      if (charCodes[currentChar] == CHAR_EQ)
        state = 29;
      else
        state = 30;
      break;
    case 29:
      token = makeToken(SB_NEQ, ln, cn);
      readChar();
      return token;
    case 30:
      error(ERR_INVALIDSYMBOL, ln, cn);
      token = makeToken(TK_NONE, ln, cn);
      return token;
    case 31:
      readChar();
      if (charCodes[currentChar] == CHAR_EQ)
        state = 32;
      else
        state = 33;
      break;
    case 32:
      token = makeToken(SB_ASSIGN, ln, cn);
      readChar();
      return token;
    case 33:
      token = makeToken(SB_COLON, ln, cn);
      return token;
    case 34:
      readChar();
      // Catch errors
      if (currentChar == EOF || !isprint(currentChar))
      {
        error(ERR_INVALIDCHARCONSTANT, ln, cn);
        token = makeToken(TK_NONE, ln, cn);
        return token;
      }
      tmp_char = currentChar;
      state = 35;
      break;
    case 35:
      readChar();
      if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
        state = 36;
      else
      {
        error(ERR_INVALIDCHARCONSTANT, ln, cn);
        token = makeToken(TK_NONE, ln, cn);
        return token;
      }
      break;
    case 36:
      token = makeToken(TK_CHAR, ln, cn);
      strncpy(token->string, &tmp_char, 1);
      readChar();
      return token;
    case 37:
      return makeToken(TK_EOF, lineNo, colNo);
    case 38:
      token = makeToken(TK_NONE, lineNo, colNo);
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
      readChar();
      return token;
    case 39:
      token = makeToken(SB_RPAR, lineNo, colNo);
      readChar();
      return token;
    }
  }
}

/******************************************************************/

void printToken(Token *token)
{

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType)
  {
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

int scan(char *fileName)
{
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF)
  {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[])
{
  if (argc <= 1)
  {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR)
  {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}