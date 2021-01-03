# Python 解释器

Python-Interpreter-2020

## `BigNumber.hpp`

* 实现大整数运算
*  成员变量
  * `bool sign;` 1表示正数，0表示负数（*数字`0`两种符号皆可）
  * `vector<int> digit;` 
* 成员函数
  * `bool empty() const;` 判断是否为`0`
  * 重载加减乘除、取模，大小比较等操作
  * `explicit operator string() const;`
  * `explicit operator double() const;`

## `AnyType.hpp`

* 实现不同类型间统一管理，类型包括`int, float, bool, str, None, break, continue, return`
* 成员变量：除了`None, break, continue`，每种类型对应一个成员变量（*return变量定义：`vector<AnyType> return_type`）
* 成员函数
  * 实现类型转换，`put2int(), put2float(), put2str(), put2bool()`，返回类型是`AnyType`
  * `static void convert(AnyType &, AnyType &)` 将两个AnyType转换为同类型
  * 重载加减乘除、取模，大小比较等操作，**所有运算符返回类型皆为`AnyType`**

## `Namespace.hpp`

* 实现对命名空间的统一管理
* 成员变量
  * `map<string, AnyType> global`
  * `vector<map<string, AnyType> > stack1` 管理`while | if`的`suite`
  * `vector<map<string, AnyType> > stack2` 管理自定义函数的`suite`
  * `vector<int> stack0` 总栈，存每一次压栈的类型(`1 | 2`)
* 进入函数体，把传入的参数压栈，进入`while | if`，压入一层空`map`；退出`suite`时弹栈
* 对于`stack2`，先在栈顶查找，没有找到再查找`global`，没有则在栈顶新建
* 对于`stack1`，从栈顶往下查找，直到查到`stack2`停止，最后查找`global`，没有则在栈顶新建

## `Evalvisitor.hpp`

* `VisitTest`返回`string | AnyType`，`string`表示变量名；`AnyType`表示一个右值
* `visitTestlist`返回`vector<antlrcpp::Any>`其中`antlrcpp::Any`类型是`string | AnyType`

