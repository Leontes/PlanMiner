// Las expresiones aritméticas de la regresión simbólica.
#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace regression {

enum class Operation { constant, variable, add, subtract, multiply, divide };

/// Una expresión aritmética: una constante, una variable (por su índice) o una operación binaria.
/// Es inmutable y comparte sus subexpresiones, así que copiarla es barato.
class Expression {
 public:
  static Expression constant(double value);
  static Expression variable(std::size_t index);
  static Expression binary(Operation operation, Expression left, Expression right);

  Operation operation() const;
  bool is_leaf() const;
  /// El valor de una constante.
  double value() const;
  /// El índice de una variable.
  std::size_t index() const;
  /// Los operandos de una operación binaria.
  const Expression& left() const;
  const Expression& right() const;

  /// El número de nodos: constantes, variables y operaciones.
  std::size_t size() const;

  /// El valor en cada ejemplo. variables[v][e] es el valor de la variable v en el ejemplo e (NaN
  /// si es desconocido). El resultado es NaN donde falta una variable o se divide por 0.
  std::vector<double> evaluate(const std::vector<std::vector<double>>& variables,
                               std::size_t examples) const;

  /// La expresión en la notación prefija de PDDL, con los nombres de las variables:
  /// "(* (distance ?1 ?2) (slow-burn ?0))".
  std::string to_string(const std::vector<std::string>& names) const;

  /// Igualdad de estructura: misma operación, mismos operandos, mismas constantes.
  bool operator==(const Expression& other) const;

 private:
  struct Node;
  explicit Expression(std::shared_ptr<const Node> node);
  std::shared_ptr<const Node> node_;
};

}  // namespace regression
