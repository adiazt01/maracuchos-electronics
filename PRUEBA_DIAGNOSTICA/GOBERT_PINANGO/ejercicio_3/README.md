# Ejercicio 3 — Palabras reservadas de C (C)

## Enunciado

Para un programa escrito en C, cargado en memoria de forma dinámica, verificar si existen palabras reservadas y traducirlas a su equivalente en español.

## Estructura

| Archivo | Descripción |
|---|---|
| `traductor.c` | Implementación del cargador, analizador y traductor |
| `ejemplo.c` | Programa C de prueba con palabras reservadas en código, comentarios y cadenas |
| `Makefile` | Compilación |
| `instalar.sh` | Verificación/instalación de gcc y make |
| `generar_resultados.sh` | Regenera `resultados_c.txt` y `codigo_traducido.txt` |
| `resultados_c.txt` | Informe de la corrida de referencia |
| `codigo_traducido.txt` | Código de `ejemplo.c` con las palabras reservadas traducidas |
| `README.md` | Este documento |

## Instalación y compilación

```bash
./instalar.sh
make
```

## Uso

```bash
./traductor [archivo.c]
./traductor [archivo.c] --traducir
```

- Si se omite el archivo, se usa `ejemplo.c`.
- Sin opciones: muestra un informe con las palabras reservadas encontradas, su equivalente en español y el número de ocurrencias.
- `--traducir` o `-t`: escribe en `stdout` el código con las palabras reservadas reemplazadas. El informe se envía a `stderr` para poder redirigir solo el código traducido.
- `-h` o `--ayuda`: muestra la ayuda.

### Ejemplos

```bash
./traductor ejemplo.c
./traductor ejemplo.c --traducir > ejemplo_traducido.c
```

## Algoritmo

1. **Carga dinámica:** el archivo se abre en modo binario, se determina su tamaño con `fseek`/`ftell` y se reserva un buffer con `malloc`. El contenido se lee con `fread` y se termina con `'\0'`.
2. **Análisis léxico:** se recorre el buffer carácter por carácter.
   - Los comentarios `//...` y `/*...*/` se copian sin analizar su contenido.
   - Las cadenas `"..."` y los caracteres `'...'` se copian respetando los escapes.
   - Los identificadores se detectan con `[A-Za-z_][A-Za-z0-9_]*` y se comparan contra una tabla de palabras reservadas.
3. **Traducción:** si el identificador es una palabra reservada, se incrementa su contador y, en modo `--traducir`, se escribe su equivalente en español. En caso contrario, se copia el identificador original.
4. **Salida:** se genera el informe y/o el código traducido en un buffer dinámico que crece con `realloc`.
5. **Liberación:** se liberan el buffer del archivo, el buffer de salida y los recursos utilizados.

Las palabras reservadas dentro de comentarios, cadenas o caracteres **no** se cuentan ni se traducen.

## Tabla de equivalencias

### C89 / C90

| Inglés | Español | Inglés | Español |
|---|---|---|---|
| `auto` | `automatico` | `break` | `romper` |
| `case` | `caso` | `char` | `caracter` |
| `const` | `constante` | `continue` | `continuar` |
| `default` | `predeterminado` | `do` | `hacer` |
| `double` | `doble` | `else` | `sino` |
| `enum` | `enumeracion` | `extern` | `externo` |
| `float` | `flotante` | `for` | `para` |
| `goto` | `ir_a` | `if` | `si` |
| `int` | `entero` | `long` | `largo` |
| `register` | `registro` | `return` | `retornar` |
| `short` | `corto` | `signed` | `con_signo` |
| `sizeof` | `tamano_de` | `static` | `estatico` |
| `struct` | `estructura` | `switch` | `segun` |
| `typedef` | `definir_tipo` | `union` | `union` |
| `unsigned` | `sin_signo` | `void` | `vacio` |
| `volatile` | `volatil` | `while` | `mientras` |

### C99

| Inglés | Español |
|---|---|
| `inline` | `en_linea` |
| `restrict` | `restringido` |
| `_Bool` | `_Booleano` |
| `_Complex` | `_Complejo` |
| `_Imaginary` | `_Imaginario` |

### C11

| Inglés | Español |
|---|---|
| `_Alignas` | `_Alinear` |
| `_Alignof` | `_Alineacion_de` |
| `_Atomic` | `_Atomico` |
| `_Generic` | `_Generico` |
| `_Noreturn` | `_SinRetorno` |
| `_Static_assert` | `_Afirmacion_estatica` |
| `_Thread_local` | `_Hilo_local` |

Los equivalentes se escriben sin acentos para que puedan usarse como identificadores en el código traducido. La traducción es didáctica: no pretende producir un programa compilable en español, sino evidenciar la detección y sustitución de palabras reservadas.

## Ejemplo de informe

Salida de `./traductor ejemplo.c` (fragmento):

```text
Palabras reservadas encontradas en "ejemplo.c":
  break          -> romper             (2 ocurrencias)
  case           -> caso               (1 ocurrencia)
  char           -> caracter           (1 ocurrencia)
  const          -> constante          (3 ocurrencias)
  ...
  int            -> entero             (21 ocurrencias)
  ...
  _Bool          -> _Booleano          (1 ocurrencia)

Total: 99 ocurrencia(s) de 34 palabra(s) reservada(s) distinta(s).
```

## Validación

- `ejemplo.c` es un programa C válido: `gcc -std=c11 -Wall -Wextra -Wpedantic -fsyntax-only ejemplo.c` no reporta errores.
- Se comprobó que las palabras reservadas dentro de comentarios, cadenas y caracteres no se cuentan.
- Se comprobó que identificadores como `intentos` no se confunden con la palabra reservada `int`.
- La opción `--traducir` conserva comentarios y cadenas intactos y sustituye únicamente las palabras reservadas del código.
