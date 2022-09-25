%{
#include <stdio.h>
%}

%token INTEGER_TYPE DOUBLE_TYPE BOOLEAN_TYPE STRING_TYPE BOOLEAN INTEGER DOUBLE STRING ASSIGNMENT_OP PLUS_OP MINUS_OP LP RP LB RB COMMA MULT_OP EQUALS_OP NOTEQUALS_OP MOD_OP LESS_THAN_OP GREATER_THAN_OP DIVIDE_OP LESS_THAN_EQ_OP GREATER_THAN_EQ_OP AND_OP OR_OP NOT_OP SEMICOLON FUNCTION_DECL MAIN RETURN RETURNS DOT IF ELSE ELSEIF WHILE FOR FROM DO ENDDO TO ENDIF THEN IN OUT STREAM_OUT STREAM_IN READ_HEADING READ_ALTITUDE READ_TEMPERATURE VERTICAL_MOVE HORIZONTAL_MOVE TURN_LEFT TURN_RIGHT SPRAY_NOZZLE CONNECT_BASE_COMPUTER WAIT IDENTIFIER

%left AND_OP OR_OP

%start program

%%

program: empty | functions main_function;

empty: /*no text*/;
functions: empty
          | functions function
          ;
function: function_header LB statements RB;
main_function: main_function_header LB statements RB;

function_name: IDENTIFIER;

function_header: FUNCTION_DECL function_name LP function_parameter_list RP
                | FUNCTION_DECL function_name LP function_parameter_list RP RETURNS data_type
                ;
main_function_header: FUNCTION_DECL MAIN LP function_parameter_list RP;
function_parameter_list: function_parameter_list COMMA function_param
                        | function_param
                        | empty
                        ;
function_param: data_type IDENTIFIER;
function_call: function_name LP function_call_parameter_list RP 
function_call_parameter_list: function_call_parameter_list COMMA expression
                        | expression
                        | empty
                        ;

data_type: INTEGER_TYPE
          | DOUBLE_TYPE
          | BOOLEAN_TYPE
          | STRING_TYPE
          ;
number: INTEGER
      | DOUBLE
      ;
          
statements: statements statement
          | statement
          ;

statement: declaration_stmnt SEMICOLON
        | if_stmnt
			  | loop_stmnt
        | assignment_stmnt SEMICOLON
			  | return_stmnt SEMICOLON
			  | inout_stmnt SEMICOLON
        | primitive_func_stmnt SEMICOLON
        | function_call SEMICOLON
        ;    

expression: numerical_expression 
          | logical_expression 
          | function_call
          | STRING
          ;

numerical_expression: numerical_expression PLUS_OP num_term 
                    | numerical_expression MINUS_OP num_term
                    | num_term
                    ;
num_term: IDENTIFIER 
        | number 
        | num_term MULT_OP num_factor 
        | num_term MOD_OP num_factor 
        | num_term DIVIDE_OP num_factor
        ;
num_factor: LP numerical_expression RP 
          | IDENTIFIER 
          | number 
          ;
          
logical_expression: LP logical_expression_noparantheses RP
                  | BOOLEAN
                  ;

logical_expression_noparantheses: LP logical_expression_noparantheses RP
				          | LP NOT_OP logical_expression_noparantheses RP
				          | logical_expression_noparantheses AND_OP logical_expression_noparantheses
				          | logical_expression_noparantheses OR_OP logical_expression_noparantheses
				          | BOOLEAN
			        	  | IDENTIFIER
				          | numerical_expression relational_operator numerical_expression
                  ;

relational_operator: EQUALS_OP 
                  | NOTEQUALS_OP 
                  | GREATER_THAN_OP 
                  | LESS_THAN_OP 
                  | GREATER_THAN_EQ_OP 
                  | LESS_THAN_EQ_OP
                  ;
        
declaration_stmnt: initialized_declaration 
                | noninitialized_declaration
                ;
initialized_declaration: data_type IDENTIFIER ASSIGNMENT_OP expression;
noninitialized_declaration: data_type IDENTIFIER;

assignment_stmnt: IDENTIFIER ASSIGNMENT_OP expression; 
          
loop_stmnt: for_stmnt 
          | while_stmnt
          ;

for_stmnt: FOR IDENTIFIER FROM numerical_expression TO numerical_expression DO statements ENDDO ;

while_stmnt: WHILE logical_expression DO statements ENDDO ;

if_stmnt: IF logical_expression THEN statements multiple_elseif else_stmnt  ENDIF 
        |  IF logical_expression THEN statements multiple_elseif ENDIF
        |  IF logical_expression THEN statements else_stmnt ENDIF
        |  IF logical_expression THEN statements ENDIF
        ;

else_stmnt : ELSE statements; 

multiple_elseif: elseif
                | elseif multiple_elseif
                ;
elseif: ELSEIF logical_expression THEN statements

inout_stmnt: input_stmnt
            | output_stmnt
            ;
input_stmnt: IN STREAM_IN IDENTIFIER ;
output_stmnt: OUT STREAM_OUT expression ;
return_stmnt: RETURN expression ;


primitive_func_stmnt: read_heading_call
                    |  read_altitude_call
                    |  read_temperature_call
                    |  vertical_move_call
                    |  horizontal_move_call
                    |  turn_left_call
                    |  turn_right_call
                    |  spray_nozzle_call
                    |  connect_base_computer_call
                    |  wait_call
                    ;

read_heading_call: READ_HEADING STREAM_IN IDENTIFIER ;
read_altitude_call: READ_ALTITUDE STREAM_IN IDENTIFIER ;
read_temperature_call: READ_TEMPERATURE STREAM_IN IDENTIFIER ;
vertical_move_call: VERTICAL_MOVE STREAM_OUT numerical_expression
                  | VERTICAL_MOVE
                  ;
                                
horizontal_move_call: HORIZONTAL_MOVE STREAM_OUT numerical_expression
                    | HORIZONTAL_MOVE 
                    ;
turn_left_call: TURN_LEFT STREAM_OUT numerical_expression
              | TURN_LEFT
              ;
turn_right_call: TURN_RIGHT STREAM_OUT numerical_expression
              | TURN_RIGHT
              ;
spray_nozzle_call: SPRAY_NOZZLE STREAM_OUT logical_expression ;
connect_base_computer_call: CONNECT_BASE_COMPUTER STREAM_OUT numerical_expression ;
wait_call: WAIT STREAM_OUT numerical_expression
          | WAIT
          ;

%%
#include "lex.yy.c"
int lineno;
int has_errors = 0;

main() { 

  yyparse();
  if (!has_errors) {
    printf("Parse successful.\n");
  }
  
  return 0;
}

yyerror( char *s ) { has_errors = 1; fprintf( stderr, "Syntax error on line %d!\n", lineno + 1); return 1; };