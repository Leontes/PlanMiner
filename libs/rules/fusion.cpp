#include "rules/fusion.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <map>
#include <optional>
#include <sstream>
#include <utility>

namespace rules {

namespace {

// El código sigue al original (Old/src/Rules/FuseRules.cpp) bloque a bloque, con sus nombres
// entre paréntesis: el resultado depende de cómo se cortan y se vuelven a juntar los nombres.

// (auxVec) Una comparación agrupada: la operación, el otro lado o el número, y el valor.
using Entry = std::vector<std::string>;
// (mapComps) Las comparaciones por su lado izquierdo y su lado derecho ("NUMERO" si es un número).
using Groups = std::map<std::pair<std::string, std::string>, std::vector<Entry>>;

struct Weighted {
  int weight = 0;
  Literal literal;
};

// Los límites de los intervalos del original: min() es el positivo más pequeño, no -infinito.
constexpr double lowest = std::numeric_limits<double>::min();
constexpr double highest = std::numeric_limits<double>::max();

std::vector<std::string> split(const std::string& text) {
  std::istringstream in(text);
  std::vector<std::string> tokens;
  for (std::string token; in >> token;) {
    tokens.push_back(token);
  }
  return tokens;
}

// Un token que puede faltar: el original lee fuera de rango.
const std::string& token(const std::vector<std::string>& tokens, std::size_t i) {
  static const std::string none;
  return i < tokens.size() ? tokens[i] : none;
}

bool is_comparison(const std::string& op) { return op == "opEQ" || op == "opLT" || op == "opGT"; }

bool is_truth(const std::string& value) { return value == "True" || value == "False"; }

// (isnumber)
bool is_number(const std::string& text) {
  if (text.size() == 1 && text[0] == '-') {
    return false;
  }
  return text.find_first_not_of("-.0123456789") == std::string::npos;
}

// Los tipos de la cabecera en los nombres: "?param_0 ~ tipo" pasa a "?param_0 ~ ?tipo". Cada pasada
// vuelve a juntar los tokens, con un espacio detrás de cada uno.
void substitute_header(std::vector<LegacyRule>& rules, const std::vector<std::string>& header) {
  for (std::size_t i = 0; i < header.size(); ++i) {
    const std::string parameter = "?param_" + std::to_string(i);
    for (LegacyRule& rule : rules) {
      for (Literal& literal : rule.conditions) {
        const std::vector<std::string> tokens = split(literal.attribute);
        std::string text;
        for (std::size_t k = 0; k < tokens.size(); ++k) {
          if (tokens[k] == parameter) {
            text += tokens[k] + " ";
            ++k;
            text += token(tokens, k) + " ";
            ++k;
            text += "?" + header[i] + " ";
          } else {
            text += tokens[k] + " ";
          }
        }
        literal.attribute = text;
      }
    }
  }
}

void remove_repeated(std::vector<LegacyRule>& rules) {
  for (LegacyRule& rule : rules) {
    std::vector<Literal> unique;
    for (const Literal& literal : rule.conditions) {
      if (std::find(unique.begin(), unique.end(), literal) == unique.end()) {
        unique.push_back(literal);
      }
    }
    rule.conditions = std::move(unique);
  }
}

// (preVec, postVec) Las condiciones de las reglas de un estado, sumando los ejemplos que cubren.
void accumulate(std::vector<Weighted>& all, const Literal& literal, int covered) {
  for (Weighted& weighted : all) {
    if (weighted.literal == literal) {
      weighted.weight += covered;
      return;
    }
  }
  all.push_back({covered, literal});
}

// Un atributo con dos valores: gana el primero si cubre más del 5 % de los ejemplos de los dos; si
// no, el otro (en el estado de después, el primero también). Con majority, el que cubre más, salvo
// si la diferencia no pasa de band por la media de los dos (con 0, un empate): entonces, ninguno.
std::vector<Weighted> select(const std::vector<Weighted>& all, bool post, bool majority,
                             double band) {
  std::vector<Weighted> selected;
  std::vector<bool> seen(all.size(), false);
  for (std::size_t i = 0; i < all.size(); ++i) {
    if (seen[i]) {
      continue;
    }
    seen[i] = true;
    std::optional<std::size_t> conflict;
    for (std::size_t j = 0; j < all.size(); ++j) {
      if (!seen[j] && all[i].literal.attribute == all[j].literal.attribute) {
        conflict = j;
        seen[j] = true;
      }
    }
    if (!conflict) {
      selected.push_back(all[i]);
      continue;
    }
    const double w1 = all[i].weight;
    const double w2 = all[*conflict].weight;
    if (majority) {
      if (std::abs(w1 - w2) > band * (w1 + w2) / 2.0) {
        selected.push_back(w1 > w2 ? all[i] : all[*conflict]);
      }
    } else if (w1 / (w1 + w2) > 0.05) {
      selected.push_back(all[i]);
    } else if (w2 / (w1 + w2) > 0.05) {
      selected.push_back(post ? all[i] : all[*conflict]);
    }
  }
  return selected;
}

// Lo que queda de las banderas =, > y < de una comparación.
void combine(bool eq, bool gt, bool lt, const std::string& other, std::vector<Entry>& out) {
  if (eq && gt && !lt) {
    out.push_back({"opGET", other, "true"});
  } else if (eq && !gt && lt) {
    out.push_back({"opLET", other, "true"});
  } else if (eq && !gt && !lt) {
    out.push_back({"opEQ", other, "true"});
  } else if (!eq && gt && lt) {
    out.push_back({"opEQ", other, "false"});
  } else if (!eq && gt && !lt) {
    out.push_back({"opGT", other, "true"});
  } else if (!eq && !gt && lt) {
    out.push_back({"opLT", other, "true"});
  }
}

// (mapCompsFiltrado) Una comparación por grupo; con números, una por número (std::stoi: el
// original busca con std::stod e inserta con std::stoi, así que con decimales reinicia las
// banderas en cada condición). Fuera de los números, las banderas se reinician en cada condición
// del grupo: solo cuenta la última.
Groups filter(const Groups& groups) {
  Groups filtered;
  for (const auto& [key, entries] : groups) {
    std::vector<Entry>& out = filtered[key];
    if (key.second == "NUMERO") {
      std::map<double, std::vector<bool>> flags;
      for (const Entry& entry : entries) {
        const double found = std::stod(entry[1]);
        const auto inserted = static_cast<double>(std::stoi(entry[1]));
        if (flags.find(found) == flags.end()) {
          flags[inserted] = {false, false, false};
        }
        if (entry[0] == "opEQ" && entry[2] == "True") {
          flags[inserted][0] = true;
        }
        if (entry[0] == "opGT" && entry[2] == "True") {
          flags[inserted][1] = true;
        }
        if (entry[0] == "opLT" && entry[2] == "True") {
          flags[inserted][2] = true;
        }
      }
      for (const auto& [number, bits] : flags) {
        combine(bits[0], bits[1], bits[2], std::to_string(number), out);
      }
    } else {
      bool eq = false;
      bool lt = false;
      bool gt = false;
      for (const Entry& entry : entries) {
        eq = entry[0] == "opEQ" && entry[2] == "True";
        lt = entry[0] == "opLT" && entry[2] == "True";
        gt = entry[0] == "opGT" && entry[2] == "True";
      }
      combine(eq, gt, lt, key.second, out);
    }
  }
  for (auto it = filtered.begin(); it != filtered.end();) {
    it = it->second.empty() ? filtered.erase(it) : std::next(it);
  }
  return filtered;
}

// Un intervalo del original: [inf, sup], abierto por cada lado si su bandera es cierta.
struct Interval {
  double inf = lowest;
  double sup = highest;
  bool inf_open = true;
  bool sup_open = true;

