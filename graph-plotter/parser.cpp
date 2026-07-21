#include "parser.hpp"
#include <vector>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <map>

enum class TokType { Num, Ident, Plus, Minus, Star, Slash, LParen, RParen, End };

struct Token {
  TokType type;
  double numValue = 0.0;
  std::string text;
};

static std::vector<Token> tokenize(const std::string& src) {
  std::vector<Token> tokens;
  size_t i = 0;
  size_t n = src.size();

  while (i < n) {
    char c = src[i];

    if (std::isspace((unsigned char)c)) { i++; continue; }

    if (std::isdigit((unsigned char)c) || c == '.') {
      size_t start = i;
      bool sawDot = false;
      while (i < n && (std::isdigit((unsigned char)src[i]) || (src[i] == '.' && !sawDot))) {
        if (src[i] == '.') sawDot = true;
        i++;
      }
      std::string numStr = src.substr(start, i - start);
      Token t;
      t.type = TokType::Num;
      t.numValue = std::stod(numStr);
      tokens.push_back(t);
      continue;
    }

    if (std::isalpha((unsigned char)c)) {
      size_t start = i;
      while (i < n && std::isalnum((unsigned char)src[i])) i++;
      Token t;
      t.type = TokType::Ident;
      t.text = src.substr(start, i - start);
      tokens.push_back(t);
      continue;
    }

    switch (c) {
      case '+': tokens.push_back({TokType::Plus});   i++; continue;
      case '-': tokens.push_back({TokType::Minus});  i++; continue;
      case '*': tokens.push_back({TokType::Star});   i++; continue;
      case '/': tokens.push_back({TokType::Slash});  i++; continue;
      case '(': tokens.push_back({TokType::LParen}); i++; continue;
      case ')': tokens.push_back({TokType::RParen}); i++; continue;
      default:
        throw std::runtime_error(std::string("Unexpected character '") + c + "' in equation");
    }
  }

  tokens.push_back({TokType::End});
  return tokens;
}

static const std::map<std::string, FuncKind>& funcTable() {
  static const std::map<std::string, FuncKind> table = {
    {"sin", FuncKind::Sin}, {"cos", FuncKind::Cos}, {"tan", FuncKind::Tan},
    {"exp", FuncKind::Exp}, {"log", FuncKind::Log}, {"sqrt", FuncKind::Sqrt},
    {"abs", FuncKind::Abs},
  };
  return table;
}

class Parser {
public:
  explicit Parser(std::vector<Token> toks) : tokens(std::move(toks)) {}

  ExprPtr parse() {
    ExprPtr result = parseExpr();
    expect(TokType::End, "end of expression");
    return result;
  }

private:
  std::vector<Token> tokens;
  size_t pos = 0;

  const Token& peek() const { return tokens[pos]; }
  const Token& advance() { return tokens[pos++]; }
  bool check(TokType t) const { return peek().type == t; }

  void expect(TokType t, const std::string& what) {
    if (!check(t))
      throw std::runtime_error("Expected " + what + " in equation");
    advance();
  }

  ExprPtr parseExpr() {
    ExprPtr node = parseTerm();
    while (check(TokType::Plus) || check(TokType::Minus)) {
      TokType op = advance().type;
      ExprPtr rhs = parseTerm();
      BinOpKind kind = (op == TokType::Plus) ? BinOpKind::Add : BinOpKind::Sub;
      node = std::make_shared<BinaryOp>(kind, node, rhs);
    }
    return node;
  }

  ExprPtr parseTerm() {
    ExprPtr node = parseFactor();
    while (check(TokType::Star) || check(TokType::Slash)) {
      TokType op = advance().type;
      ExprPtr rhs = parseFactor();
      BinOpKind kind = (op == TokType::Star) ? BinOpKind::Mul : BinOpKind::Div;
      node = std::make_shared<BinaryOp>(kind, node, rhs);
    }
    return node;
  }

  ExprPtr parseFactor() {
    if (check(TokType::Minus)) {
      advance();
      return std::make_shared<UnaryNeg>(parseFactor());
    }

    if (check(TokType::Num)) {
      double v = advance().numValue;
      return std::make_shared<Constant>(v);
    }

    if (check(TokType::Ident)) {
      std::string name = advance().text;

      if (check(TokType::LParen)) {
        advance(); // consume '('
        ExprPtr arg = parseExpr();
        expect(TokType::RParen, "')'");

        auto it = funcTable().find(name);
        if (it == funcTable().end())
          throw std::runtime_error("Unknown function '" + name + "'");
        return std::make_shared<FuncCall>(it->second, arg);
      }

      if (name == "x") return std::make_shared<Variable>(VarName::X);
      if (name == "z") return std::make_shared<Variable>(VarName::Z);
      if (name == "t") return std::make_shared<Variable>(VarName::T);
      throw std::runtime_error("Unknown identifier '" + name + "'");
    }

    if (check(TokType::LParen)) {
      advance();
      ExprPtr node = parseExpr();
      expect(TokType::RParen, "')'");
      return node;
    }

    throw std::runtime_error("Unexpected token in equation");
  }
};

ExprPtr parseExpression(const std::string& text) {
  std::vector<Token> tokens = tokenize(text);
  Parser parser(std::move(tokens));
  return parser.parse();
}

ExprPtr loadExpressionFromFile(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("Cannot open equation file: " + path);

  std::string line;
  while (std::getline(file, line)) {
    // Skip blank lines and comment lines starting with '#'.
    size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
    if (firstNonSpace == std::string::npos) continue;
    if (line[firstNonSpace] == '#') continue;

    return parseExpression(line);
  }

  throw std::runtime_error("No equation found in file: " + path);
}