#include <iostream>
#include <vector>

#Define BASEID

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
    node_Statement(){}
    ~node_Statement(){}
};

class node_Expression : public Statement{
  public:
    node_Expression(){}
    ~node_Expression(){}
};

class node_Program{
  // 関数宣言の集合
  std::vector<node_Function_Declaration*> Function_Decl_List;
  // 関数定義の集合
  std::vector<node_Function*> Function_List;

  public:
    node_Program(){}
    ~node_Program(){}

    // Programノードが空かどうか
    bool is_empty();

    // i番目のプロトタイプ宣言を取り出す
    node_Function_Declaration* get_Prototype(int i){
      if(i < Function_Decl_List.size())
        return Function_Dcl_List.at(i);
      else
        return NULL;
    }

    // i番目の関数を取り出す
    node_Function* get_Function(int i){
      if(i < Function_List.size())
        return Function_List>at(i);
      else
        return NULL;
    }

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
    std::string get_FuncName(){return Prototype->getName();}
    // この関数のプロトタイプ宣言を取り出す
    node_Function_Declaration* get_Prototype(){return Prototype;}
    // この関数のブロック部分を取り出す
    node_Block* get_FuncBlock(){return FuncBlock;}
};

// 関数宣言　プロトタイプ宣言
class node_Function_Declaration{
  // 関数の名前
  std::string Name;
  // 引数の名前
  std::vector<string> Params;

  public:
    node_Function_Declaration(const std::string &name, std::vecto<string> &params)
    : Name(name), Params(params){}
    ~node_Function_Declaration(){}

    std::string get_Name(){return Name;}
    // i番目の引数の名前を取り出す
    std::string get_Param(int i){
      if(i < Prams.size())
        return Params.at(i);
      else
        return NULL;
    }
    // 引数の数を取り出す
    int get_Params_Size(){return Params.size();}
};

// 関数やIF文などの｛｝で囲まれた部分 : Statementの集合
class node_Block {
  // Statementの集合
  std::vector<Node*> Statement_List;
  // 変数定義の集合
  // 変数定義のノードはここに入れる
  std::vector<node_Variable_Declaration*> Variable_Decl_List;

  public:
    node_Block(){}
    ~node_Block(){}

    // Statement_ListにNodeを追加する
    bool add_Statement(Node *stmt){Statement_List.pushback(stmt);}
    // Variable_Decl_Listに変数定義のNodeを追加する
    bool add_Variable_Decl(Variable_declaration *vdecl){}

    // Statement_Listからi番目の要素を取り出す
    Node* get_Statement(int i){
      if(i < Statement_List.size())
        return Statement_List.at(i);
      else
        return NULL;
    }
    // Variable_Decl_Listからi番目の要素を取り出す
    Node* get_Variable_Decl(int i){
      if(i < Variable_Decl_List.size())
        return Variable_Decl_List.at(i);
      else
        return NULL;
    }
};

// 変数宣言
class node_Variable_Declaration : public Statement{
  // 宣言の種類（関数内 or 引数部分)
  public:
    typdef enum{
      param,
      local
    }DeclType;

  private:
    // 変数の名前
    std::string Name;
    // 宣言の種類
    DeclType Type;

  public:
    node_Variable_Declaration(const std::string &name)
    : Node(VariableDeclID), Name(name){}
    ~node_Variable_Declaration(){}

    std:string get_Name(){return Name;}
    DeclType get_Type(){return Type;}

    bool set_Type(DeclType type){Type = type; return true;}
};

// if else文
//class node_IF_Statement : public Statement{

//};

// return文
class node_Return : public Statement{
  //戻り値となるexpr
  Node *Expr;

  public:
    node_Return(Node *expr) : Node(ReturnID), Expr(expr) {}
    ~node_Return(){}

    Node* get_Expr(){return Expr;}
};

// 二項演算子
class node_Binary_Operator : public Expression{
  // 二項演算子
  std::string Op;
  // 右辺
  Node *RHS;
  // 左辺
  Node *LHS;

  public:
    node_Binary_Operator(std::string *op, Node *rhs, Node *lhs)
    : Node(BinaryOpID), Op(op), RHS(rhs), LHS(lhs){}
    ~node_Binary_Operator(){}

    std::string get_Op(){return Op;}
    Node* get_RHS(){return RHS;}
    Node* get_LHS(){return LHS;}
};

// 変数参照
class node_Variable : public Expression{
  // 変数の名前
  std::string Name;

  public:
    node_Variable(const std::string &name) : Node(VariableID), Name(name){}
    ~node_Variable();

    std::string get_Name(){return Name;}
};

// Int型の数値
class node_Integer : public Expresion{
  // 数値
  int Val;

  public:
    node_Integer(int val) : Node(IntegerID), Val(val){}
    ~node_Integer();

    int get_Val(){return Val;}
};

// Double型の数値
class node_Double : public Expression{
  // 数値
  double Val;

  public:
    node_Double(double val) : Node(DoubleID), Val(val){}
    ~node_Double();

    int get_Val(){return Val;}
};

// Char型の数値
class node_Char : public Expression{
  // 数値
  char Val;

  public:
    node_Char(char val) : Node(CharID), Val(val){}
    ~node_Char();

    int get_Val(){return Val;}
};

// 関数呼び出し
class node_Function_Call : public Expression{
  // 呼び出される関数の名前
  std::string Callee;
  // 関数呼び出しの引数
  std::vector<Node*> Args;

  public:
    node_Function_Call(const std::string &callee, std::vector<Node*> &args)
     : Node(FuncCallID), Callee(callee), Args(args){}
    ~node_Function_Call();

    std::string get_Callee(){return Callee;}
    std::vector get_Args(){return Args;}
};
