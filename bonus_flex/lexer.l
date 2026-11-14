%option noyywrap

%{
#include <stdio.h>

/* Exposed to C++ so parity tests can verify lexical error counts */
int flex_error_count = 0;
%}

letter      [a-zA-Z]
digit       [0-9]
digits      {digit}+

%%
    /* Keywords listed first to match Student 1's reserved priority */
"boolean"|"int"|"float"|"if"|"else"|"while"     { fprintf(yyout, "%s\n", yytext); }

    /* Identifiers and Numbers (matching phase1.rules) */
{letter}({letter}|{digit})*                     { fprintf(yyout, "id\n"); }
{digit}+|{digit}+\.{digits}(E{digits})?         { fprintf(yyout, "num\n"); }

    /* Relational, Assignment, and Arithmetic Operators */
"=="|"!="|">"|">="|"<"|"<="                     { fprintf(yyout, "relop\n"); }
"="                                             { fprintf(yyout, "assign\n"); }
"+"|"-"                                         { fprintf(yyout, "addop\n"); }
"*"|"/"                                         { fprintf(yyout, "mulop\n"); }

    /* Punctuation */
";"|","|"("|")"|"{"|"}"                         { fprintf(yyout, "%s\n", yytext); }

    /* Whitespace (matching Scanner::isWhitespace) */
[ \t\r\n\f\v]+                                  { /* skip whitespace */ }

    /* Panic-mode error recovery: count invalid byte and continue */
.                                               { ++flex_error_count; }
%%