  void empty() {
    sup = lowest;
    inf = highest;
    inf_open = false;
    sup_open = false;
  }
  // Las situaciones que distingue el original, con el nombre de sus comentarios.
  bool point() const {
    return inf != lowest && sup != highest && inf == sup && !inf_open && !sup_open;
  }
  bool closed_up() const { return inf != lowest && sup == highest && !inf_open && sup_open; }
  bool open_up() const { return inf != lowest && sup == highest && inf_open && sup_open; }
  bool closed_down() const { return inf == lowest && sup != highest && inf_open && !sup_open; }
  bool open_both() const { return inf != lowest && sup != highest && inf_open && sup_open; }
  bool all() const { return inf == lowest && sup == highest && inf_open && sup_open; }

  // Cada comparación pasa por todas las situaciones en orden, y una puede llevar a la siguiente.
  void apply(const std::string& op, double y) {
    if (op == "opGET") {
      if (point()) {
        if (y <= inf) {
          inf = y, sup = highest, inf_open = false, sup_open = true;
        } else {
          empty();
        }
      }
      if (closed_up() && y < inf) {
        inf = y, inf_open = false;
      }
      if (open_up() && y < inf) {
        inf = y, inf_open = false;
      }
      if (closed_down()) {
        if (y < sup) {
          inf = y, inf_open = false;
        } else {
          empty();
        }
      }
      if (open_both()) {
        if (y < inf) {
          inf = y, inf_open = false;
        } else {
          empty();
        }
      }
      if (all()) {
        inf = y, inf_open = false;
      }
    } else if (op == "opGT") {
      if (point()) {
        if (y <= inf) {
          inf = y, sup = highest, inf_open = true, sup_open = true;
        } else {
          empty();
        }
      }
      if (closed_up() && y < inf) {
        inf = y, inf_open = true;
      }
      if (open_up() && y < inf) {
        inf = y, inf_open = true;
      }
      if (closed_down()) {
        if (y < sup) {
          inf = y, inf_open = true;
        } else {
          empty();
        }
      }
      if (open_both()) {
        if (y < inf) {
          inf = y, inf_open = true, sup_open = true;
        } else {
          empty();
        }
      }
      if (all()) {
        inf = y, inf_open = true;
      }
    } else if (op == "opLET") {
      if (point()) {
        if (y >= inf) {
          sup = y, sup_open = false, inf = lowest, inf_open = true;
        } else {
          empty();
        }
      }
      if (closed_up()) {
        if (y > inf) {
          sup = y, sup_open = false;
        } else {
          empty();
        }
      }
      if (open_up()) {
        if (y > inf) {
          sup = y, sup_open = false;
        } else {
          empty();
        }
      }
      if (closed_down() && y > sup) {
        sup = y, sup_open = false;
      }
      if (open_both() && y > sup) {
        sup = y, sup_open = false;
      }
      if (all()) {
        sup = y, sup_open = false;
      }
    } else if (op == "opLT") {
      if (point()) {
        if (y > inf) {
          sup = y, inf = lowest, inf_open = true, sup_open = true;
        } else {
          empty();
        }
      }
      if (closed_up()) {
        if (y > inf) {
          sup = y, sup_open = true;
        } else {
          empty();
        }
      }
      if (open_up()) {
        if (y > inf) {
          sup = y, sup_open = true;
        } else {
          empty();
        }
      }
      if (closed_down() && y > sup) {
        sup = y, sup_open = true;
      }
      if (open_both() && y > sup) {
        sup = y, sup_open = true;
      }
      if (all()) {
        sup = y, sup_open = true;
      }
    } else {
      // Igualdad, también la que es False.
      if (point() && y > inf) {
        empty();
      }
      if (closed_up() && y < inf) {
        empty();
      }
      if (open_up() && y < inf) {
        empty();
      }
      if (closed_down() && y > sup) {
        empty();
      }
      if (open_both() && y > sup) {
        empty();
      }
      if (all()) {
        inf = y, inf_open = false, sup = y, sup_open = false;
      }
    }
  }

