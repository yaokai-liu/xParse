
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


xRegexp_Object ::= Group | Sequence | CharacterClass 
               | Quantifiered | Labeled | Called | SwitchCase
               | (ATTR xRegexp_Object)


Group ::= "(" xParse_Regexp ")"

Sequence ::= ( PLAIN_CHAR | SINGLE_ESCAPE )+

CharacterClass ::= "[" (PLAIN_CHAR | SINGLE_ESCAPE | Range | CharacterClass | ClassMacro)+ "]"
                 | CLASS_ESCAPE

Quantifiered ::= xRegexp_Object Quantifier

Labeled ::= xRegexp_Object "=" "<" IDENTIFIER ">"

Called ::= ("@" | "$") Refer

SwitchCase ::= "<" ArithExpression "?" xParse_Regexp ">"

Range ::= PLAIN_CHAR "-" PLAIN_CHAR

ClassMacro ::= ":" IDENTIFIER ":"

Quantifier ::= "{" ArithExpression? ","? ArithExpression? ";"? ArithExpression? "}"
             | QUANTIFIER_MACRO

Refer ::= "<" Literal ">"

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

Identifierized ::= IDENTIFIER | Captured | ValueLiteral

Captured ::= "$" (IDENTIFIER | NUMBER) ("." (IDENTIFIER | NUMBER))?

ValueLiteral ::= NUMBER | STRING | BOOL
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

