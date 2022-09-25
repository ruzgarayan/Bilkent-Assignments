INTEGER_TYPE  				Integer
DOUBLE_TYPE  				Double
BOOLEAN_TYPE  				Boolean
STRING_TYPE					String
BOOLEAN  					(TRUE|FALSE)  
INTEGER  					([+-])?[0-9]+  
DOUBLE  					([+-])?[0-9]*{DOT}?[0-9]+
STRING						\".*\"
ASSIGNMENT_OP				\=
PLUS_OP  					\+
MINUS_OP 					\-
LP  						\(
RP  						\)
LB  						\{
RB  						\}
COMMA  						\,
MULT_OP  					\*
EQUALS_OP  					EQUALS
NOTEQUALS_OP  				NEQUALS
MOD_OP  					\%
LESS_THAN_OP				LT
GREATER_THAN_OP 			GT
DIVIDE_OP  					\/
LESS_THAN_EQ_OP 			LTEQ
GREATER_THAN_EQ_OP 			GTEQ
AND_OP 						AND
OR_OP 						OR
NOT_OP  					NOT
SEMICOLON 					\;
FUNCTION_DECL				function
MAIN						main
RETURN						return
RETURNS						returns
DOT    						\.
ALPHANUMERIC 				([A-Za-z_$]|[0-9])
IF	 						if
ELSE						else
ELSEIF					elseif
WHILE						while
FOR							for   
FROM						from  	
DO							do
ENDDO						enddo
TO 							to
ENDIF 						endif
THEN						then
IN							in
OUT							out 
STREAM_OUT					<<
STREAM_IN 					>>
READ_HEADING				readHeading
READ_ALTITUDE				readAltitude
READ_TEMPERATURE			readTemperature
VERTICAL_MOVE 				verticalMove
HORIZONTAL_MOVE				horizontalMove
TURN_LEFT					turnLeft
TURN_RIGHT					turnRight
SPRAY_NOZZLE 				sprayNozzle
CONNECT_BASE_COMPUTER 		connectBaseComputer
WAIT						wait
IDENTIFIER					[A-Za-z_$]({ALPHANUMERIC})*
COMMENT  					#[^\n]*

%%
{INTEGER_TYPE}  			{return INTEGER_TYPE;}
{DOUBLE_TYPE}  				{return DOUBLE_TYPE;}
{BOOLEAN_TYPE}  			{return BOOLEAN_TYPE;}
{STRING_TYPE}  				{return STRING_TYPE;}
{BOOLEAN}					{return BOOLEAN;}
{INTEGER}  					{return INTEGER;}
{DOUBLE}  					{return DOUBLE;}
{STRING}  					{return STRING;}
{ASSIGNMENT_OP}  			{return ASSIGNMENT_OP;}
{PLUS_OP}					{return PLUS_OP;}
{MINUS_OP}					{return MINUS_OP;}
{LP}						{return LP;}
{RP}						{return RP;}
{LB}						{return LB;}
{RB}						{return RB;}
{COMMA}						{return COMMA;}
{MULT_OP}					{return MULT_OP;}
{EQUALS_OP}					{return EQUALS_OP;}
{NOTEQUALS_OP}				{return NOTEQUALS_OP;}
{MOD_OP}					{return MOD_OP;}
{LESS_THAN_OP}				{return LESS_THAN_OP;}
{GREATER_THAN_OP}			{return GREATER_THAN_OP;}
{DIVIDE_OP}					{return DIVIDE_OP;}
{LESS_THAN_EQ_OP}			{return LESS_THAN_EQ_OP;}
{GREATER_THAN_EQ_OP}		{return GREATER_THAN_EQ_OP;}
{AND_OP}					{return AND_OP;}
{OR_OP}						{return OR_OP;}
{NOT_OP}					{return NOT_OP;}
{SEMICOLON}					{return SEMICOLON;}
{FUNCTION_DECL}				{return FUNCTION_DECL;}
{MAIN}						{return MAIN;}
{RETURN}					{return RETURN;}
{RETURNS}					{return RETURNS;}
{DOT}						{return DOT;}
{IF}  						{return IF;}
{ELSE}  					{return ELSE;}
{ELSEIF}  					{return ELSEIF;}
{WHILE}  					{return WHILE;}   
{FOR}  						{return FOR;}   
{FROM}  					{return FROM;} 	
{DO}  						{return DO;}
{ENDDO}  					{return ENDDO;}
{TO}  						{return TO;}
{ENDIF}  					{return ENDIF;}
{THEN}  					{return THEN;}
{IN}						{return IN;}
{OUT}						{return OUT;}
{STREAM_OUT}				{return STREAM_OUT;}
{STREAM_IN} 				{return STREAM_IN;}
{READ_HEADING}				{return READ_HEADING;}
{READ_ALTITUDE}				{return READ_ALTITUDE;}
{READ_TEMPERATURE}			{return READ_TEMPERATURE;}
{VERTICAL_MOVE} 			{return VERTICAL_MOVE;}
{HORIZONTAL_MOVE}			{return HORIZONTAL_MOVE;}
{TURN_LEFT}					{return TURN_LEFT;}
{TURN_RIGHT}				{return TURN_RIGHT;}
{SPRAY_NOZZLE} 				{return SPRAY_NOZZLE;}
{CONNECT_BASE_COMPUTER} 	{return CONNECT_BASE_COMPUTER;}
{WAIT} 						{return WAIT;}
{IDENTIFIER}				{return IDENTIFIER;}
{COMMENT}					{}

\n                { extern int lineno; lineno++;}

%%
int yywrap() { return 1; }