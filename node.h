#ifndef ___NODE_H
#define ___NODE_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include<llvm/Support/Casting.h>

class node_Function;
class node_Function_Declaration;
class node_Block;
class node_Variable_Declaration;

#define SAFE_DELETE(x) {delete x; x=NULL;}
#define SAFE_DELETEA(x) {delete[] x; x=NULL;}

// Nodeの種類一覧
enum NodeID{
  BaseID,
  VariableID,
  VariableDeclID,
  StatementID,
  ExpressionID,
  IntegerID,
  DoubleID,
  CharID,
  FuncCallID,
  IfStmtID,
  BinaryOpID,
  ReturnID
};

// 基底クラス
class Node{
  // Nodeの種類
  NodeID ID = BaseID;

  public:
  Node(){}
  Node(NodeID id):ID(id){}
  virtual ~Node(){}
  NodeID get_NodeID() const {return ID;}
};

class node_Statement : public Node{
  public:
    node_Statement(NodeID id) : Node(id) {}
    ~node_Statement(){}
};

class node_Expression : public node_Statement{
  public:
    node_Expression(NodeID id) : node_Statement(id) {}
    ~node_Expression(){}
};

class node_Program{
  // 関数宣言の集合
  std::vector<node_Function_Declaration*> *Function_Decl_List;
  // 関数定義の集合
  std::vector<node_Function*> *Function_List;

  public:
    node_Program(std::vector<node_Function*> *func_list)
    : Function_List(func_list){}
    node_Program(std::vector<node_Function_Declaration*> *func_decl_list, std::vector<node_Function*> *func_list)
    : Function_Decl_List(func_decl_list), Function_List(func_list){}
    ~node_Program(){}

    // Programノードが空かどうか
    bool empty();

    // i番目のプロトタイプ宣言を取り出す
    node_Function_Declaration* get_Prototype(int i){
      if(i < Function_Decl_List->size())
        return Function_Decl_List->at(i);
      else
        return NULL;
    }

    // i番目の関数を取り出す
    node_Function* get_Function(int i){
      if(i < Function_List->size())
        return Function_List->at(i);
      else
        return NULL;
    }

};

// 関数宣言　プロトタイプ宣言
class node_Function_Declaration{
  // 関数の名前
  std::string *Name;
  // 引数の名前
  std::vector<node_Variable_Declaration*> *Params;

  public:
    node_Function_Declaration(std::string *name, std::vector<node_Variable_Declaration*> *params)
    : Name(name), Params(params){}
    node_Function_Declaration(std::string *name)
    : Name(name){}
    ~node_Function_Declaration(){}

    // 関数名を取得する
    std::string get_Name(){
      std::string c_name = *Name;
      return c_name;
    }
    // i番目の引数の名前を取り出す
    node_Variable_Declaration* get_Param(int i){
      if(i < Params->size())
        return Params->at(i);
      else
        return NULL;
    }
    // 引数の数を取り出す
    int get_Params_Size(){return Params->size();}
};

// 変数宣言
class node_Variable_Declaration : public node_Statement{
  // 宣言の種類（関数内 or 引数部分)
  public:
    typedef enum{
      param,
      local
    }DeclType;

  private:
    // 変数の名前
    std::string *Name;
    // 宣言の種類
    DeclType Type;

  public:
    node_Variable_Declaration(std::string *name)
    : node_Statement(VariableDeclID), Name(name){}
    ~node_Variable_Declaration(){}

    static inline bool classof(node_Variable_Declaration const*){return true;}
    //渡されたStatementノードがVariableDeclIDか判定する
    static inline bool classof(node_Statement const *base){
      return base->get_NodeID()==VariableDeclID;
    }

    std::string get_Name(){
      std::string c_name = *Name;
      return c_name;
    }
    DeclType get_Type(){return Type;}

    bool set_Type(DeclType type){Type = type; return true;}
};


// 関数やIF文などの｛｝で囲まれた部分 : Statementの集合
class node_Block {
  // Statementの集合
  std::vector<node_Statement*> *Statement_List;
  // 変数定義の集合
  // 変数定義のノードはここに入れる
  // std::vector<node_Variable_Declaration*> *Variable_Decl_List;

  public:
    node_Block(std::vector<node_Statement*> *stmt_list)
    : Statement_List(stmt_list) {}
    ~node_Block(){}

    // Statement_ListにNodeを追加する
    bool add_Statement(node_Statement *stmt){Statement_List->push_back(stmt);}
    // Variable_Decl_Listに変数定義のNodeを追加する
    //bool add_Variable_Decl(node_Variable_Declaration *vdecl){}

    // Statement_Listからi番目の要素を取り出す
    node_Statement* get_Statement(int i){
      if(i < Statement_List->size())
        return Statement_List->at(i);
      else
        return NULL;
    }
    // Variable_Decl_Listからi番目の要素を取り出す
    //Node* get_Variable_Decl(int i){
    //  if(i < Variable_Decl_List->size())
    //    return Variable_Decl_List->at(i);
    //  else
    //    return NULL;
    //}
};

