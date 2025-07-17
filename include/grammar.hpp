#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>

namespace Autumn {

// "Autum Grammar"
constexpr const char *autumngrammar = R"(
x           := a | b | ... | aa ...
program     := statement*
statement   := externaldecl | assignexpr | typedecl | typedef

typedef     := object typesymbol fields \{\}
typealias   := "type alias" type fields
fields      := field | fields field
field       := constructor | constructor typesymbol*
constructor := typesymbol

typedecl    := x : typeexpr
externaldecl:= external typedecl

assignexpr  := x = valueexpr

typeexpr    := typesymbol | paramtype | typevar | functiontype
funtype     := typeexpr -> typeexpr
producttype := typeexpr × typexexpr × ...
typesymbol  := primtype | customtype
primtype    := Int | Bool | Float
customtype  := A | B | ... | Aa | ...

valueexpr   := fappexpr | lambdaexpr | iteexpr | initnextexpr | letexpr |
               this | lambdaexpr
iteexpr     := if expr then expr else expr
intextexpr  := init expr next expr
fappexpr    := valueexpr valueexpr*
letexpr     := let x = valueexpr in valueexpr
lambdaexpr  := x --> expr
)";

} // namespace Autumn

#endif // GRAMMAR_H
