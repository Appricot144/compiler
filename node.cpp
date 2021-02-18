#include "node.h"

//Programノードが空かどうかを判定するメソッド
// param  : void
// retrun : 空:true ,それ以外:false
bool node_Program::empty(){
  if(Function_Decl_List->size() == 0 && Function_List.size() == 0 )
    return true;
  return false;
}
