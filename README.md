# Edge

Edge is a programming language for expressing graphs in code and can be easily used to present graphical charts.

Edge aims to provide an easy syntax for expressing complicated node networks, automata, or even flow charts.

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
A -> B
B -> C
C -> A
```

A more complicated example shows where Edge comes in handy:
```
begin config
weighted: true
end

A, "A's node label" :: var
var -> B
var -> C

| You can even implicitly define an identifier for easier modifications
A ::
_ -> B
_ -> C
```

## Parser

A recursive descent parser is included in `/parser`.
This can be made by running `make` inside the directory.

This parser can then be tested by running commands such as:

```
cat parser/test/program1.edge | ./parser/parse.exe
```
