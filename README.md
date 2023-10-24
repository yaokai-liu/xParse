
# xParse Engine

xParse Engine is a [xRe](https://gitlab.com/yaokai-liu/xre)(deprecated) based, 
[xLex](https://gitlab.com/yaokai-liu/xlex)(deprecated) based text parsing and processing program 

*So did xParse and xRe. And interesting, xParse is xRe based.*

*Actually, `xRe --> xLex --> xParse`, that's the way of evolution of my x-series text parsing and processing program.*

xParse Engine reads xParse rules and uses them to process the input text and output the result.

## Grammar
xParse need a language to describe the inputs and a language to describe how to make up the outputs.

The parsing language that xParse using is called `xParse Parsing Language`, `xPL` for short.

And The composing language that xParse using is called `xParse Composing Language`, `xCL` for short.

They combine regularly to form a grammatical rule, 
while the xPL is called `xParse_Regexp` and xCL is called `xParse_Program`,
i.e.:

```
xParse_Rule ::= ("void" | TOKEN) IDENTIFIER "(" xParse_Regexp ")" "{" xParse_Program "}"

```

**xParse Regexp**

Some grammars of xParse regexp are defined as follows:
```

xParse_Regexp ::= ( "~" )? xParse_Regexp ( "&" )?

xParse_Regexp ::= Branch "|" xParse_Regexp
                | Branch


Branch ::= (xParseObject)+


xParseObject ::= Group | Sequence | CharacterClass 
               | Quantifiered | Labeled | Called | SwitchCase
               | (ATTR XParseObject)


Group ::= "(" xParse_Regexp ")"

Sequence ::= ( PLAIN_CHAR | SINGLE_ESCAPE )+

CharacterClass ::= "[" (PLAIN_CHAR | SINGLE_ESCAPE | Range | CharacterClass | ClassMacro)+ "]"
                 | CLASS_ESCAPE

Quantifiered ::= xParse_Object Quantifier

Labeled ::= xParse_Object "=" "<" IDENTIFIER ">"

Called ::= ("@" | "$") Label

SwitchCase ::= "<" ArithExpression "?" xParse_Regexp ">"

Range ::= PLAIN_CHAR "-" PLAIN_CHAR

ClassMacro ::= ":" IDENTIFIER ":"

Quantifier ::= "{" ArithExpression? ","? ArithExpression? ";"? ArithExpression? "}"
             | QUANTIFIER_MACRO

Label ::= "<" Literal ">"

```

Unlike those regular expression, xParse regexp is designed for not only parsing but also capturing.
Thus connected plain characters will be combined as an integration, i.e. `Sequence`.

Also, xParse regexps use blank characters as separators for `ReObj`s, this is not existing in other regular expression.

**xParse Program**

Some grammars of xParse program are defined as follows:
```
xParse_Program ::= ProgramBlock

ProgramBlock ::= ProgramStatement | "{" ProgramStatement* "}"

ProgramStatement ::= Assignment
                   | ControlStatement 
                   | ExitMark


Assignment ::= IDENTIFIER ":=" ArithExpression ";"

ControlStatement ::= ("if" | "while") Parenthesis ProgramBlock ("else" ProgramBlock)?

ExitMark ::= "$<"

ArithExpression ::= SingleExpr BIN_OP ArithExpression
                  | SingleExpr

Parenthesis ::= "(" ArithExpression ")"

SingleExpr ::= FUNC_NAME "(" Arg? ")"
             | SIN_OP SingleExpr
             | Parenthesis
             | Literal

Args ::= ArithExpression ("," ArithExpression)*

Literal ::= IDENTIFIER | NUMBER | ("#" NUMBER) | Capture

Capture ::= "$" ( IDENTIFIER | NUMBER )
```

Unlike those programming languages, the syntax of xParse is only designed for text processing,
and in simplicity.

**Token Type**

xParse need some way to describe what is the memory structure of a rule's result.
xParse uses a `struct` in C like grammar to deal with it, called `xParse Token`.
A xParse token will be defined with a structure, that led with a keyword `token`, and bracketed with `{` and `}`.

xParse need types to determinate size of members of tokens. But not all types are legal in xParse.
xParse only support `int16`, `int32`, `int64`, `char`, `string` and `bool`,
which means 16-bit integer, 32-bit integer, 64-bit integer, 8-bit data, address of a string (`ptr_t` in C), 
and the bool type(1 byte storage).
But tokens are not types.

For a better using, xlex provides `struct` and `union` to describe continuous memories and members' shared memories.
These two structures can be nested.

Besides, `void` is also can be used as a type, to describe those rules with no output.

With those types and `void`, the grammar to define a xParse token will be like:
```
TokenDef ::= "token" IDENTIFIER "{" (BUILTIN_TYPE MEMBER_NAME ";")* "}" ";"

```
where `IDENTIFIER` is the token name.

Note that xParse allows an empty token as result of a rule. And xParse disallows a nesting token in a token.


## xParse Regexp Grammar Structures

### Escape

**Definition:**
An escape is `\ ` concat with some characters.
escape has two kinds, `SINGLE_ESCAPE` and `CLASS_ESCAPE`.
`SINGLE_ESCAPE` will be viewed as a plain character and integrated into a `Sequence`.
`CLASS_ESCAPE` will be viewed as a character class.

All escapes please refer the [escape table](#escape-table).

### Plain Text (Sequence)

**Definition:**
Plain text(`Sequence`) is strings with only `PLAIN_CHAR` or `SINGLE_ESCAPE`.

**Breaking:**
And blank characters like ` `(space), `\n`(line feed), `\t`(horizontal tab), etc.,
i.e. all characters that `\s` in other kinds of normal regular expression,
will be viewed as breaking of plain text.

**Function:**
Match a sequence is to match every character once in order, even exists duplicates.

### Character Class (Set)

**Definition**:
A `CharacterClass` is characters bracketed with `[` and `]`.

**Function:**
Match a set is to match any none `-` or `^` character in the set's characters once, but include characters between
the two characters concat with `-`.

1. **Range:** a range is a continuous character set, that described with a `-` in its start character and end character.
2. **Class Macro:** a class macro is a class that builtin with xParse processor. *(not implemented yet)*
3. **inverse:** characters after `^` will be thought forbidden, if matched, means match failed.

*It is considered to support calculations between character classes.*

### Group

**Definition:**
A `Group` is an xParse regexp bracketed with `(` and `)`.

**Function:**
Match a group is match the xParse regexp in it.

**Feature 1:**
When creating a group, there will create a label refer to it,
and the label's name always is a number, according to the `(`'s orders.

### Branch

**Definition:**
`Branch`s are those xParse regexps split by `|`, and every part should not include a `|` stands.
Those parts are called `Branch`.

**Feature:**
Branches in a group share group's inner common environment.

### Quantifier

**Definition:**
A `Quantifier` is some expressions bracketed with `{` and `}` but divided with `,` and `;`.
Number of expressions is 0 to 3, while the `,` and `;` can be omitted.

The first expression is the lower bound(included),
and the second is the upper bound(included, too).
The third expression is the step count of match.

Value of lower bound and upper bound must be non-negative integer, and step's must be positive integer.

But if the step is set as a non-positive, the parser will be performed with a non-defined behavior.

Upper bound's allowed reach the positive infinity.

And:
1. If lower bound is not set, it will be set to 0 by default;

2. if upper bound is not set, it will default to positive infinity;

3. if step count is not set, it will be set as 1 by default.

### Quantifier Macros

xParse macros are those strings to replace with their plains in an xParse regexp before match stage.

All xParse macros are:

| Macros | Value                 |
|:-------|:----------------------|
| `*`    | `{,;}` or `{0,;1}`    |
| `+`    | `{1,;}` or `{1,;1}`   |
| `?`    | `{0,1;}` or `{0,1;1}` |


**Function:**
Quantifier will not do any match operation. But it will let the executor repeat the last struct before it at least its
lower bound and at most its upper bound, for every step count.

### Switch
*(not implemented yet)*

**Definition:**
A `Switch` is an expression and an xParse regexp bracketed with `<` and `>` but seperated with `?` in order.

**Function:**
If the value of the expression is not a zero, the executor will execute the xParse.
If the xParse regexp has more than one branches, xParse parser will execute the branch the expression referred.

### Label

**Definition:**
A `Label` is a number or an identifier bracketed with `<` and `>`.

`Label`s can be created by `=` after a `ReObj`, also can be used led by `@` or `$`.

When a label is creating, it will refer to the previous `ReObj`,
and create a capture to store its last matching result.

Specially, in xParse, when call a label, its name can be a rule name, or a token.
If a called label's name is a rule, means it will call the rule to process the input.
If a called label's name is a token, means it will look up all rules that return this kind of token to find a suitable rule to process the input. 

**Feature:**
When using `@` to call a label, executor will execute its referring, and update its capture;
When using `$` to call a label, executor will match its last capture.

**Restrict:**
When creating a label, the label name must be identifier, numbers are forbidden.

### Capture

**Definition:**
A `Capture` is a piece of memory space to store results of executing xParse regexp.

A capture will be automatically created, and only for a group or a label.

### xParse Attributes

xParse structures can have some extern attributes, them are parts of xParse grammar.
xParse attributes tell parser and executor how to process those suffixed structures.

xParse attributes determinate some special behaviors of parsing and matching.

- **Only Parse:**
  Character `!` is an attribute prefix. It means the xParse structure string it prefixes will only compile to an xParse structure,
  but will not match.

- **Inverse:**
  Character `^` is an attribute prefix. An inverse-prefixed structure will be matched only when the structure is not matched.

- **Other Special Attributes:**
  *(no more yet)*

  
## xParse Program Grammar Structures

An xParse rule has only one xParse program.

xParse program is to process results captured by the xParse regexp of current xParse rule,
and composed them to rule output.

### Program Statement

Statements of xParse program are C like statements, except assignments.

There is no declaration in xParse program.
Type of Variables are only two kinds, integer or string.
And user don't need to declare e_type when create a variable.

- **Assignment:**
  An assignment will create a variable that alive in current xParse program if not exists,
  or it will modify the value of variable.
  
  Specially, for symbol `$`, that means store the value to the rule output.

- **Control Statement:**
  xParse uses control statements, it contains both branch and loop statements.

- **ExitMark:**
  Since xParse program has no `return` keyword, it uses exit mark to end current xParse rule. 

### Arithmetic Expression

**Definition:**
An `ArithExpression` is an arithmetic expression in `{` and `}` brackets in a xParseer expression
or in statements in a xParseer program.

In xParseer programs, expressions always are part of statements.

There are some different expressions:
- **Literal Values:**
  Literal values are those numbers or strings with no variables.

- **Variables**:
  variables are those identifiers consistent with only lower letters, digits and underscore, and must start with lower letters.

- **Literals:**
  Literal values and variables are all called literals.

- **Functions**:
  functions are builtin defined by compiler or interrupter.
  The name of functions must be UPPER letters, digits and underscore, and must start with UPPER letters.
  Arguments of functions are some expressions bracketed with `(` and `)` after function name,
  and if more than one should be separated with `,`.
  Functions will be list in the [Functions Table](#functions-table)

- **Operator Expressions**:
  other expressions are operator expressions. Normally them have at least one operator.
  Operators will be list in the [Operators table](#operators-table).

**Note:**
Literals, functions, parentheses, single operator expressions are all called single expressions.

**Note:**
arithmetic expressions of xParseer program all are lazy evaluated,
because xParseer program is what user tells to xParseer executor only how the result `$` is computed.


## Grammar Keywords

| keywords | means                                    |
|:---------|:-----------------------------------------|
| `void`   | no output or input                       |
| `token`  | to define a token                        |
| `struct` | to define a struct                       |
| `union`  | to define a union                        |
| `char`   | basic data type, 1 byte storage          |
| `int16`  | basic data type, 2 byte storage          |
| `int32`  | basic data type, 4 byte storage          |
| `int64`  | basic data type, 8 byte storage          |
| `string` | basic data type, `ptr_t` size in C       |
| `bool`   | basic data type, 1 byte storage          |
| `while`  | to imply a control flow, means a loop    |
| `if`     | to imply a control flow, means a branch  |
| `else`   | to imply a control flow, means otherwise |
| `true`   | bool value, means true.                  |
| `false`  | bool value, means false.                 |


## Grammar Symbols

### xParse Regexp

| Symbols     | means or uses                                                              |
|:------------|:---------------------------------------------------------------------------|
| `{` and `}` | means a quantifier                                                         |
| `[` and `]` | means a character set                                                      |
| `(` and `)` | means a group                                                              |
| `<` and `>` | means a label or switch case                                               |
| `~`         | means at begin of match                                                    |
| `!`         | means parse but not match                                                  |
| `@`         | means call a label to match                                                |
| `$`         | means parse the result of label                                            |
| `^`         | means inverse match                                                        |
| `&`         | means at end of match                                                      |
| `\|`        | means branch                                                               |
| `*`         | quantifier macro, means `{0,+∞; 1}`                                        |
| `+`         | quantifier macro, means `{1,+∞; 1}`                                        |
| `?`         | quantifier macro, means `{0,1; 1}`;<br/> or means options in a switch case |
| `-`         | character range connector                                                  |
| `=`         | means set label                                                            |
| `:`         | character set in a character                                               |
| `\ `        | means escape                                                               |
| `,`         | range spelitor in quantifier                                               |
| `;`         | split the range and step in quantifier                                     |


### xParse Program


| Symbols     | means or uses              |
|:------------|:---------------------------|
| `{` and `}` | means a program block      |
| `[` and `]` | means a subscript          |
| `(` and `)` | means a parentheses        |
| `<` and `>` | means greater or less than |
| `~`         | bits inverse               |
| `&`         | bits and                   |
| `\|`        | bits or                    |
| `^`         | bits xor                   |
| `<<`        | bits left shift            |
| `>>`        | bits right shift           |
| `!`         | logic inverse              |
| `&&`        | logic and                  |
| `\|\|`      | logic or                   |
| `*`         | mul                        |
| `/`         | div                        |
| `%`         | mod                        |
| `+`         | add                        |
| `-`         | sub                        |
| `:=`        | assign                     |
| `==`        | equals to                  |
| `>=`        | greater than or equal      |
| `<=`        | less than or equal         |
| `;`         | end of sentence            |
| `,`         | split items                |
| `"`         | string literal             |
| `.`         | attribute access           |


## Functions Table

(no content yet)

## Escape Table

(no content yet)

## Operators Table

(no content yet)

## Namespace

xParse has 3 namespaces for different kinds of identifiers.

  - **tokens**.  
    all tokens are in a same namespace.

  - **variables**.  
    all variable share a same namespace but in different scopes.

  - **labels** and **rules**.  
    all label and rules share a same namespace.  
    label's name can't be set same with any rule.  
    label's scope is only in local xParse regexp.

## Codegen

(no content yet)

## Virtual Machine and Byte Code Instructions

This part is custom virtual machine for xParse, and optional for users of xParse.

Though, a virtual machine and byte code ISA are not necessary for xParse, 
xParse need a backend to do those real operations.

Users can specify a backend for xParse if they like.
The backend can be LLVM, or a C code generator, 
or other language's code generator, or a custom virtual machine.

But the backend must be compliant conventions specified in [Codegen](#codegen).

### Virtual Machine

The virtual machine included in this project is called `xParse VM`.

xParse VM has a unchangeable register `zero` that is always 0.

xParse VM has three auto-update register `src`, `inst` and `ra`. 
  - The `src` register always refers to the address of current character in input text. <br/>
    The register will automatically increase in normal match, 
    or automatically changed when match context was changed. 
  - The `inst` register always refers to the address of current instruction. <br/>
    The register will automatically increase in normal executing,
    or automatically changed when using `jump` instructions. 
  - The `ra` register keep the address of instruction to return back. <br/>
    Normally, it will be not changed, 
    but it can be automatically changed by function call and function return.

xParse VM has two register to keep the top of two stacks.
  - The `src_top` keep the top address of source stack, which store those match contexts. <br/>
    This register will be automatically update when match context was changed.
  - The `ra_top` keep the top address of return address stack. <br/>
    This register will be automatically update when a function was called or return.

xParse VM also has a register to record internal status of machine, named `status`.
`status` has 3 fields: machine status, regexp status, arith status.
  - machine status has one flag: `vm_mode` that 1 bit. 
    `vm_mode` can be set by instructions.
    When `vm_mode == 1`, means machine is in regexp mode, else means it is in arith mode.
  - regexp status has two flags: `match_result` and `match_mode`, both are 1 bit.
    `match_result` is a read only field, match_mode can be set by instructions.
    When `match_mode == 1`, means inverse match, else means normal match. 

For the convenience of match in regexp, xParse specifies a register `count` 
to determinate how many characters will be compared.
This register can be set by instruction in regexp mode.

### Byte Code Instructions

All instructions of xParse XVM are aligned with 4 bytes. 
Those instructions are specially designed for text process.
There are 9 kinds of instructions, in follow list:

| inst types   | inst structs | explains                                                      |
|:-------------|:-------------|:--------------------------------------------------------------|
| `single`     | C            | do no operation                                               |
| `set_value`  | CI           | set special flag registers' value                             |
| `los_reg`    | CRRI         | load or store, operands are registers                         |
| `load_imm`   | CRI          | load immediate to register, <br/> but no such inst for store  |
| `match_lit`  | CS           | match characters with literals in instructions                |
| `match_reg`  | CRI          | match characters between both strings that registers refer to |
| `ctx_cahnge` | CRI          | change the context of virtual machine                         |
| `jump`       | CII          | jump to inst                                                  |
| `ret`        | C            | return to inst                                                |
| `arith`      | CRRR         | arithmetic instructions                                       |
| `arith_imm`  | CRI          | arithmetic instructions by immediate                          |

here "C" means opcode, "R" means register, "I" means immediate number, "S" means multi chars.
 
Specially, `match_lit`, `match_reg` and `ctx_change` are only for xParse parsing.

`match_reg` instructions depend on register `count`.
When execute `match_reg` instructions, 
the machine will compare characters in `count` times,
and this must be atomic.

#### Instruction Set

**Functional Instructions**:

| inst name     | inst types   | explains                                                                    |
|:--------------|:-------------|:----------------------------------------------------------------------------|
| `nop`         | `single`     | do no operation                                                             |
| `set_vm_mode` | `set_value`  | set machine mode                                                            |
| `set_ma_mode` | `set_value`  | set match mode                                                              |
| `load`        | `los_reg`    | load number                                                                 |
| `sh_mv`       | `los_reg`    | shift move                                                                  |
| `store`       | `los_reg`    | store number                                                                |
| `load_imm`    | `load_imm`   | load immediate to register                                                  |
| `char_lit`    | `match_lit`  | match single character with literal                                         |
| `seq_lit2`    | `match_lit`  | match two consequence characters with two literal characters                |
| `seq_lit3`    | `match_lit`  | match three consequence characters with three literal characters            |
| `set_lit2`    | `match_lit`  | match single character in two literal characters                            |
| `set_lit3`    | `match_lit`  | match single character in three literals characters                         |
| `seq_reg`     | `match_reg`  | match strings referred by two registers, with count are in register `count` |
| `set_reg`     | `match_reg`  | match character in character set, with count are in register `count`        |
| `enter`       | `ctx_cahnge` | push `src` to `SRC_STACK` and increase `src_top`                            |
| `reset`       | `ctx_cahnge` | read value from `src_top`to `src`                                           |
| `exit`        | `ctx_cahnge` | pop value from `SRC_STACK` to `src` and decrease `src_top`                  |
| `jump`        | `jump`       | jump to inst                                                                |
| `ret`         | `single`     | return to inst                                                              |


**Arithmetic instructions**:

| inst name | inst types  | explains                        |
|:----------|:------------|:--------------------------------|
| `add`     | `arith`     | arithmetic add                  |
| `sub`     | `arith`     | arithmetic subtract             |
| `mul`     | `arith`     | arithmetic multiply             |
| `div`     | `arith`     | arithmetic divide               |
| `mod`     | `arith`     | arithmetic modulus              |
| `b_and`   | `arith`     | bits and                        |
| `b_or`    | `arith`     | bits or                         |
| `b_xor`   | `arith`     | bits xor                        |
| `b_lsh`   | `arith`     | bits left shift                 |
| `b_rsh`   | `arith`     | bits right shift                |
| `b_inv`   | `arith`     | bits inverse                    |
| `l_and`   | `arith`     | logic and                       |
| `l_or`    | `arith`     | logic or                        |
| `l_xor`   | `arith`     | logic xor                       |
| `l_inv`   | `arith`     | logic inverse                   |
| `cmp`     | `arith`     | compare                         |
| `add_i`   | `arith_imm` | immediately arithmetic add      |
| `sub_i`   | `arith_imm` | immediately arithmetic subtract |
| `mul_i`   | `arith_imm` | immediately arithmetic multiply |
| `div_i`   | `arith_imm` | immediately arithmetic divide   |
| `mod_i`   | `arith_imm` | immediately arithmetic modulus  |
| `b_and_i` | `arith_imm` | immediately bits and            |
| `b_or_i`  | `arith_imm` | immediately bits or             |
| `b_xor_i` | `arith_imm` | immediately bits xor            |
| `b_lsh_i` | `arith_imm` | immediately bits left shift     |
| `b_rsh_i` | `arith_imm` | immediately bits right shift    |
| `b_inv_i` | `arith_imm` | immediately bits inverse        |
| `l_and_i` | `arith_imm` | immediately logic and           |
| `l_or_i`  | `arith_imm` | immediately logic or            |
| `l_xor_i` | `arith_imm` | immediately logic xor           |
| `l_inv_i` | `arith_imm` | immediately logic inverse       |
| `cmp_i`   | `arith_imm` | immediately compare             |

