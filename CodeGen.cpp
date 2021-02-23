#include "node.h"
#include "CodeGen.h"

// コンストラクタ
CodeGen::CodeGen(){
  Builder = new IRBuilder<>(Context);
  Mod = NULL;
}

// コード生成のトリガとなるdoCodeGen関数
// params : node_Program    Module名（入力ファイル）
// return 成功時 : true　 失敗時 : false
bool CodeGen::do_CodeGen(node_Program *Program, std::string name){
  return generate_Program(Program, name);
}

// Moduleの取得
Module &CodeGen::get_Module(){
  if(Mod)
    return *Mod;
  else
    return *(new Module("null", Context));
}



// Module生成
// params : node_Program , Module名（ソースコード名）
// return 成功時 : true　 失敗時 : false
// Programノードの内にあるFunction_DeclarationノードとFunctionノードを探索して、
// それぞれのコード生成メソッドを呼び出す。
bool CodeGen::generate_Program(node_Program *program, std::string name){
  //Module生成
  Mod = new Module(name, Context);

  // Function_declaration
  for(int i=0; ; i++){
    node_Function_Declaration *proto = program->get_Prototype(i);
    if(!proto)
      break;
    else if(!generate_Prototype(proto, Mod)){
      SAFE_DELETE(Mod);
      return false;
    }
  }

  // Function
  for(int i=0; ; i++){
    node_Function *func = program->get_Function(i);
    if(!func)
      break;
    else if(!generate_Function(func, Mod)){
      SAFE_DELETE(Mod);
      return false;
    }
  }

  return true;
}

// 関数宣言生成メソッド
// param  :node_Function_Declaration , Module
// retrun :生成したFunctionのポインタ
// 関数の再定義を検知し、Moduleにプロトタイプ宣言を追加する
Function* CodeGen::generate_Prototype(node_Function_Declaration *proto, Module *mod){
  // 二重定義の確認
  Function *func = mod->getFunction(proto->get_Name());
  if(func){
    if(func->arg_size()==proto->get_Params_Size() && func->empty()){
      return func;
    }else{
      fprintf(stderr, "In CodeGen: duplicate declaration function : %s\n", proto->get_Name());
      return NULL;
    }
  }

  // Moduleに追加されていないなら追加する。

  //create arg_types
  std::vector<Type*> int_types(proto->get_Params_Size(),
                               Type::getInt32Ty(Context) );
  //create func type
  FunctionType *func_type = FunctionType::get(Type::getInt32Ty(Context),
                                              int_types,
                                              false );
  //create function
  func = Function::Create(func_type, Function::ExternalLinkage, proto->get_Name(), mod);

  // 設定したfucntionの引数に名前をつける
  Function::arg_iterator arg_iter=func->arg_begin();
  for(int i=0; i<proto->get_Params_Size(); i++){
    arg_iter->setName(proto->get_Param(i)->get_Name().append("_arg") );
    arg_iter++;
  }

  return func;
}

//関数の生成メソッド
// params : node_Function(関数) , Module(ソースコード)
// return : 生成したFunctionのポインタ
//generate_Prototypeで生成したFunctionにブロック部分の追加。
//generate_Blockによるブロックの生成。
Function* CodeGen::generate_Function(node_Function *func_node, Module *mod){
  Function *func = generate_Prototype(func_node->get_Prototype(), mod);
  if(!func){
    return NULL;
  }
  Current_Func = func;
  BasicBlock *bblock = BasicBlock::Create(Context, "entry", func);
  Builder->SetInsertPoint(bblock);

  // 引数をBlockノードに格納する(要改善)
  node_Function_Declaration *proto = func_node->get_Prototype();
  node_Block *block = func_node->get_FuncBlock();
  node_Variable_Declaration *arg;
  for(int i=0; i<proto->get_Params_Size(); i++){
    arg = proto->get_Param(i);
    block->add_Statement(arg);
  }

  //Functionのボディを生成する
  generate_Block(func_node->get_FuncBlock());

  return func;
}

