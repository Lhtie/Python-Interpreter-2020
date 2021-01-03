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
  * `map<string, AnyType> global`全局变量存储池
  * `vector<map<string, AnyType> > stack`手工栈空间
* 进入函数体，把传入的参数放入新的一层`map`压栈；退出函数体时弹栈
* **对于`while | if`的`suite`，只需要使用栈顶的命名空间**

## `Evalvisitor.hpp`

* `VisitTest`返回`string | AnyType`，`string`表示变量名；`AnyType`表示一个右值
* `visitTestlist`返回`vector<antlrcpp::Any>`其中`antlrcpp::Any`类型是`string | AnyType`

## 卡常

所有的传参是否是`const`引用，无关紧要

不同命名空间的实现对常数影响很小（虽然我的实现已经很简单了）

乘除不需要故意优化（虽然这里常数如果很大的话，BigIntegerTest过不去）

**常数最大的是`ctx`对`vector`的构造**

* 正常的写法应该是：先把`ctx`构造的`vector`存入自己定义的`vector`中，再操作
* **不要过多使用`ctx->...(int)`**