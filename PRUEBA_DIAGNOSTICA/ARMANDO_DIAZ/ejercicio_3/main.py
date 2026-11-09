import re
import sys
from typing import TypeAlias

Counts: TypeAlias = dict[str, int]

KEYWORDS: dict[str, str] = {
    "auto": "automático",
    "break": "romper",
    "case": "caso",
    "char": "carácter",
    "const": "constante",
    "continue": "continuar",
    "default": "defecto",
    "do": "hacer",
    "double": "doble",
    "else": "sino",
    "enum": "enumeración",
    "extern": "externo",
    "float": "flotante",
    "for": "para",
    "goto": "ir_a",
    "if": "si",
    "inline": "en_línea",
    "int": "entero",
    "long": "largo",
    "register": "registro",
    "restrict": "restringido",
    "return": "retornar",
    "short": "corto",
    "signed": "con_signo",
    "sizeof": "tamaño_de",
    "static": "estático",
    "struct": "estructura",
    "switch": "según",
    "typedef": "tipo_def",
    "union": "unión",
    "unsigned": "sin_signo",
    "void": "vacío",
    "volatile": "volátil",
    "while": "mientras",
    "_Alignas": "alinear_como",
    "_Alignof": "alineación_de",
    "_Atomic": "atómico",
    "_Bool": "booleano",
    "_Complex": "complejo",
    "_Generic": "genérico",
    "_Imaginary": "imaginario",
    "_Noreturn": "no_retorna",
    "_Static_assert": "afirmar_estático",
    "_Thread_local": "hilo_local",
}

TOKEN_PATTERN = re.compile(
    r"//[^\n]*"  # comentario de una línea
    r"|/\*.*?\*/"  # comentario de varias líneas
    r'|"(?:\\.|[^"\\])*"'  # cadena de texto
    r"|'(?:\\.|[^'\\])*'"  # carácter
    r"|\d[A-Za-z0-9_.]*"  # número
    r"|[A-Za-z_][A-Za-z0-9_]*",  # identificador o palabra reservada
    re.DOTALL,
)


def translate_source(source: str) -> tuple[str, Counts]:
    counts: Counts = {}

    def replace(match: re.Match[str]) -> str:
        token = match.group(0)
        if token in KEYWORDS:
            counts[token] = counts.get(token, 0) + 1
            return KEYWORDS[token]
        return token

    return TOKEN_PATTERN.sub(replace, source), counts


def format_report(counts: Counts) -> str:
    if not counts:
        return "No se encontraron palabras reservadas de C."
    lines = [f"Se encontraron {sum(counts.values())} palabras reservadas:"]
    for word, count in sorted(counts.items()):
        lines.append(f"  {word} -> {KEYWORDS[word]}  ({count})")
    return "\n".join(lines)


def main() -> None:
    path = sys.argv[1] if len(sys.argv) > 1 else "ejemplo.c"
    with open(path, encoding="utf-8") as source_file:
        source = source_file.read()

    translated, counts = translate_source(source)

    with open("programa_traducido.c", "w", encoding="utf-8") as output_file:
        output_file.write(translated)

    print(format_report(counts))
    print("\nCódigo traducido guardado en programa_traducido.c")


if __name__ == "__main__":
    main()
