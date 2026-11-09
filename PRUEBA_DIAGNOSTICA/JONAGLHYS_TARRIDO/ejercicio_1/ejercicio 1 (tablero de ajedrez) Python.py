import time

def es_movimiento_caballo(p1, p2):
    dc = abs(ord(p1[0]) - ord(p2[0]))
    dr = abs(int(p1[1]) - int(p2[1]))
    return (dc == 2 and dr == 1) or (dc == 1 and dr == 2)

def validar_movimientos(ak, movimientos):
    if len(ak) != 2 or ak[0] not in 'abcdefgh' or not ak[1].isdigit() or not (1 <= int(ak[1]) <= 8):
        return False, 0, "Posicion inicial invalida"
    
    actual = ak
    for i, mov in enumerate(movimientos):
        if not mov.startswith('K') or len(mov) != 3:
            return False, i + 1, f"falla letra {mov[0]} no pertenece a un caballo o formato incorrecto"
        
        siguiente = mov[1:]
        sig_col, sig_row = siguiente[0], siguiente[1]
        
        if sig_col not in 'abcdefgh' or not sig_row.isdigit() or not (1 <= int(sig_row) <= 8):
            return False, i + 1, f"falla en el movimiento {i + 1}"
            
        if not es_movimiento_caballo(actual, siguiente):
            return False, i + 1, f"falla en el movimiento {i + 1}"
            
        actual = siguiente
        
    return True, 0, "movimiento CORRECTO"

# Datos de la prueba
ak = "b4"
lista_movs = [
    "Ka6", "Kc5", "Ka4", "Kc3", "Ka2", "Kc1", "Ke2", "Kg1", "Kh3", "Kg5",
    "Ke6", "Kc7", "Ka8", "Kc7", "Kb5", "Kd6", "Kb7", "Ka5", "Kc4", "Ka3",
    "Kc2", "Ka1", "Kc2", "Kb4", "Ka6", "Kb8", "Kd7", "Kb6", "Ka8", "Kc7",
    "Ka6", "Kc5", "Ka4", "Kc3", "Ka2", "Kc1", "Ke2", "Kg3", "Kh5", "Kg7",
    "Ke8", "Kc7", "Ka8", "Kc7", "Kb5", "Kd4", "Kb3", "Ka1", "Kc2", "Ka3"
]

# Medición de tiempo para 100 ejecuciones
inicio = time.time()

for _ in range(100):
    valido, idx, mensaje = validar_movimientos(ak, lista_movs)

fin = time.time()
tiempo_python = fin - inicio

# Para guardar en archivo txt
with open("resultado_ejercicio1_py.txt", "w") as archivo:
    archivo.write("--- RESULTADOS EJERCICIO 1 (Python) ---\n")
    archivo.write("Iteraciones: 100\n")
    if valido:
        archivo.write("Salida: movimiento CORRECTO\n")
    else:
        archivo.write(f"Salida: INCORRECTO - {mensaje}\n")
    archivo.write(f"Tiempo de ejecucion: {tiempo_python:.6f} segundos\n")

print(f"Prueba del tablero de ajedrez en Python finalizada.")
print(f"Tiempo medido: {tiempo_python:.6f} segundos.")
