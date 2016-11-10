# Edge Grammar Rules

## Grammar Productions | First Token Set

<!-- Program -->
P --> {S}                                             ||    { VERTEX, |, |-, begin}

<!-- Statement -->
S --> A | E | C | M | G                               ||    { ID, VERTEX, |, |-, begin}

<!-- Edge -->
E --> ID | VERTEX -> (ID | VERTEX)(, Weight)(, Label) ||    { ID, VERTEX }

<!-- Variable assignment -->
A --> VERTEX :: [ ID ]                                ||    { VERTEX }

<!-- Comments -->
C --> \| { . }                                        ||    { | }
M --> \|- { . } -\|                                   ||    { |- }

<!-- Section -->
G --> begin LABEL { R } end                           ||    { begin }   
<!-- Rule -->
R --> PROPERTY : true | false                         ||    { PROPERTY }
