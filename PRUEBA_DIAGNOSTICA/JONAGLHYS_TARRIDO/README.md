# 📋 Documentación Técnica: Prueba Diagnóstica

* **Estudiante:** Jonaglhys Gabriela Tarrido Azócar
* **Asignatura:** Lenguajes y Compiladores
* **Sección:** 2

---

## 🎯 Definición y Alcance de la Práctica
El presente repositorio contiene la resolución formal de la prueba diagnóstica de la cátedra, enfocada en la validación algorítmica, la gestión eficiente de memoria dinámica, el análisis léxico de código fuente y la evaluación comparativa de rendimiento computacional entre lenguajes de alto y bajo nivel.

---

## ⚙️ Especificaciones y Observaciones por Ejercicio

### ♞ Ejercicio 1: Validador de Movimientos del Caballo de Ajedrez
* **Definición:** Algoritmo de validación secuencial para un conjunto de hasta 500 movimientos del caballo partiendo de la coordenada inicial $a2$.
* **Observaciones y Criterios de Resolución:**
  * **Validación Léxica:** Se verifica el formato estricto del token, exigiendo el identificador de pieza ('K') y la acotación dentro de las coordenadas válidas del tablero ($a-h$ para columnas, $1-8$ para filas).
  * **Validación Geométrica:** Se implementó una verificación de movimiento basada en el patrón en "L" mediante desplazamientos absolutos estrictos ($\Delta x = 2 \land \Delta y = 1$, o viceversa). Ante cualquier anomalía, el sistema interrumpe la ejecución para reportar la falla exacta.

### 📐 Ejercicio 2: Triángulo de Pascal y Polinomios con Memoria Dinámica
* **Definición:** Generación de coeficientes binomiales correspondientes a la expansión de $(x+1)^n$ y análisis comparativo de rendimiento computacional.
* **Observaciones y Criterios de Resolución:**
  * **Gestión de Memoria:** Construcción iterativa del triángulo utilizando asignación dinámica de memoria en tiempo de ejecución para optimizar los recursos.
  * **Evaluación de Rendimiento:** Se incorporó una comparativa de tiempos de ejecución para $n=100$ iteraciones contrastando la eficiencia de compilación en **C** frente a la interpretación en **Python**, evidenciando el comportamiento de bajo nivel en el manejo de memoria.

### 🔍 Ejercicio 3: Analizador Léxico y Traductor de Palabras Reservadas en C
* **Definición:** Escáner léxico diseñado para la lectura, segmentación y traducción de palabras clave del lenguaje C a español.
* **Observaciones y Criterios de Resolución:**
  * **Carga en Buffer:** Lectura optimizada del archivo fuente mediante punteros y funciones de manejo de archivos (`malloc`, `fseek`, `fread`) para procesar el texto carácter por carácter.
  * **Estructura de Datos y Resumen:** El sistema aísla los identificadores, contrasta contra un diccionario de palabras reservadas e implementa un contador de frecuencia. Esto evita duplicados innecesarios en la salida y genera un informe estructurado y limpio en el archivo `resultado_ejercicio3.txt`.

---

## 🎥 Enlace de Defensa
El video se encuentra especificado en el archivo `ENLACE_VIDEO.md`.
