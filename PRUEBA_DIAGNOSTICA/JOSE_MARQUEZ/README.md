# Prueba diagnóstica — Lenguajes y Compiladores (2026-II)

- **Estudiante:** José Marquez
- **Video de defensa:** https://drive.google.com/file/d/1ayR_kkLbzH9SYUak1pItaFW32Z6jdNxh/view?usp=drive_link

## Contenido de esta carpeta

```
JOSE_MARQUEZ/
├── README.md
├── ENLACE_VIDEO.md          enlace al video de defensa
├── instalar.bat              verifica Python y gcc y crea las carpetas
├── requirements.txt          sin dependencias externas (solo biblioteca estándar)
├── ejercicio_1/               verificador de movimientos de caballo   [LENGUAJE: Python o C]
├── ejercicio_2/
│   ├── c/                    triángulo de Pascal y polinomio (x+1)^n en C
│   └── python/               triángulo de Pascal y polinomio (x+1)^n en Python
└── ejercicio_3/               palabras reservadas de C traducidas al español (en C)
```

Cada carpeta incluye los `resultados_*.txt` de la corrida que aparece en el video.

## Entorno y preparación

Probado en **Windows 10** con **Python 3.12.10** y **gcc (tdm64-1) 9.2.0 de 64 bits**
(el que trae Embarcadero Dev-C++; se necesita de 64 bits por el tipo `__int128` del ejercicio 2).

1. **Extraer la carpeta completa** antes de ejecutar nada (no abrir archivos desde dentro de un .zip).
2. Abrir `cmd` en la carpeta y ejecutar:
   ```
   instalar.bat
   ```
   El script comprueba Python (lo instala con `winget`, paquete `Python.Python.3.12`, si falta), busca `gcc`
   (si no está en el PATH, usa el de Dev-C++ en su ruta por defecto) y crea la estructura de carpetas.
3. Si `gcc --version` no responde en una ventana nueva de `cmd`, agregar al PATH de Windows la carpeta
   `C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin` (o la ruta donde esté instalado Dev-C++).
4. `requirements.txt` está vacío a propósito: los programas solo usan la biblioteca estándar.

Todos los programas aceptan `--pruebas`, que ejecuta casos de prueba automáticos (por ejemplo `python pascal.py --pruebas`).

## Ejercicio 1 — Movimientos de caballo

Pide la **posición inicial** (ej. `a2`), el **número de movimientos n** y la **cadena** de movimientos (ej. `Kc2 Kd4 Kb3`).
Valida los tres datos (casilla a–h / 1–8, n entero positivo, cadena no vacía con exactamente n movimientos) y
repregunta si alguno es inválido. Después verifica los movimientos y **se detiene en el primer error**:
`movimiento CORRECTO.`, `INCORRECTO falla en el movimiento X.` o `INCORRECTO falla letra Q no pertenece a un caballo.`

Un movimiento es válido si la diferencia entre columnas y filas es (1,2) o (2,1) (salto en L). Cada movimiento
parte de la casilla a la que llegó el anterior.

Medición: tras verificar, repite la verificación **10 veces** y escribe los tiempos en `resultados_*.txt`.
Datos de prueba: `cadena_enunciado.txt` (500 movimientos válidos desde `a2`) y `cadena_con_error.txt` (falla en el movimiento 250);
se copian y se pegan en el programa.

<!-- DEJA SOLO EL BLOQUE DEL LENGUAJE QUE ENTREGASTE Y BORRA ESTA LÍNEA -->
```
cd ejercicio_1
python caballo.py
```
```
cd ejercicio_1
gcc caballo.c -o caballo.exe
caballo.exe
```

## Ejercicio 2 — Triángulo de Pascal y (x+1)^n (Python y C)

Pide un entero **n no negativo**, lo valida (no vacío, solo dígitos, sin signo negativo) y genera el triángulo de
Pascal por programación dinámica (`fila[k] = previa[k-1] + previa[k]`) en **memoria dinámica**
(listas en Python; `malloc`/`free` en C). Muestra los coeficientes, el polinomio y el cálculo paso a paso
de `f(x) = (x+1)^n` para un **x fijo (x = 2)**, que no se pide al usuario.

Si se ingresa **n = 100**, mide el tiempo de generar los polinomios de n = 0 a 100 (1000 repeticiones) y lo escribe en `resultados_*.txt`.

```
cd ejercicio_2\python
python pascal.py

cd ejercicio_2\c
gcc pascal.c -o pascal.exe
pascal.exe
```
(si el enlazador reclama `pow`, agregar `-lm`).

**Límites:** en C los coeficientes son enteros de 128 bits (`__int128`), por lo que n llega hasta **130** y,
si f(x) no cabe en 128 bits, el programa avisa del desbordamiento y da un valor aproximado.
En Python los enteros no tienen límite de tamaño; se fijó n ≤ 2000 porque el triángulo completo ocupa unos 340 MB.

## Ejercicio 3 — Palabras reservadas de C (en C)

Carga un archivo de código C en **memoria dinámica** (`malloc`; la salida crece con `realloc`), recorre **todo** el código y
reemplaza solo las palabras reservadas por su equivalente en español, **resaltándolas** (fondo amarillo en `cmd`;
entre comillas invertidas en el archivo de texto). Reconoce las 44 palabras reservadas de C89, C99 y C11.
No se tocan los comentarios, los textos entre comillas, los números, los nombres que solo contienen una palabra
reservada (`interior`) ni las directivas (`#include <float.h>`).

```
cd ejercicio_3
gcc traductor.c -o traductor.exe
traductor.exe
```
Pide la ruta del archivo (Enter usa `ejemplo.c`). Escribe `codigo_traducido.txt` con el código completo traducido.
Las traducciones están en minúsculas y sin acentos para verse bien en la consola de Windows (`int` → `entero`, `while` → `mientras`, `return` → `retornar`...).

## Observaciones

- Los tiempos se midieron con `QueryPerformanceCounter` (C) y `time.perf_counter()` (Python), compilando con
  `gcc archivo.c -o archivo.exe`, **sin opciones de optimización**. Varían entre ejecuciones y equipos;
  lo relevante es el orden de magnitud (C resulta bastante más rápido que Python en los ejercicios 1 y 2).
- Cada ejecución **sobrescribe** el archivo de resultados; los incluidos son los de la corrida mostrada en el video.
- Interpretaciones tomadas del enunciado: «generar n=100» = polinomios desde n = 0 hasta n = 100;
  «memoria dinámica» = reservar memoria con `malloc` y construir cada fila a partir de la anterior (programación dinámica).
