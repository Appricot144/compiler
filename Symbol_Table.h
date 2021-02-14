#include"node.h"
// 識別子の名前の衝突を探す

class variable_Table{
  // 記号表
  std::vector<node_Variable*> Table;

  public:
    variable_Table(){}
    ~variable_Table(){}

    // 同名の変数が無いか調べる、無かったら記号表に登録する。
    // あった場合、二重定義エラー。
    bool add_Tuple(std::string &name){
      for(int i=0; i<Table.size(); i++){
        if( Table.at(i)->get_Name() == name)
          break;
        else{
          node_Variable *var = new node_Variable(name);
          Table.push_back(var);
          return true;
        }
      }
      //二重定義エラー
      std::cout << "In Parser: duplicate declaration variable : %s" << name << std::endl;
      return false;
    }
    // 渡された変数が宣言済みの変数か調べる。
    // 宣言済みであった場合、そのインスタンスのポインタを返す。
    // 記号表に変数が無かった場合、未定義参照エラー。
    node_Variable* get_Tuple(std::string &name){
      for(int i=0; i<Table.size(); i++){
        if( Table.at(i)->get_Name() == name)
          return Table.at(i);
      }

      //未定義参照エラー
      std::cout << "In Parser: Undefined variable : %s" << name << std::endl;
      node_Variable *tmp = new node_Variable(nullptr);
      return tmp;
    }
};
