#ifndef VISITORS_HPP
#define VISITORS_HPP

#include "ast.hpp"

class ExpressionEvaluator : public AST::AstVisitor
{
    int result_{};

public:
    void visit(AST::AddNode& node)
    {
        ExpressionEvaluator lv, rv;
        node.left().accept(lv);
        node.right().accept(rv);
        result_ = lv.result() + rv.result();
    }

    void visit(AST::MultiplyNode& node)
    {
        ExpressionEvaluator lv, rv;
        node.left().accept(lv);
        node.right().accept(rv);
        result_ = lv.result() * rv.result();
    }

    void visit(AST::IntNode& node)
    {
        result_ = node.value();
    }

    int result() const
    {
        return result_;
    }
};

// TODO: Implement visitor that prints expression to string
class PrintingVisitor : public AST::AstVisitor
{
    std::string result_;

public:
    void visit(AST::AddNode& node) override
    {
        PrintingVisitor lv, rv;
        node.left().accept(lv);
        node.right().accept(rv);
        result_ = "(" + lv.result() + " + " + rv.result() + ")";
    }

    void visit(AST::MultiplyNode& node) override
    {
        PrintingVisitor lv, rv;
        node.left().accept(lv);
        node.right().accept(rv);
        result_ = "(" + lv.result() + " * " + rv.result() + ")";
    }

    void visit(AST::IntNode& node) override
    {
        result_ = std::to_string(node.value());
    }

    std::string result() const
    {
        return result_;
    }
};

#endif // VISITORS_HPP