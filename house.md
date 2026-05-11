## Stmt
- if/else
- while/for
- ret/cont/break
- switch/match
- block
- expr
- decl
- null

## Decl
- var
- func
- class
- alias
- variant

## Expr
- binary
- unary pref -> post
- call
- index
- access
- type
- lambda

```
x = [1,2,3]
x = Person{.name = " "}

```

`= - инструкция`

```BNF
var-decl := "var" binding-list ";"
binding-list := binding ("," binding)* ";"
binding := identifier ":" type ("=" init)*
```