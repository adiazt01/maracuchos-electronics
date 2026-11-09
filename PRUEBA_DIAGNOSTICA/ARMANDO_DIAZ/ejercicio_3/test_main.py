import unittest
from main import translate_source, format_report


class TestTranslate(unittest.TestCase):
    def test_declaration(self):
        text, counts = translate_source("int x = 0;")
        self.assertEqual(text, "entero x = 0;")
        self.assertEqual(counts, {"int": 1})

    def test_if_else_return(self):
        text, _ = translate_source("if (a) return 1; else b;")
        self.assertEqual(text, "si (a) retornar 1; sino b;")

    def test_identifier_containing_keyword_is_kept(self):
        text, counts = translate_source("interior = 2;")
        self.assertEqual(text, "interior = 2;")
        self.assertEqual(counts, {})

    def test_string_is_kept(self):
        text, counts = translate_source('printf("while int");')
        self.assertEqual(text, 'printf("while int");')
        self.assertEqual(counts, {})

    def test_line_comment_is_kept(self):
        text, _ = translate_source("// for aqui\nint y;")
        self.assertEqual(text, "// for aqui\nentero y;")

    def test_block_comment_is_kept(self):
        text, _ = translate_source("/* if */ while (1) {}")
        self.assertEqual(text, "/* if */ mientras (1) {}")

    def test_char_literal_is_kept(self):
        text, counts = translate_source("char letra = 'i';")
        self.assertEqual(text, "carácter letra = 'i';")
        self.assertEqual(counts, {"char": 1})

    def test_counts_repeated_keywords(self):
        _, counts = translate_source("int a; int b; return 0;")
        self.assertEqual(counts, {"int": 2, "return": 1})


class TestReport(unittest.TestCase):
    def test_without_keywords(self):
        self.assertEqual(format_report({}), "No se encontraron palabras reservadas de C.")

    def test_with_keywords(self):
        report = format_report({"int": 2})
        self.assertIn("Se encontraron 2 palabras reservadas:", report)
        self.assertIn("int -> entero  (2)", report)


if __name__ == "__main__":
    unittest.main()
