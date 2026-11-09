use std::env;
use std::hint::black_box;
use std::time::{Duration, Instant};

const MOVIMIENTOS_500: &str = include_str!("../movimientos.txt");

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
struct Posicion {
    fila: i32,
    columna: i32,
}

#[derive(Debug, PartialEq, Eq)]
enum ErrorMovimiento {
    LetraNoCaballo(char),
    MovimientoInvalido(usize),
}

fn parsear_casilla(texto: &str) -> Option<Posicion> {
    let bytes = texto.as_bytes();
    if bytes.len() != 2 {
        return None;
    }
    let columna = bytes[0] as char;
    if !('a'..='h').contains(&columna) {
        return None;
    }
    let fila = bytes[1] as char;
    if !('1'..='8').contains(&fila) {
        return None;
    }
    Some(Posicion {
        fila: fila.to_digit(10).unwrap() as i32,
        columna: (columna as u8 - b'a') as i32,
    })
}

fn es_movimiento_de_caballo(origen: Posicion, destino: Posicion) -> bool {
    let df = (origen.fila - destino.fila).abs();
    let dc = (origen.columna - destino.columna).abs();
    matches!((df, dc), (1, 2) | (2, 1))
}

fn verificar(inicial: &str, n: usize, movimientos: &[String]) -> Result<(), ErrorMovimiento> {
    let mut posicion = parsear_casilla(inicial).expect("posicion inicial valida");

    for indice in 0..n {
        let numero = indice + 1;
        let movimiento = match movimientos.get(indice) {
            Some(movimiento) => movimiento,
            None => return Err(ErrorMovimiento::MovimientoInvalido(numero)),
        };
        let bytes = movimiento.as_bytes();

        if bytes.is_empty() {
            return Err(ErrorMovimiento::MovimientoInvalido(numero));
        }
        if bytes[0] != b'K' {
            return Err(ErrorMovimiento::LetraNoCaballo(bytes[0] as char));
        }
        if bytes.len() != 3 {
            return Err(ErrorMovimiento::MovimientoInvalido(numero));
        }

        let destino = match parsear_casilla(&movimiento[1..]) {
            Some(destino) => destino,
            None => return Err(ErrorMovimiento::MovimientoInvalido(numero)),
        };

        if !es_movimiento_de_caballo(posicion, destino) {
            return Err(ErrorMovimiento::MovimientoInvalido(numero));
        }
        posicion = destino;
    }

    Ok(())
}

fn mensaje(resultado: &Result<(), ErrorMovimiento>) -> String {
    match resultado {
        Ok(()) => "movimiento CORRECTO.".to_string(),
        Err(ErrorMovimiento::LetraNoCaballo(letra)) => {
            format!("movimiento INCORRECTO falla letra {letra} no pertenece a un caballo.")
        }
        Err(ErrorMovimiento::MovimientoInvalido(numero)) => {
            format!("movimiento INCORRECTO falla en el movimiento {numero}.")
        }
    }
}

fn verificar_texto(inicial: &str, n: usize, cadena: &str) -> Result<(), ErrorMovimiento> {
    let movimientos: Vec<String> = cadena.split_whitespace().map(str::to_string).collect();
    verificar(inicial, n, &movimientos)
}

fn ejecutar_ejemplos() {
    let ejemplos: [(&str, usize, &str); 9] = [
        ("a1", 4, "Kc2 Kd4 Kb3 Kc5"),
        ("a1", 4, "Kc2 Kd4 Kb4 Kc5"),
        ("a2", 5, "Kc3 Ke4 Kf6 Ke8 Kg7"),
        ("a2", 5, "Kc8 Ke4 Kf6 Ke8 Kg7"),
        ("a8", 2, "Kb6 Kd7"),
        ("a8", 2, "Kb6 Kb7"),
        ("a1", 10, "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kc6 Kb4"),
        ("a1", 10, "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kc6 Qb4"),
        ("a1", 10, "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kh6 Kb4"),
    ];

    println!("=== Ejemplos del enunciado ===");
    for (inicial, n, cadena) in ejemplos {
        let resultado = verificar_texto(inicial, n, cadena);
        println!("Ak = {inicial} n = {n} y C = {cadena}");
        println!("salida: {}\n", mensaje(&resultado));
    }
}

fn formatear_tiempo(duracion: Duration) -> String {
    let micros = duracion.as_secs_f64() * 1_000_000.0;
    if micros < 1_000.0 {
        format!("{micros:.3} us")
    } else {
        format!("{:.3} ms", micros / 1_000.0)
    }
}

fn ejecutar_benchmark() {
    let movimientos: Vec<String> = MOVIMIENTOS_500
        .split_whitespace()
        .map(str::to_string)
        .collect();

    assert_eq!(
        movimientos.len(),
        500,
        "el archivo movimientos.txt debe contener 500 movimientos"
    );

    println!("=== Verificacion de 500 movimientos desde a2 (10 repeticiones) ===");
    let mut total = Duration::ZERO;
    let mut tiempos = Vec::with_capacity(10);

    for corrida in 1..=10 {
        let inicio = Instant::now();
        let resultado = verificar("a2", movimientos.len(), &movimientos);
        let duracion = inicio.elapsed();
        total += duracion;
        tiempos.push(duracion);
        black_box(&resultado);

        println!(
            "corrida {corrida:>2}: {:>12} -> {}",
            formatear_tiempo(duracion),
            mensaje(&resultado)
        );
    }

    let minimo = *tiempos.iter().min().expect("hay tiempos medidos");
    let mut ordenados = tiempos.clone();
    ordenados.sort();
    let mediana = ordenados[ordenados.len() / 2];

    println!("\nTiempo total: {}", formatear_tiempo(total));
    println!(
        "Tiempo promedio por verificacion: {}",
        formatear_tiempo(total / 10)
    );
    println!("Tiempo minimo: {}", formatear_tiempo(minimo));
    println!("Mediana: {}", formatear_tiempo(mediana));
}

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();

    if args.is_empty() {
        ejecutar_ejemplos();
        ejecutar_benchmark();
        return;
    }

    if args[0] == "--bench" || args[0] == "-b" {
        ejecutar_benchmark();
        return;
    }

    if args.len() < 2 {
        eprintln!("Uso: caballo <posicion_inicial> <n> [movimientos...]");
        eprintln!("     caballo a1 4 \"Kc2 Kd4 Kb3 Kc5\"");
        eprintln!("     caballo --bench");
        std::process::exit(2);
    }

    let inicial = &args[0];
    if parsear_casilla(inicial).is_none() {
        eprintln!("Posicion inicial invalida: {inicial}");
        std::process::exit(2);
    }

    let n: usize = match args[1].parse() {
        Ok(n) => n,
        Err(_) => {
            eprintln!("El numero de movimientos debe ser un entero no negativo.");
            std::process::exit(2);
        }
    };

    let cadena = args[2..].join(" ");
    let resultado = verificar_texto(inicial, n, &cadena);
    println!("salida: {}", mensaje(&resultado));
}
