#include "node.h"
#include "CodeGen.h"
#include "parser.hpp"

// コンストラクタ
CodeGen::CodeGen(){
  Builder = new IRBuilder<>(getGlobalContext());
  Mod = NULL;
}

// コード生成のトリガとなるdoCodeGen関数
// params : node_Program    Module名（入力ファイル）
// return 成功時 : true　 失敗時 : false
bool CodeGen::do_CodeGen(node_Program &program, std::string name){
  return generate_Program(program, name);
}

// Moduleの取得
Module &CodeGen::get_Module(){
  if(Mod)
    return *Mod;
  else
    return *(new Module("null", getGlobalContext()));
}



// Module生成
// params : node_Program , Module名（ソースコード名）
// return 成功時 : true　 失敗時 : false
// Programノードの内にあるFunction_DeclarationノードとFunctionノードを探索して、
// それぞれのコード生成メソッドを呼び出す。
bool CodeGen::generate_Program(node_Program &program, std::string name){
  //Module生成
  Mod = new Module(name, getGlobalContext());

  // Function_declaration
  for(int i=0; ; i++){
    node_Function_Declaration *proto = program.get_Prototype(i);
    if(!proto)
      break;
    else if(!generate_Prototype(proto, Mod)){
      SAFE_DELETE(Mod);
      return false;
    }
  }

  // Function
  for(int i=0; ; i++){
    node_Function *func = program.get_Function(i);
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
      fpritnf(stderr, "In CodeGen: duplicate declaration function : %s\n", proto->get_Name());
      return NULL;
    }
  }

  // Moduleに追加されていないなら追加する。

  //create arg_types
  std::vector<Type*> int_types(proto->get_Params_Size(),
                               Type::getInt32Ty(getGlobalContext()) );
  //create func type
  FunctionType *func_type = FunctionType::get(Type::getInt32Ty(getGlobalContext()),
                                              int_types,
                                              false);
  //create function
  func = Function::Create(func_type, Function::ExternalLinkage, proto->getName(), mod);

  // 設定したfucntionの引数に名前をつける
  Function::arg_iterator arg_iter=func->arg_begin();
  for(int i=0; i<proto->get_Params_Size(); i++){
    arg_iter->setName(proto->get_Param(i));
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
}

//ブロックの生成メソッド
Value* CodeGen::generate_Block(node_Block *block){

}

//変数宣言生成メソッド（alloca命令）
// param  : node_Variable_Decralation (変数宣言ノード)
// return : 生成したValueのポインタ
//alloca命令を生成しする．argsだった時は引数の値を格納する．
Value *Codegen::generate_Variable_Declaration(node_Variable_Decralation *v_decl){
  //create allocate
  AllocaInt *alloca = Builder->CreateAlloca(Type::getInt32ty(getGlobalContext()),
                                            0,
                                            v_decl->get_Name());

  //if args allocate
  if(v_decl->get_Type() == node_Variable_Decralation::param){
    //store args
    ValueSymbolTable &vs_table = Current_Func->getValueSymbolTable();
    Builder->CreateStore(vs_table.lookup(v_decl->get_Name()), alloca);
  }

  return alloca;
}


//Statement生成メソッド
// param  : node_Statement
// return : 生成したValueのポインタ
//引数として与えられたStatementノードの種類を判別して，
//各種コード生成メソッドを呼び出す．
Value *CodeGen::generate_Statement(node_Statement *stmt){
  if(isa<node_Binary_Expression>(stmt)){
    return generate_Binary_Expression(dyn_cast<node_Binary_Expression>(stmt));
  }else if(isa<node_Function_Call>(stmt)){
    return generate_Function_Call(dyn_cast<node_Function_Call>(stmt));
  }else if(isa<node_Return>(stmt)){
    return generate_Return(dyn_cast<node_Return>(stmt));
  }else if(isa<generate_Expression>){
    return generate_Expression(dyn_cast<node_Expression>(stmt));
  }else{
    return NULL;
  }
}

//二項演算生成メソッド
// param  :
// return :
//