All escapes please name the [escape table](#escape-table).

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
When creating a group, there will create a refer for it,
and name of the refer always is a number, according to the `(`'s orders.

### Branch

**Definition:**
`Branch`s are those xParse regexps split by `|`, and every part should not include a `|` stands directly.
Those parts are called `Branch`.

**Feature:**
Branches in a group share the group's inner common environment.

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

### Refer

**Definition:**
A `Refer` is a number or an identifier bracketed with `<` and `>`.

`Refer`s can be created by `=` after a `ReObj`, also can be used led by `@` or `$`.

When a refer is creating, it will name to the previous `ReObj`,
and create a capture to store its last matching result.

Specially, in xParse, when call a refer, its name can be a rule name, or a token.
If a called name is a rule, means it will call the rule to process the input.
If a called name is a token, means it will look up all rules that return this kind of token to find a suitable rule to process the input. 

**Feature:**
When using `@` to call a refer, executor will execute its referring, and update its capture;
When using `$` to call a refer, executor will match what it last captured.

**Restrict:**
When creating a refer by `=`, the refer name must be identifier, numbers are forbidden.

### Capture

**Definition:**
A `Capture` is a piece of memory space to store results of executing xParse regexp.

A capture will be automatically created, and only for a group or a refer.

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
An `ArithExpression` is an arithmetic expression in `{` and `}` brackets in a xParser expression
or in statements in a xParser program.

In xParser programs, expressions always are part of statements.

There are some different expressions:
- **Literal Values:**
  Literal values are those numbers or strings without variables.

- **Variables**:
  variables are those identifiers consistent with only lower letters, digits and underscore, and must start with lower letters.

- **Literals:**
  Literal values, variables, captured constants in xRegexp are all called literals.

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
arithmetic expressions of xParser program all are lazy evaluated,
because xParser program is what user tells to xParser executor only how the result `$` is computed.


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
| `<` and `>` | means a refer or switch case                                               |
| `~`         | means at begin of match                                                    |
| `!`         | means parse but not match                                                  |
| `@`         | means call a refer to match                                                |
| `$`         | means parse the result of refer                                            |
| `^`         | means inverse match                                                        |
| `&`         | means at end of match                                                      |
| `\|`        | means branch                                                               |
| `*`         | quantifier macro, means `{0,+∞; 1}`                                        |
| `+`         | quantifier macro, means `{1,+∞; 1}`                                        |
| `?`         | quantifier macro, means `{0,1; 1}`;<br/> or means options in a switch case |
| `-`         | character range connector                                                  |
| `=`         | means set refer                                                            |
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

  - **refers** and **rules**.  
    all refer and rules share a same namespace. 
    names of refers can't be set same with any rule. 
    scope of refers is only in local xParse regexp.

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
#### Registers
xParse VM has an unchangeable register `zero` that is always 0.

| registers     | RSW | explains                                |
|:--------------|:----|:----------------------------------------|
| `zero`        | R   | always 0                                |
| `reset_src`   | R   | if match failed, address to reset `src` |
| `ret_addr`    | R   | return address of function              |
| `regex_level` | R   | regex environment level                 |
| `call_level`  | R   | function call level                     |
| `status`      | RS  | status register                         |
| `stack_top`   | R   | stack top for function context          |
| `src`         | R W | refers offset from head of input text   |
| `inst`        | R W | refers to current executing instruction |
| `count`       | R W | count register for match instruction    |
| `jump_base`   | R W | base address of jump instruction        |
| `call_vec`    | R W | function table for call                 |
| `arith[0-15]` | R W | 16 arithmetic registers                 |

### Byte Code Instructions

All instructions of xParse XVM are aligned with 4 bytes. 
Those instructions are specially designed for text process.
There are 9 kinds of instructions, in follow list:

| inst types  | inst structs | explains                                                      |
|:------------|:-------------|:--------------------------------------------------------------|
| `single`    | C            | do no operation                                               |
| `set_value` | CI           | set special flag registers' value                             |
| `msl_reg`   | CRRI         | load or store, operands are registers                         |
| `load_imm`  | CRI          | load immediate to register, <br/> but no such inst for store  |
| `match_lit` | CS           | match characters with literals in instructions                |
| `match_reg` | CRI          | match characters between both strings that registers name to  |
| `jump`      | CII          | jump to inst                                                  |
| `arith_reg` | CRRR         | arithmetic instructions                                       |
| `arith_imm` | CRI          | arithmetic instructions with immediate                        |
| `cmp_reg`   | CRRI         | comparing instructions                                        |
| `cmp_imm`   | CRI          | comparing instructions with immediate                         |

here "C" means opcode, "R" means register, "I" means immediate number, "S" means multi chars.
 
Specially, `match_lit`, `match_reg` and `ctx_change` are only for xParse parsing.

`match_reg` instructions depend on register `count`.
When execute `match_reg` instructions, 
the machine will compare characters in `count` times,
and this must be atomic.

#### Instruction Set

**Functional Instructions**:

| inst name        | inst types  | explains                                                                                                      |
|:-----------------|:------------|:--------------------------------------------------------------------------------------------------------------|
| `nop`            | `single`    | do no operation                                                                                               |
| `set_vm_mode`    | `set_value` | set machine mode                                                                                              |
| `set_ma_mode`    | `set_value` | set match mode                                                                                                |
| `clear_ma_flag`  | `single`    | clear the match flag field of status register                                                                 |
| `clear_cmp_flag` | `single`    | clear the compare flag field of status register                                                               |
| `load`           | `msl_reg`   | load number from memory                                                                                       |
| `shift_move`     | `msl_reg`   | shift move                                                                                                    |
| `store`          | `msl_reg`   | store number to memory                                                                                        |
| `load_imm`       | `load_imm`  | load immediate to register                                                                                    |
| `seq_lit1`       | `match_lit` | match single character with literal                                                                           |
| `seq_lit2`       | `match_lit` | match two consequence characters with two literal characters                                                  |
| `seq_lit3`       | `match_lit` | match three consequence characters with three literal characters                                              |
| `set_lit1`       | `match_lit` | match single character in literal                                                                             |
| `set_lit2`       | `match_lit` | match single character in two literal characters                                                              |
| `set_lit3`       | `match_lit` | match single character in three literals characters                                                           |
| `seq_reg`        | `match_reg` | match strings referred by two registers, with count are in register `count`                                   |
| `set_reg`        | `match_reg` | match character in character set, with count are in register `count`                                          |
| `enter`          | `single`    | push `reset_src_reg` to `SRC_STACK`, <br/>copy `src_reg` to `reset_src_reg` and increase `src_level`          |
| `reset`          | `single`    | copy `reset_src_reg` to `src_reg`                                                                             |
| `exit`           | `single`    | pop out top value from `SRC_STACK` to `reset_src_reg`, decrease `src_level`                                   |
| `jump_directly`  | `jump`      | jump to inst directly                                                                                         |
| `jump_if_eq`     | `jump`      | jump to inst if eq_flag on                                                                                    |
| `jump_if_ne`     | `jump`      | jump to inst if eq_flag off                                                                                   |
| `jump_if_lt`     | `jump`      | jump to inst if lt_flag on                                                                                    |
| `jump_if_gt`     | `jump`      | jump to inst if gt_flag on                                                                                    |
| `jump_if_le`     | `jump`      | jump to inst if eq_flag and lt_flag on                                                                        |
| `jump_if_ge`     | `jump`      | jump to inst if eq_flag and gt_flag on                                                                        |
| `jump_if_ma`     | `jump`      | jump to inst if match_flag on                                                                                 |
| `jump_if_nm`     | `jump`      | jump to inst if match_flag off                                                                                |
| `call`           | `jump`      | push `ra` to `RA_STACK` and increase `ra_level`, <br/>and set `ra` be next inst, jump accoding `call_vec_reg` |
| `ret`            | `single`    | return to `ra` and pop inst to `ra` from `RA_STACK` and decrease `ra_level`                                   |
| `cmp`            | `cmp_reg`   | compare                                                                                                       |
| `cmp_i`          | `cmp_imm`   | immediately compare                                                                                           |


**Arithmetic instructions**:

| inst name | inst types  | explains                        |
|:----------|:------------|:--------------------------------|
| `add`     | `arith_reg` | arithmetic add                  |
| `sub`     | `arith_reg` | arithmetic subtract             |
| `mul`     | `arith_reg` | arithmetic multiply             |
| `div`     | `arith_reg` | arithmetic divide               |
| `mod`     | `arith_reg` | arithmetic modulus              |
| `b_and`   | `arith_reg` | bits and                        |
| `b_or`    | `arith_reg` | bits or                         |
| `b_xor`   | `arith_reg` | bits xor                        |
| `b_lsh`   | `arith_reg` | bits left shift                 |
| `b_rsh`   | `arith_reg` | bits right shift                |
| `b_inv`   | `arith_reg` | bits inverse                    |
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
