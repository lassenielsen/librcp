/* Grammar of regular expressions.  */

%{
  #include <rcp/re.hpp>
  #define YYSTYPE RE*
  #include <math.h>
  char yylex (void) {return 'a';}
  void yyerror (char const *err) { throw err; }
%}

%token ALPH /* [^\+*]|\\ */
%token BS   /* \ */
%token PLUS /* + */
%token STAR /* * */

%% /* Grammar rules and actions follow.  */
re:       re STAR       { $$ = new RE_Star($1); }
        | re2           { $$ = $1; }
;

re2:      re3 re2       { $$ = new RE_Seq($1,$2); }
        | re3           { $$ = $1; }
;

re3:      re3 '+' re4   { $$ = new RE_Sum($1,$3); }
        | re4           { $$ = $1; }
;

re4:      '0'           { $$ = new RE(); }
        | '1'           { $$ = new RE_One(); }
        | '\\' '\\'     { $$ = new RE_Char('\\'); }
        | '\\' '+'      { $$ = new RE_Char('+'); }
        | '\\' '*'      { $$ = new RE_Char('*'); }
        | '\\' '('      { $$ = new RE_Char('('); }
        | '\\' ')'      { $$ = new RE_Char(')'); }
        | 'a'           { $$ = new RE_Char('a'); }
        | '(' re ')'    { $$ = $2 }
;
%%
