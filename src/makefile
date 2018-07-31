###############################################################################
# Edge language recursive descent parser
# written by Josh Hawkins
###############################################################################
LEX		=	flex
.PRECIOUS 	=	parser.cpp rules.l lexer.h

edge: lex.yy.o main.o parser.o
	g++ -o $@ $^

lex.yy.o: lex.yy.c lexer.h
	gcc -o $@ -c $<

parser.o: parser.cpp *.h
	g++ -o $@ -c $<

main.o: main.cpp *.h
	$(CXX) -o $@ -c $<

lex.yy.c: rules.l
	$(LEX) $^

clean:
	$(RM) *.o lex.yy.c edge
