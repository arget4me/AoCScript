# AoCScript

## Grammar
	Program				::= {Statement}
	Statement			::= Assignment ";"
							| PrintStatement ";"
							| LoadStatement ";"
							| IfStatement ";"
							| LoopStatement ";"
							| AssertStatement ";"
	Assignment			::= Identifier "=" ( Expression | "LINE" | String )
	PrintStatement		::= ( "print" | "simon says" ) ( Identifier | String | "DAY" )
	LoadStatement		::= "load" String
	IfStatement			::= "if" Expression ":" {Statement} "else" ":" {Statement} "end"
	LoopStatement		::= "loop" ( Expression "times" | Iterator ) ":" {Statement} "loopstop"
	Iterator			::= "DAY" "." "lines" | "LINE" "." chars
	AssertStatement		::= "assert" Expression ":" String
	Expression			::= Logic { ("<" | ">" | "==" | "<=" | ">=" ) Logic}
	Logic				::= Term { ("+" | "-") Term}
	Term				::= Factor { ("*" | "/" | "modulo" ) Factor}
	Factor				::= Number
							| Identifier
							| "(" Expression ")"
							| Negate
	Negate				::= "-" Factor
	Identifier			::= Letter { Letter | Digit }
	Number				::= Digit { Digit }
	Letter				::=	"a" | ... |"z" | "A" | ... | "Z"    // Any alphabetical character, [a-zA-Z]
	Digit				::= "0" | ... | "9"						// Any numeric digit [0-9]
	String				::= \".*\"

## TODO
[x] Need loading input file<br/>
[ ] Need string lengths<br/>
[ ] Need string character indexing<br/>
[ ] Need string matching operators<br/>
[ ] Need character matching operators<br/>
[ ] Need string manipulations : append, substring, split, etc.<br/>
[ ] Probably need multiline strings.<br/>
[ ] Need a list data-structure : vector\<int\>.<br/>
[ ] Need a map data-structure : map<string, int>.<br/>

## Examples

### Code from "examples/example7.aoc":

	load "examples/example1.aoc";
	print DAY;

	a = 2;
	print a;


	print "SOME PRINT";
	print "SOME FAILED PRINT";
	print "SOME SUCCESS PRINT";
	print "SOME RED PRINT";
	print "SOME YELLOW PRINT";
	print "SOME GREEN PRINT";
	print "SOME BLUE PRINT";
	print "SOME MAGENTA PRINT";
	print "SOME CYAN PRINT";
	print "SOME WHITE PRINT";
	print "Colors are: GREEN CYAN GREEN YELLOW MAGENTA SUCCESS FAILED BLUE RED WHITE.";

![Example7 Output](/screenshots/example7.aoc.png)

<br/>

### Code from "examples/example5.aoc":

	size = 10;
	i = 0;
	loop size times:
		simon says i;
		if i modulo 2 == 0:
			print "EVEN";
		else:
			print "ODD";
		end;
		i = i + 1;
	loopstop;

	print i;
	assert i == size : "'i' must be equal to 'size' at this point";

	print "SUCCESS!";

Results:

	Run code : examples/example5.aoc
	Simon Says: i   = 0
	Simon Says: 'EVEN'
	Simon Says: i   = 1
	Simon Says: 'ODD'
	Simon Says: i   = 2
	Simon Says: 'EVEN'
	Simon Says: i   = 3
	Simon Says: 'ODD'
	Simon Says: i   = 4
	Simon Says: 'EVEN'
	Simon Says: i   = 5
	Simon Says: 'ODD'
	Simon Says: i   = 6
	Simon Says: 'EVEN'
	Simon Says: i   = 7
	Simon Says: 'ODD'
	Simon Says: i   = 8
	Simon Says: 'EVEN'
	Simon Says: i   = 9
	Simon Says: 'ODD'
	Simon Says: i   = 10
	Simon Says: 'SUCCESS!'