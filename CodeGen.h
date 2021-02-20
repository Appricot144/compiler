#ifndef ___CODEGEN_H
#define ___CODEGEN_H

//　コード生成するクラス
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/PrettyStackTrace.h>
#include "llvm/Support/Debug.h"
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Support/raw_ostream.h>
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/IR/LegacyPassManager.h"

//#include <llvm/Bitcode/ReaderWriter.h>
//#include <llvm/ExecutionEngine/GenericValue.h>
//#include <llvm/ExecutionEngine/JIT.h>

using namespace llvm;

// Program（翻訳単位）一つにつき一つのコード生成メソッドを生成する。
// Programノードを深さ優先で探索し、LLVM-IRを生成する。
class CodeGen{

  private:
    // 現在コード生成中のFunction
    Function *Current_Func;
    // 生成したModule格納
    Module *Mod;
    // LLVM-IRを生成するIRBuilderクラス
    IRBuilder<> *Builder;
    // 定数や型の表を含めた、グローバル情報の管理をするもの
    // ロックの保障がないので１スレッドにつき一つだけ使うこと。
    LLVMContext Context;

  public:
    CodeGen();
    ~CodeGen(){}

    bool do_CodeGen(node_Program *Program, std::string name);
    Module &get_Module();

  private:
    bool generate_Program(node_Program *program, std::string name);
    Function *generate_Prototype(node_Function_Declaration *proto, Module *mod);
    Function *generate_Function(node_Function *func, Module *mod);
    Value *generate_Block(node_Block *block);
    Value *generate_Variable_Declaration(node_Variable_Declaration *v_decl);
    Value *generate_Statement(node_Statement *stmt);
    Value *generate_Binary_Expression(node_Binary_Operator *bin_expr);
    Value *generate_Function_Call(node_Function_Call *func_call);
    Value *generate_Return(node_Return *ret);
    Value *generate_Expression(node_Expression *expr);
    Value *generate_Variable(node_Variable *var);
    Value *generate_Integer(int value);
};

#endif
