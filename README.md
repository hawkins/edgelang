# Edge

Edge is a programming language for expressing graphs in code and can be easily used to present graphical charts.

Edge aims to provide an easy syntax for expressing complicated node networks, finite automata, or even flow charts.

Basic syntax is as follows:

```Edge
| Comments are set off with pipes
|-
   Multiline
   comments too!
-|

| You can configure graph settings with sections
begin config
weighted: false
directed: false
end

| Let's make a triangle
| Define vertices
A :: start
B :: left
C :: right
| Define edges between vertices
start -> left
start -> right
left -> right
```

A more complicated example shows where Edge comes in handy:

*Note: Not all complex features of language implemented yet*

```
begin config
weighted: true
end

A, "A's node label" :: var
var -> B
var -> C

| You can even implicitly define an identifier for easier modifications
A ::
_ -> B, 5
_ -> C, 7
```

## Parser

A recursive descent parser is included in `/src`.
This can be made by running `make` inside the directory.

The parser will parse the input file, as well as write a JS output file with Node View so you can view the generated graph by loading `src/test/index.html` in your browser, provided you use Edge to generate `test.js`.

This parser can be tested by running commands such as:

```
cd src
make
./parse.exe -i test/program0.edge -o test/test.js
```

Alternatively, you can use the REPL by omitting the `-i` flag:

```
./parse.exe -o test/test.js
```
