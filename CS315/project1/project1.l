%option main
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
{INTEGER_TYPE}  			{printf("INTEGER_TYPE ");}
{DOUBLE_TYPE}  				{printf("DOUBLE_TYPE ");}
{BOOLEAN_TYPE}  			{printf("BOOLEAN_TYPE ");}
{STRING_TYPE}  				{printf("STRING_TYPE ");}
{BOOLEAN}					{printf("BOOLEAN ");}
{INTEGER}  					{printf("INTEGER ");}
{DOUBLE}  					{printf("DOUBLE ");}
{STRING}  					{printf("STRING ");}
{ASSIGNMENT_OP}  			{printf("ASSIGNMENT_OP ");}
{PLUS_OP}					{printf("PLUS_OP ");}
{MINUS_OP}					{printf("MINUS_OP ");}
{LP}						{printf("LP ");}
{RP}						{printf("RP ");}
{LB}						{printf("LB ");}
{RB}						{printf("RB ");}
{COMMA}						{printf("COMMA ");}
{MULT_OP}					{printf("MULT_OP ");}
{EQUALS_OP}					{printf("EQUALS ");}
{NOTEQUALS_OP}				{printf("NEQUALS ");}
{MOD_OP}					{printf("MOD_OP ");}
{LESS_THAN_OP}				{printf("LESS_THAN_OP ");}
{GREATER_THAN_OP}			{printf("GREATER_THAN_OP ");}
{DIVIDE_OP}					{printf("DIVIDE_OP ");}
{LESS_THAN_EQ_OP}			{printf("LESS_THAN_EQ_OP ");}
{GREATER_THAN_EQ_OP}		{printf("GREATER_THAN_EQ_OP ");}
{AND_OP}					{printf("AND_OP ");}
{OR_OP}						{printf("OR_OP ");}
{NOT_OP}					{printf("NOT_OP ");}
{SEMICOLON}					{printf("SEMICOLON ");}
{FUNCTION_DECL}				{printf("FUNCTION_DECL ");}
{MAIN}						{printf("MAIN ");}
{RETURN}					{printf("RETURN ");}
{RETURNS}					{printf("RETURNS ");}
{DOT}						{printf("DOT ");}
{IF}  						{printf("IF ");}
{ELSE}  					{printf("ELSE ");}
{WHILE}  					{printf("WHILE ");}   
{FOR}  						{printf("FOR ");}   
{FROM}  					{printf("FROM ");} 	
{DO}  						{printf("DO ");}
{ENDDO}  					{printf("ENDDO ");}
{TO}  						{printf("TO ");}
{ENDIF}  					{printf("ENDIF ");}
{THEN}  					{printf("THEN ");}
{IN}						{printf("IN ");}
{OUT}						{printf("OUT ");}
{STREAM_OUT}				{printf("STREAM_OUT ");}
{STREAM_IN} 				{printf("STREAM_IN ");}
{READ_HEADING}				{printf("READ_HEADING ");}
{READ_ALTITUDE}				{printf("READ_ALTITUDE ");}
{READ_TEMPERATURE}			{printf("READ_TEMPERATURE ");}
{VERTICAL_MOVE} 			{printf("VERTICAL_MOVE ");}
{HORIZONTAL_MOVE}			{printf("HORIZONTAL_MOVE ");}
{TURN_LEFT}					{printf("TURN_LEFT ");}
{TURN_RIGHT}				{printf("TURN_RIGHT ");}
{SPRAY_NOZZLE} 				{printf("SPRAY_NOOZLE ");}
{CONNECT_BASE_COMPUTER} 	{printf("CONNECT_BASE_COMPUTER ");}
{WAIT} 						{printf("WAIT ");}
{IDENTIFIER}				{printf("IDENTIFIER ");}
{COMMENT}					{printf("COMMENT ");}
