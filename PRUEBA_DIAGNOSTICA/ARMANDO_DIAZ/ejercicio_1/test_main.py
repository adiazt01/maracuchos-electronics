import unittest

from main import check_path, is_knight_move, notation_to_coords


class TestCoordenada(unittest.TestCase):
    def test_a1(self):
        self.assertEqual(notation_to_coords("a1"), (0, 0))

    def test_c2(self):
        self.assertEqual(notation_to_coords("c2"), (2, 1))

    def test_h8(self):
        self.assertEqual(notation_to_coords("h8"), (7, 7))


class TestSaltoCaballo(unittest.TestCase):
    def test_salto_valido(self):
        self.assertTrue(is_knight_move("a1", "c2"))

    def test_no_es_salto(self):
        self.assertFalse(is_knight_move("d4", "b4"))


class TestVerificar(unittest.TestCase):
    def test_ejemplos_del_enunciado(self):
        casos = [
            ("a1", 4, "Kc2 Kd4 Kb3 Kc5", "movimiento CORRECTO."),
            ("a1", 4, "Kc2 Kd4 Kb4 Kc5", "INCORRECTO falla en el movimiento 3."),
            ("a2", 5, "Kc3 Ke4 Kf6 Ke8 Kg7", "movimiento CORRECTO."),
            ("a2", 5, "Kc8 Ke4 Kf6 Ke8 Kg7", "INCORRECTO falla en el movimiento 1."),
            ("a8", 2, "Kb6 Kd7", "movimiento CORRECTO."),
            ("a8", 2, "Kb6 Kb7", "INCORRECTO falla en el movimiento 2."),
            (
                "a1",
                10,
                "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kc6 Kb4",
                "movimiento CORRECTO.",
            ),
            (
                "a1",
                10,
                "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kc6 Qb4",
                "INCORRECTO falla letra Q no pertenece a un caballo.",
            ),
            (
                "a1",
                10,
                "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kh6 Kb4",
                "INCORRECTO falla en el movimiento 9.",
            ),
        ]
        for ak, n, cadena, esperado in casos:
            with self.subTest(ak=ak, cadena=cadena):
                self.assertEqual(check_path(ak, n, cadena), esperado)


if __name__ == "__main__":
    unittest.main()
