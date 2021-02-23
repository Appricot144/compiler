#include"node.h"
// 識別子の名前の衝突を探す

//変数の記号表
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
        if( Table.at(i)->get_Name() == name){
          //二重定義エラー
          std::cout << "In Parser: duplicate declaration variable : " << name << std::endl;
          return false;
        }
      }

      //記号表にないなら登録
      node_Variable *var = new node_Variable(name);
      Table.push_back(var);
      return true;
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
      std::cout << "In Parser: Undefined variable : " << name << std::endl;
      node_Variable *tmp = new node_Variable(nullptr);
      return tmp;
    }

    // 変数の記号表をリフレッシュするメソッド
    void refresh_Table(){
      Table.clear();
    }

};

// 関数の記号表
//class function_Table{

//};