//ブロックの生成メソッド
// param  : node_Block
// return : 最後に生成したValueのポインタ
//変数宣言のみブロックの前半に固めて命令を生成する
//ブロック内のStatementノードを探索し、各種命令生成メソッドを呼び出す。
Value* CodeGen::generate_Block(node_Block *block){

  node_Variable_Declaration *v_decl;
  Value *v = NULL;

  //Variable Declaration
  node_Statement *stmt;
  for(int i=0; ; i++){
    stmt = block->get_Statement(i);
    if(!stmt){
      break;
    }

    if( stmt->get_NodeID()==VariableDeclID ){
      //create allocate
      v_decl = dyn_cast<node_Variable_Declaration>(stmt);
      v=generate_Variable_Declaration(v_decl);
    }

  }

  //other Statement
  for(int i=0; ; i++){
    stmt = block->get_Statement(i);
    if(!stmt){
      break;
    }

    v = generate_Statement(stmt);
  }

  return v;
}

//変数宣言生成メソッド（alloca命令）
// param  : node_Variable_Declaration (変数宣言ノード)
// return : 生成したValueのポインタ
//alloca命令を生成しする．argsだった時は引数の値を格納する．
Value *CodeGen::generate_Variable_Declaration(node_Variable_Declaration *v_decl){
  //create allocate
  AllocaInst *alloca = Builder->CreateAlloca(Type::getInt32Ty(Context),
                                            0,
                                            v_decl->get_Name());

  //if args allocate
  if(v_decl->get_Type() == node_Variable_Declaration::param){
    //store args
    ValueSymbolTable *vs_table = Current_Func->getValueSymbolTable();
    Builder->CreateStore(vs_table->lookup(v_decl->get_Name().append("_arg")), alloca);
  }

  return alloca;
}

//Statement生成メソッド
// param  : node_Statement
// return : 生成したValueのポインタ
//引数として与えられたStatementノードの種類を判別して，
//各種コード生成メソッドを呼び出す．
Value *CodeGen::generate_Statement(node_Statement *stmt){
  if(isa<node_Function_Call>(stmt)){
    return generate_Function_Call(dyn_cast<node_Function_Call>(stmt));
  }else if(isa<node_Return>(stmt)){
    return generate_Return(dyn_cast<node_Return>(stmt));
  }else if(isa<node_Expression>(stmt)){
    return generate_Expression(dyn_cast<node_Expression>(stmt));
  }else{
    return NULL;
  }
}

//二項演算生成メソッド
// param  : node_Binary_Operator(二項演算子ノード)
// return : 生成したValueのポインタ
//
Value *CodeGen::generate_Binary_Expression(node_Binary_Operator *bin_expr){
  node_Expression *lhs = bin_expr->get_LHS();
  node_Expression *rhs = bin_expr->get_RHS();
  Value *lhs_v;
  Value *rhs_v;

  //assignment
  if(bin_expr->get_Op() == '='){
    //lhs is variable
    node_Variable *lhs_var = dyn_cast<node_Variable>(lhs);
    ValueSymbolTable *vs_table = Current_Func->getValueSymbolTable();
    lhs_v = vs_table->lookup(lhs_var->get_Name());

  //other oeprand
  }else{
    //lhs is ?
      //binary?
    if(isa<node_Binary_Operator>(lhs)){
      lhs_v = generate_Binary_Expression(dyn_cast<node_Binary_Operator>(lhs));

      //Variable?
    }else if(isa<node_Variable>(lhs)){
      lhs_v = generate_Variable(dyn_cast<node_Variable>(lhs));

      //Integer?
    }else if(isa<node_Integer>(lhs)){
      node_Integer *val = dyn_cast<node_Integer>(lhs);
      lhs_v = generate_Integer(val->get_Val());
    }

  }

  // create rhs value
    //binary Expression?
  if(isa<node_Binary_Operator>(rhs)){
    rhs_v = generate_Binary_Expression(dyn_cast<node_Binary_Operator>(rhs));

    //Variable?
  }else if(isa<node_Variable>(rhs)){
    rhs_v = generate_Variable(dyn_cast<node_Variable>(rhs));

    //Funcion call?
  }else if(isa<node_Function_Call>(rhs)){
    rhs_v = generate_Function_Call(dyn_cast<node_Function_Call>(rhs));

    //Integer?
  }else if(isa<node_Integer>(rhs)){
    node_Integer *val = dyn_cast<node_Integer>(rhs);
    rhs_v = generate_Integer(val->get_Val());
  }

  if(bin_expr->get_Op() == '='){
    //store
    return Builder->CreateStore(rhs_v, lhs_v);
  }else if(bin_expr->get_Op() == '+' ){
    //add
    return Builder->CreateAdd(lhs_v, rhs_v, "add_tmp");
  }else if(bin_expr->get_Op() == '-'){
    //sub
    return Builder->CreateSub(lhs_v, rhs_v, "sub_tmp");
  }else if(bin_expr->get_Op() == '*'){
    //mul
    return Builder->CreateMul(lhs_v, rhs_v, "mul_tmp");
  }else if(bin_expr->get_Op() == '/'){
    //div
    return Builder->CreateSDiv(lhs_v, rhs_v, "div_tmp");
  }
}


