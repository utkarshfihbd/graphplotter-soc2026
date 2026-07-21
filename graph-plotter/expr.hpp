#ifndef _EXPR_HPP_
#define _EXPR_HPP_

#include <memory>
#include <string>
#include <cmath>
#include <stdexcept>

struct Expr {
  virtual double eval(double x, double z, double t) const = 0;
  virtual ~Expr() = default;
};

using ExprPtr = std::shared_ptr<Expr>;

struct Constant : Expr {
  double value;
  explicit Constant(double v) : value(v) {}
  double eval(double, double, double) const override {
    return value;
  }
};

enum class VarName { X, Z, T };

struct Variable : Expr {
  VarName name;
  explicit Variable(VarName n) : name(n) {}
  double eval(double x, double z, double t) const override {
    switch (name) {
      case VarName::X: return x;
      case VarName::Z: return z;
      case VarName::T: return t;
    }
    return 0.0;
  }
};

enum class BinOpKind { Add, Sub, Mul, Div };

struct BinaryOp : Expr {
  BinOpKind kind;
  ExprPtr lhs, rhs;
  BinaryOp(BinOpKind k, ExprPtr l, ExprPtr r) : kind(k), lhs(l), rhs(r) {}

  double eval(double x, double z, double t) const override {
    double a = lhs->eval(x, z, t);
    double b = rhs->eval(x, z, t);
    switch (kind) {
      case BinOpKind::Add: return a + b;
      case BinOpKind::Sub: return a - b;
      case BinOpKind::Mul: return a * b;
      case BinOpKind::Div: return a / b; 
    }
    return 0.0; 
  }
};

struct UnaryNeg : Expr {
  ExprPtr operand;
  explicit UnaryNeg(ExprPtr e) : operand(e) {}
  double eval(double x, double z, double t) const override {
    return -operand->eval(x, z, t);
  }
};

enum class FuncKind { Sin, Cos, Tan, Exp, Log, Sqrt, Abs };

struct FuncCall : Expr {
  FuncKind func;
  ExprPtr arg;
  FuncCall(FuncKind f, ExprPtr a) : func(f), arg(a) {}

  double eval(double x, double z, double t) const override {
    double v = arg->eval(x, z, t);
    switch (func) {
      case FuncKind::Sin:  return std::sin(v);
      case FuncKind::Cos:  return std::cos(v);
      case FuncKind::Tan:  return std::tan(v);
      case FuncKind::Exp:  return std::exp(v);
      case FuncKind::Log:  return std::log(v);
      case FuncKind::Sqrt: return std::sqrt(v);
      case FuncKind::Abs:  return std::fabs(v);
    }
    return 0.0; 
  }
};

class ExprNode {
public:
  ExprPtr ptr;

  ExprNode(ExprPtr p) : ptr(p) {}
  ExprNode(double v) : ptr(std::make_shared<Constant>(v)) {}

  double eval(double x, double z, double t) const {
    return ptr->eval(x, z, t);
  }
};

inline ExprNode x() { return ExprNode(std::make_shared<Variable>(VarName::X)); }
inline ExprNode z() { return ExprNode(std::make_shared<Variable>(VarName::Z)); }
inline ExprNode t() { return ExprNode(std::make_shared<Variable>(VarName::T)); }
inline ExprNode operator+(ExprNode a, ExprNode b) {
  return ExprNode(std::make_shared<BinaryOp>(BinOpKind::Add, a.ptr, b.ptr));
}
inline ExprNode operator-(ExprNode a, ExprNode b) {
  return ExprNode(std::make_shared<BinaryOp>(BinOpKind::Sub, a.ptr, b.ptr));
}
inline ExprNode operator*(ExprNode a, ExprNode b) {
  return ExprNode(std::make_shared<BinaryOp>(BinOpKind::Mul, a.ptr, b.ptr));
}
inline ExprNode operator/(ExprNode a, ExprNode b) {
  return ExprNode(std::make_shared<BinaryOp>(BinOpKind::Div, a.ptr, b.ptr));
}
inline ExprNode operator-(ExprNode a) {
  return ExprNode(std::make_shared<UnaryNeg>(a.ptr));
}

inline ExprNode sin(ExprNode a)  { return ExprNode(std::make_shared<FuncCall>(FuncKind::Sin,  a.ptr)); }
inline ExprNode cos(ExprNode a)  { return ExprNode(std::make_shared<FuncCall>(FuncKind::Cos,  a.ptr)); }
inline ExprNode tan(ExprNode a)  { return ExprNode(std::make_shared<FuncCall>(FuncKind::Tan,  a.ptr)); }
inline ExprNode exp(ExprNode a)  { return ExprNode(std::make_shared<FuncCall>(FuncKind::Exp,  a.ptr)); }
inline ExprNode log(ExprNode a)  { return ExprNode(std::make_shared<FuncCall>(FuncKind::Log,  a.ptr)); }
inline ExprNode sqrt(ExprNode a) { return ExprNode(std::make_shared<FuncCall>(FuncKind::Sqrt, a.ptr)); }
inline ExprNode abs(ExprNode a)  { return ExprNode(std::make_shared<FuncCall>(FuncKind::Abs,  a.ptr)); }

#endif