// 関数
class node_Function{
  // プロトタイプ宣言
  node_Function_Declaration *Prototype;
  // 関数のBlock
  node_Block *FuncBlock;

  public:
    node_Function(node_Function_Declaration *prototype, node_Block *funcblock)
    : Prototype(prototype), FuncBlock(funcblock){}
    ~node_Function(){}

    // この関数の名前を取り出す
    std::string get_FuncName(){return Prototype->get_Name();}
    // この関数のプロトタイプ宣言を取り出す
    node_Function_Declaration* get_Prototype(){return Prototype;}
    // この関数のブロック部分を取り出す
    node_Block* get_FuncBlock(){return FuncBlock;}
};

// if else文
//class node_IF_Statement : public Statement{

//};

// return文
class node_Return : public node_Statement{
  //戻り値となるexpr
  node_Expression *Expr;

  public:
    node_Return(node_Expression *expr) : node_Statement(ReturnID), Expr(expr) {}
    ~node_Return(){}

    static inline bool classof(node_Return const*){return true;}
    //渡されたStatementノードがReturnIDか判定する
    static inline bool classof(node_Statement const* base){
      return base->get_NodeID()==ReturnID;
    }

    node_Expression* get_Expr(){return Expr;}
};

// 二項演算子
class node_Binary_Operator : public node_Expression{
  // 二項演算子
  char Op;
  // 右辺
  node_Expression *RHS;
  // 左辺
  node_Expression *LHS;

  public:
    node_Binary_Operator(char op, node_Expression *lhs, node_Expression *rhs)
    : node_Expression(BinaryOpID), Op(op), RHS(rhs), LHS(lhs) {}
    ~node_Binary_Operator(){}

    static inline bool classof(node_Binary_Operator const*){return true;}
    //渡されたStatementノードがBinaryOpIDか判定する
    static inline bool classof(node_Binary_Operator const* base){
      return base->get_NodeID()==BinaryOpID;
    }

    char get_Op(){return Op;}
    node_Expression* get_RHS(){return RHS;}
    node_Expression* get_LHS(){return LHS;}
};

// 変数参照
class node_Variable : public node_Expression{
  // 変数の名前
  std::string Name;

  public:
    node_Variable(const std::string &name) : node_Expression(VariableID), Name(name){}
    ~node_Variable(){}

    static inline bool classof(node_Variable const*){return true;}
    //渡されたExpressionノードがVariableIDか判定する
    static inline bool classof(node_Expression const* base){
      return base->get_NodeID()==VariableID;
    }

    std::string get_Name(){return Name;}
};

// Int型の数値
class node_Integer : public node_Expression{
  // 数値
  int Val;

  public:
    node_Integer(int val) : node_Expression(IntegerID), Val(val){}
    ~node_Integer();

    static inline bool classof(node_Integer const*){return true;}
    //渡されたExpressionノードがIntegerIDか判定する
    static inline bool classof(node_Expression const* base){
      return base->get_NodeID()==IntegerID;
    }

    int get_Val(){return Val;}
};

// Double型の数値
class node_Double : public node_Expression{
  // 数値
  double Val;

  public:
    node_Double(double val) : node_Expression(DoubleID), Val(val){}
    ~node_Double();

    static inline bool classof(node_Double const*){return true;}
    //渡されたExpressionノードがDoubleIDか判定する
    static inline bool classof(node_Expression const* base){
      return base->get_NodeID()==DoubleID;
    }

    int get_Val(){return Val;}
};

// Char型の数値
class node_Char : public node_Expression{
  // 数値
  char Val;

  public:
    node_Char(char val) : node_Expression(CharID), Val(val){}
    ~node_Char();

    static inline bool classof(node_Char const*){return true;}
    //渡されたExpressionノードがIntegerIDか判定する
    static inline bool classof(node_Expression const* base){
      return base->get_NodeID()==CharID;
    }

    int get_Val(){return Val;}
};

// 関数呼び出し
class node_Function_Call : public node_Expression {
  // 呼び出される関数の名前
  std::string *Callee;
  // 関数呼び出しの引数
  std::vector<node_Expression*> *Args;

  public:
    node_Function_Call(std::string *callee, std::vector<node_Expression*> *args)
     : node_Expression(FuncCallID), Callee(callee), Args(args){}
    ~node_Function_Call(){}

    static inline bool classof(node_Function_Call const*){return true;}
    //渡されたStatementノードがFuncCallIDか判定する
    static inline bool classof(node_Statement const* base){
      return base->get_NodeID()==FuncCallID;
    }

    std::string get_Callee(){
      std::string c_Callee = *Callee;
      return c_Callee;
    }
    std::vector<node_Expression*> get_Args(){
      std::vector<node_Expression*> c_Args = *Args;
      return c_Args;
    }
};

#endif
