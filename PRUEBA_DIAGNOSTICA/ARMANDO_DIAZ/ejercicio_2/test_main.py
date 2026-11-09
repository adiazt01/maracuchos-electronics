import unittest
from main import pascal_row, format_polynomial, evaluate_step_by_step


class TestPascalRow(unittest.TestCase):
    def test_row_0(self):
        self.assertEqual(pascal_row(0), [1])

    def test_row_1(self):
        self.assertEqual(pascal_row(1), [1, 1])

    def test_row_4(self):
        self.assertEqual(pascal_row(4), [1, 4, 6, 4, 1])

    def test_row_7(self):
        self.assertEqual(pascal_row(7), [1, 7, 21, 35, 35, 21, 7, 1])


class TestFormatPolynomial(unittest.TestCase):
    def test_row_0(self):
        self.assertEqual(format_polynomial([1]), "1")

    def test_row_2(self):
        self.assertEqual(format_polynomial([1, 2, 1]), "x^2 + 2x + 1")

    def test_row_4(self):
        self.assertEqual(format_polynomial(pascal_row(4)), "x^4 + 4x^3 + 6x^2 + 4x + 1")


class TestEvaluate(unittest.TestCase):
    def test_x_1_n_2(self):
        # (1+1)^2 = 4
        self.assertEqual(evaluate_step_by_step(pascal_row(2), 1)[1], 4)

    def test_x_2_n_4(self):
        # (2+1)^4 = 81
        self.assertEqual(evaluate_step_by_step(pascal_row(4), 2)[1], 81)

    def test_x_3_n_4(self):
        # (3+1)^4 = 256
        self.assertEqual(evaluate_step_by_step(pascal_row(4), 3)[1], 256)


if __name__ == "__main__":
    unittest.main()