//関数呼び出し生成メソッド(Call命令)
// params : node_Function_Call(関数呼びだしノード)
// return : 生成したValueのポインタ
//
Value *CodeGen::generate_Function_Call(node_Function_Call *func_call){
  std::vector<Value*> arg_vec; //args
  node_Expression *arg;
  Value *arg_v;
  ValueSymbolTable *vs_table = Current_Func->getValueSymbolTable();

  //args
  for(int i=0; ; i++){
    if(!(arg=func_call->get_Args(i))){
      break;
    }

      //is Call
    if(isa<node_Function_Call>(arg)){
      arg_v = generate_Function_Call(dyn_cast<node_Function_Call>(arg));

      //is Binary Expression
    }else if(isa<node_Binary_Operator>(arg)){
      arg_v = generate_Binary_Expression(dyn_cast<node_Binary_Operator>(arg));
      node_Binary_Operator *bin_expr = dyn_cast<node_Binary_Operator>(arg);
      if(bin_expr->get_Op() == '='){ //代入演算の時は、store命令ではなくload命令を出力する
        node_Variable *var = dyn_cast<node_Variable>(bin_expr->get_LHS());
        arg_v = Builder->CreateLoad(vs_table->lookup(var->get_Name()), "arg_val");
      }

      //is Variable
    }else if(isa<node_Variable>(arg)){
      arg_v = generate_Variable(dyn_cast<node_Variable>(arg));

      //is Integer
    }else if(isa<node_Integer>(arg)){
      node_Integer *val = dyn_cast<node_Integer>(arg);
      arg_v = generate_Integer(val->get_Val());

    }
    arg_vec.push_back(arg_v);
  }

  //create function call
  return Builder->CreateCall(Mod->getFunction(func_call->get_Callee()),
                             arg_vec,
                             "call_tmp");
}

//Return生成メソッド
// params : node_Return
// return : 生成したValueのポインタ
Value *CodeGen::generate_Return(node_Return *ret){
  node_Expression *expr = ret->get_Expr();
  Value *ret_v;

    //Expr ?
    //is Binary Expression
  if(isa<node_Binary_Operator>(expr)){
    ret_v = generate_Binary_Expression(dyn_cast<node_Binary_Operator>(expr));
    //is Function call
  }else if(isa<node_Function_Call>(expr)){
    ret_v = generate_Function_Call(dyn_cast<node_Function_Call>(expr));
    //is Variable
  }else if(isa<node_Variable>(expr)){
    ret_v = generate_Variable(dyn_cast<node_Variable>(expr));
    //is Integer
  }else if(isa<node_Integer>(expr)){
    node_Integer *val = dyn_cast<node_Integer>(expr);
    ret_v = generate_Integer(val->get_Val());
  }

  Builder->CreateRet(ret_v);
}

//Expression生成メソッド
// params : node_Expression
// return : 生成したValueのポインタ
Value *CodeGen::generate_Expression(node_Expression *expr){
  if(isa<node_Binary_Operator>(expr)){
    return generate_Binary_Expression(dyn_cast<node_Binary_Operator>(expr));
  }else if(isa<node_Function_Call>(expr)){
    return generate_Function_Call(dyn_cast<node_Function_Call>(expr));
  }else{
    return NULL;
  }
}

//変数参照のコード生成メソッド
// Params : node_Variable
// return : 生成したValueのポインタ
Value *CodeGen::generate_Variable(node_Variable *var){
  ValueSymbolTable *vs_table = Current_Func->getValueSymbolTable();
  return Builder->CreateLoad(vs_table->lookup(var->get_Name()), "var_tmp");
}

//int型定数の生成メソッド
// Params : int (生成する定数の値)
// return : 生成したValueのポインタ
Value *CodeGen::generate_Integer(int value){
  return ConstantInt::get(Type::getInt32Ty(Context), value);
}