  // Las condiciones que escribe el original para el intervalo.
  void write(const std::string& left, std::vector<Literal>& rule) const {
    const auto add = [&](const char* op, double bound) {
      rule.push_back({std::string{op} + " - " + left + " " + std::to_string(bound), "True"});
    };
    const bool bounded = inf != lowest && sup != highest;
    if (inf == sup && !inf_open && !sup_open) {
      add("opEQ", sup);
    }
    if (inf != sup && !inf_open && !sup_open && bounded) {
      add("opLET", sup);
      add("opGET", inf);
    }
    if (inf != sup && inf_open && sup_open && bounded) {
      add("opLT", sup);
      add("opGT", inf);
    }
    if (inf != sup && !inf_open && sup_open && bounded) {
      add("opLT", sup);
      add("opGET", inf);
    }
    if (inf != sup && inf_open && !sup_open && bounded) {
      add("opLET", sup);
      add("opGT", inf);
    }
    if (inf != sup && !inf_open && sup_open && inf != lowest && sup == highest) {
      add("opGET", inf);
    }
    if (inf != sup && inf_open && sup_open && inf != lowest && sup == highest) {
      add("opGT", inf);
    }
    if (inf != sup && inf_open && !sup_open && inf == lowest && sup != highest) {
      add("opLET", sup);
    }
    if (inf != sup && inf_open && sup_open && inf == lowest && sup != highest) {
      add("opLT", sup);
    }
  }
};

// (borrarRedundancias) Con el mismo lado izquierdo, una relación cuyo lado derecho está contenido
// (como cadena) en el de otra se quita, y todas las comparaciones que lo contienen.
std::vector<Literal> remove_redundant(std::vector<Literal> rule) {
  std::map<std::string, std::vector<std::string>> equalities;
  for (const Literal& literal : rule) {
    const std::vector<std::string> tokens = split(literal.attribute);
    if (!is_comparison(token(tokens, 0))) {
      continue;
    }
    std::vector<std::string> left;
    std::vector<std::string> right;
    std::size_t i = 2;
    if (token(tokens, 2) == "(") {
      while (i < tokens.size() && tokens[i] != ")") {
        left.push_back(tokens[i]);
        ++i;
      }
      left.push_back(token(tokens, i));
      ++i;
    } else {
      left.push_back(token(tokens, i));
      ++i;
    }
    if (token(tokens, i) == "(") {
      while (i < tokens.size()) {
        right.push_back(tokens[i]);
        ++i;
      }
    } else {
      right.push_back(token(tokens, i));
    }
    std::string left_text;
    for (const std::string& t : left) {
      left_text += t + " ";
    }
    std::string right_text;
    for (const std::string& t : right) {
      right_text += t + " ";
    }
    equalities[left_text].push_back(right_text);
  }

  for (const auto& [left, rights] : equalities) {
    std::vector<std::string> remove;
    for (std::size_t a = 0; a < rights.size(); ++a) {
      for (std::size_t b = 0; b < rights.size(); ++b) {
        if (a != b && rights[a].find(rights[b]) != std::string::npos) {
          std::string text = left + rights[b];
          text.pop_back();
          remove.push_back(text);
        }
      }
    }
    for (Literal& literal : rule) {
      if (literal.attribute.find("opGT") != std::string::npos ||
          literal.attribute.find("opLT") != std::string::npos ||
          literal.attribute.find("opEQ") != std::string::npos) {
        for (const std::string& text : remove) {
          if (literal.attribute.find(text) != std::string::npos) {
            literal.value = "ELIMINAR";
          }
        }
      }
    }
  }

  std::vector<Literal> kept;
  for (const Literal& literal : rule) {
    if (literal.value != "ELIMINAR") {
      kept.push_back(literal);
    }
  }
  return kept;
}

// Las ordenaciones que admite una comparación cierta o falsa: 1, <; 2, =; 4, >.
unsigned orderings(const std::string& op, bool truth) {
  const unsigned bits = op == "opLT" ? 1U : op == "opEQ" ? 2U : 4U;
  return truth ? bits : 7U & ~bits;
}

const char* comparison_of(unsigned orderings) {
  switch (orderings) {
    case 1:
      return "opLT";
    case 2:
      return "opEQ";
    case 4:
      return "opGT";
    case 3:
      return "opLET";
    case 6:
      return "opGET";
    default:
      return nullptr;  // ninguna restricción, solo «distinto» o una contradicción
  }
}

std::vector<Literal> precondition(const std::vector<Weighted>& pre, const FusionOptions& options) {
  std::vector<Literal> rule;
  Groups groups;
  // (lado izquierdo, lado derecho o número) -> ordenaciones admitidas
  std::map<std::pair<std::string, std::string>, unsigned> combined;
  for (const Weighted& selected : select(pre, false, options.majority, options.conflict_band)) {
    std::vector<std::string> tokens = split(selected.literal.attribute);
    const std::string& value = selected.literal.value;
    // Un valor discretizado pasa a ser una igualdad con su número.
    if (!is_truth(value) && tokens[0] != "DELTA") {
      std::vector<std::string> extended{"opEQ", "-"};
      extended.insert(extended.end(), tokens.begin(), tokens.end());
      extended.push_back(value);
      tokens = std::move(extended);
    }
    if (is_comparison(tokens[0])) {
      std::string left;
      std::size_t j = 2;
      while (j < tokens.size() && tokens[j] != ")") {
        left += tokens[j] + " ";
        ++j;
      }
      if (j < tokens.size()) {
        left += tokens[j];
        ++j;
      }
      std::string other;
      if (is_number(tokens.back())) {
        other = "NUMERO";
      } else {
        while (j < tokens.size()) {
          other += tokens[j] + " ";
          ++j;
        }
      }
      if (options.combine_comparisons) {
        // Los valores discretizados (que no son True ni False) se pierden, como en el original.
        if (is_truth(value)) {
          const auto [entry, inserted] =
              combined.try_emplace({left, other == "NUMERO" ? tokens.back() : other}, 7U);
          entry->second &= orderings(tokens[0], value == "True");
        }
        continue;
      }
      groups[{left, other}].push_back(
          {tokens[0], other == "NUMERO" ? tokens.back() : other, value});
    } else if (is_truth(value)) {
      rule.push_back(selected.literal);
    }
  }

  for (const auto& [key, entries] : filter(groups)) {
    if (key.second == "NUMERO") {
      Interval interval;
      for (const Entry& entry : entries) {
        interval.apply(entry[0], std::stod(entry[1]));
      }
      interval.write(key.first, rule);
    } else {
      rule.push_back({entries[0][0] + " - " + key.first + " " + entries[0][1], "True"});
    }
  }
  for (const auto& [key, allowed] : combined) {
    if (const char* op = comparison_of(allowed)) {
      rule.push_back({std::string{op} + " - " + key.first + " " + key.second, "True"});
    }
  }
  return remove_redundant(std::move(rule));
}

std::vector<Literal> effect(const std::vector<Weighted>& post,
                            const std::vector<Literal>& precondition,
                            const FusionOptions& options) {
  std::vector<Literal> rule;
  Groups groups;
  for (const Weighted& selected : select(post, true, options.majority, options.conflict_band)) {
    const std::vector<std::string> tokens = split(selected.literal.attribute);
    const std::string& value = selected.literal.value;
    if (is_truth(value)) {
      if (!is_comparison(tokens[0])) {
        rule.push_back(selected.literal);
      }
    } else if (tokens[0] == "DELTA") {
      if (!is_number(tokens.back())) {
        rule.push_back(selected.literal);
      } else if (std::stod(tokens.back()) != 0.0) {
        std::string changed;
        for (std::size_t k = 2; k < tokens.size(); ++k) {
          changed += tokens[k] + " ";
        }
        groups[{changed, "NUMERO"}].push_back({"DELTA", "-", value});
      }
    } else {
      std::string term;
      for (std::size_t k = 0; k + 1 < tokens.size(); ++k) {
        term += tokens[k] + " ";
      }
      term += tokens.back();
      groups[{term, "NUMERO"}].push_back({"opEQ", "-", value});
    }
  }

  // Un cambio, o una asignación si solo hay valores (val1 y val2 siguen de un grupo al siguiente).
  std::size_t change = 0;
  std::size_t assignment = 0;
  for (const auto& [key, entries] : groups) {
    bool assign = true;
    for (std::size_t i = 0; i < entries.size(); ++i) {
      if (entries[i][0] == "opEQ") {
        assignment = i;
      } else {
        change = i;
        assign = false;
      }
    }
    if (!assign) {
      rule.push_back({"DELTA - " + key.first, entries[change][2]});
    } else {
      rule.push_back({"opEQ " + key.first + " " + entries[assignment][2], entries[assignment][2]});
    }
  }

  // Lo que repite un cambio con los mismos tokens se quita.
  for (std::size_t i = 0; i < rule.size(); ++i) {
    const std::vector<std::string> tokens = split(rule[i].attribute);
    if (tokens[0] != "DELTA") {
      continue;
    }
    for (Literal& other : rule) {
      const std::vector<std::string> others = split(other.attribute);
      bool distinct = false;
      for (std::size_t k = 1; k < others.size(); ++k) {
        distinct = distinct || k + 1 >= tokens.size() || tokens[k + 1] != others[k];
      }
      if (!distinct) {
        other.value = "ELIMINAR";
      }
    }
  }

  std::vector<Literal> result;
  for (const Literal& literal : rule) {
    if (literal.value != "ELIMINAR" &&
        std::find(precondition.begin(), precondition.end(), literal) == precondition.end()) {
      result.push_back(literal);
    }
  }
  return result;
}

// Si una condición se cumple en al menos min_support de los ejemplos de su estado en que se conoce.
// Las que no se pueden comprobar (valores discretizados, cambios, atributos sin valores) se quedan.
bool supported(const Literal& literal, bool post, const Evidence& evidence, double min_support) {
  const auto column = evidence.columns.find(literal.attribute);
  if (!is_truth(literal.value) || column == evidence.columns.end()) {
    return true;
  }
  const double expected = literal.value == "True" ? 1.0 : 0.0;
  const std::size_t n = evidence.transitions;
  std::size_t known = 0;
  std::size_t holds = 0;
  for (std::size_t row = post ? n : 0; row < (post ? 2 * n : n); ++row) {
    const double value = column->second[row];
    if (!std::isnan(value)) {
      ++known;
      holds += value == expected ? 1U : 0U;
    }
  }
  return static_cast<double>(holds) >= min_support * static_cast<double>(known);
}

}  // namespace

FusionOptions FusionOptions::original() {
  FusionOptions options;
  options.validate = false;
  options.majority = false;
  options.combine_comparisons = false;
  return options;
}

Fused fuse_original(std::vector<LegacyRule> rules, const std::vector<std::string>& header) {
  return fuse(std::move(rules), header, {}, FusionOptions::original());
}

Fused fuse(std::vector<LegacyRule> rules, const std::vector<std::string>& header,
           const Evidence& evidence, const FusionOptions& options) {
  if (options.validate) {
    for (LegacyRule& rule : rules) {
      std::erase_if(rule.conditions, [&](const Literal& literal) {
        return !supported(literal, rule.covered >= 0, evidence, options.min_support);
      });
    }
  }
  substitute_header(rules, header);
  remove_repeated(rules);

  std::vector<Weighted> pre;
  std::vector<Weighted> post;
  for (const LegacyRule& rule : rules) {
    for (const Literal& literal : rule.conditions) {
      if (rule.covered < 0) {
        accumulate(pre, literal, -rule.covered);
      } else {
        accumulate(post, literal, rule.covered);
      }
    }
  }

  // Las condiciones irrelevantes: las de poco soporte.
  if (options.min_rule_support > 0.0 && evidence.transitions > 0) {
    const double least = options.min_rule_support * static_cast<double>(evidence.transitions);
    const auto irrelevant = [least](const Weighted& weighted) { return weighted.weight < least; };
    std::erase_if(pre, irrelevant);
    std::erase_if(post, irrelevant);
  }

  Fused fused;
  fused.precondition = precondition(pre, options);
  fused.effect = effect(post, fused.precondition, options);
  return fused;
}

}  // namespace rules
