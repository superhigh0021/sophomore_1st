# 条款1 视C++为一个语言联邦

没啥好说的



# 条款2 尽量以const, enum, inline替换#define

- 宏定义的变量可能不会进入符号表(symbol table)

在define之后，可能在运用该常量的过程中获得一个编译错误，这个错误也许会提到定义的结果，而不会提到那个常量的名字，这样会导致困惑，因为需要追踪那个常量就显得十分困难。

const定义的**语言常量**肯定会被编译器看到，因此会进入符号表

- 宏定义无法创建class专属常量

**#define 不重视作用域**

一旦宏被定义，就在其后的变异过程中有效  所以#define也没有封装性

- 一个属于枚举类型的数值可以充当int使用

```
#class GamePlayer{
private:
	enum{NumTurns=5};

	int scores[NumTurns];
};
```

- 对于形如函数的宏，最好用inline函数替换#define

**现代编译器会自动inline，而不是根据编写代码者是否添加inline来判断**



# 条款3 尽可能使用const

T* const  指针不得指向不同东西

const T*       T const*   指针指向的东西不可变动

**class的const成员函数可以操作const对象**

**如果函数返回类型是内置类型，那么改动函数返回值不合法**    **对返回值的改动必须要求函数返回值是左值引用**

**const成员函数可以使用类的成员，但不能修改他们**

**类的const成员变量（常量）只能通过构造函数的初始化列表**



# 条款4 确定对象被使用前已被初始化

**在使用前确保初始化**

**构造函数一定要用member initialization list进行构造 而不是对于每个成员挨个赋值**

**原因在于 基于赋值的版本会先调用default构造函数对成员进行默认初始化，然后再调用这个版本的构造函数进行赋值动作的初始化**

**效率远远不如member initialization list的构造**

**如果成员变量是const或reference，就一定需要初值，不能被赋值**

**C++有十分固定的成员初始化次序，class的成员变量总是以其声明次序被初始化，不论它们在构造函数中的顺序是什么样的 但是最好按照声明顺序进行初始化**

**Singleton模式**：

C++在多单元编译的时候  编译顺序具有不确定性  **以local static对象 替换non-local static对象**   **这部分注意看原书**



# 条款5 了解C++默默编写并调用哪些函数

默认函数只在被需要的时候才产生

编译器产出的析构函数是non-virtual的

**在内含const成员、reference成员的类中，编译器会拒绝编译赋值操作的代码。因为C++并不允许 让reference和const对象改指向不同对象**



# 条款6 若不想使用编译器自动生成的函数，就该明确拒绝

侯捷给出的做法是将拷贝相关函数声明为private并且不予实现  **过时**

C++11可以通过在相关函数的声明后跟=delete 实现删除该函数

1.将拷贝函数声明为private并不绝对安全 因为成员函数和友元函数还是可以调用private函数

2.如果定义为private且不定义的话  对它们的调用将会获得一个 **链接错误**

所以其实还是不太方便



# 条款7 为多态基类声明virtual析构函数

派生类对象被绑定到基类指针或者引用上  在调用析构函数时，如果不是virtual析构函数，那么派生类中的部分没有被销毁

**欲实现virtual函数 对象必须携带某些信息 主要用来在运行期决定哪一个virtual函数被调用**  **这些信息通常由vptr指针指出  vptr指向一个由函数指针构成的数组，称为vtbl(virtual table)**   **每一个带有virtual函数的class都有一个相应的vtbl 当对象调用某一virtual函数 实际上取决于该对象的vptr所指向的那个vtbl**

**如果类内包含virtual函数，其对象体积会增加**



# 条款8 别让异常逃离析构函数

类不希望析构函数可以产生异常

析构函数应该捕捉任何异常 然后吞下它们或结束程序

如果需要对某个操作函数运行期间抛出的异常做出反应，那么class应该提供一个普通函数而非在析构函数中执行该操作



# 条款9 绝不在构造或析构函数中调用virtual函数

如果在构造函数中调用virtual函数  那么创建一个派生类对象的时候 基类构造优先于派生类构造 此时调用的virtual函数是基类的对应函数  **因为在base class构造期间 virtual函数不是virtual函数** 

**在派生类对象的基类构造期间，对象的类型是base class而不是derived class**

不止virtual函数会被编译器解析之base class，如果使用运行期类型信息 入dynamic-cast和typeid 也会把对象视作base class

一个可行的做法是 在derived class的构造函数中 将一个static派生类对象作为参数传递给base class的构造函数  这样就避免了调用一个未初始化/未生成的对象



# 条款10 令operator=返回一个reference to *this

内置数据类型可以连锁赋值   如果希望自定义数据类型也支持连锁赋值 **那么必须令operator=返回一个*this的引用** 



# 条款11 在operator=中处理“自我赋值”

不要认为自我赋值不存在  

```cpp
a[i]=a[j]   //潜在的自我赋值
```

这一类并不明显的自我赋值  是“别名”带来的结果

**一般来说  在operator=的最前面加上一个“证同测试”即可**  但是这个方法不是异常安全的  

**正确操作：记住原来的 令其指向新的  再删除它**



# 条款12 复制对象时勿忘其每一个成分

当为派生类写拷贝函数的时候必须要考虑到base class的成分   那些成分有可能还private的  所以必须小心复制base class成分  调用base class的拷贝函数

**不应该让拷贝复制操作符调用拷贝构造函数**

当发现拷贝构造函数和拷贝赋值操作符之间有相近的代码  消除重复代码的做法是 建立一个新的成员函数给两个函数调用  **该函数一般是private的  且常被命名为init**



# 条款13 以对象管理资源

- RAII对象：

​	为防止资源泄漏，使用RAII对象  它们在构造函数中获得资源并在析构函数中释放资源



# 条款14 在资源管理类中小心coping行为

**复制底层资源**：复制资源管理的对象 应该同时也复制其所包覆的资源   复制资源管理对象时进行的是**深拷贝**

普遍而常见的RAII行为是：阻止拷贝、运用引用计数法。 不过其他行为也都可能被实现。



# 条款15 在资源管理类中提供对原始资源的访问 

//! 智能指针提供get成员函数 用来返回智能指针内部的原始指针(的副本)

如果觉得每一次都需要调用get非常麻烦  可以令为资源管理类提供隐式转换函数：

```cpp
class Font{
public:
    ...
    operator FontHandle()const{return f;}
};
```

但是隐式类型转换可能会引发问题

```cpp
Font f1(getFont());
...
FontHandle f2=f1;//本意是拷贝一个Font对象 但是隐式转换为FontHandle才复制它
```

f1和f2都管理一个FontHandle，如果f1被销毁 那么f2因此称为“虚吊的”。

**对原始资源的访问可能经由显式转换或隐式转换。一般而言显式转换比较安全，但隐式转换对客户比较方便**



# 条款16 成对使用new和delete时要采用相同形式

delete的最大问题在于:即将被删除的内存之内究竟有多少对象?**这决定了有多少个析构函数被调用**



# 条款17 以独立语句将newed对象置入智能指针

- 智能指针的构造函数是explicit的,不能隐式类型转换

```cpp
//函数声明
int priority();
void process(std::shared_ptr<Widget>pw,int priority);

process(new Widget,priority());  //无法通过编译
process(std::shared_ptr<Widget>(new Widget),priority())//可以通过 但可能泄漏资源
```

C++编译器核算函数参数的顺序是不固定的！

如果按照如下顺序：

①执行 new Widget

②调用priority

③调用std::shared_ptr构造函数

**如果对priority的调用产生异常，new Widget返回的指针将会遗失，因为尚未被置入std::shared_ptr内**

---

解决方法：使用分离语句构造智能指针

```cpp
std::share_ptr<Widget>pw(new Widget);
process(pw,priority());  	//大功告成！
```

**编译器对于 “跨越语句的各项操作” 没有重新排列的自由**  只有在一条语句内部才有重排的自由度



# 条款18 让接口容易被正确使用，不易被误用

std::shared_ptr有一个特别好的性质：它会自动使用他的 ”每个指针专属的删除器“ ，银耳消除另一个潜在的客户错误：cross-DLL problem。

这个问题发生于 对象在动态链接程序库中被new创建，却在另一个DLL内被delete销毁。**会导致运行期错误**    std::shared_ptr没有这个错误 因为它默认的删除器是来自它诞生的那个DLL的delete。

---

Boost库中的shared_ptr是原始值镇的两倍大，以动态分配内存作为簿记用途和 ”删除器之专属数据“ ，以virtual形式调用删除器，并在多线程程序修改引用次数时蒙受线程同步化的额外开销。(只要定义一个预处理器符号就可以关闭多线程支持)

**他比原始指针大且慢，而且使用辅助动态内存**  但是降低客户使用成本倒是真的！

# 条款19 设计class犹如设计type

- 新的type对象如果被pass by value 会调用拷贝构造函数
- 新的type需要什么样的转换？  如果希望允许类型T1被隐式转换为类型T2，就必须①在class T1中写一个类型转换函数operator T2 或者②在class T2中写一个non-explicit-one-argument构造函数



# 条款20 宁以pass-by-reference-to-const替换pass-by-value

- 以by-reference方式传递参数也可以避免对象切割的问题

如果一个derived对象以by value的方式传递并被视为一个base class对象，base class的构造函数会被调用，derived class部分将被切割，只留下base class部分

---

- 在C++编译器底层，reference是以指针实现出来的，因此pass by reference通常意味着真正传递的是指针

---

以上规则不适用于内置类型，以及STL的迭代器和函数对象。对他们而言，pass by value比较适当。



# 条款21 必须返回对象时，别妄想返回其reference

- 任何一个函数如果返回一个reference指向某个local对象，都将一败涂地

不要返回一个pointer或reference指向一个local stack对象，或返回reference指向一个heap-allocated对象，或pointer或reference指向一个local static对象而有可能同时需要多个这样的对象。

在对象之间搬移数值的最直接办法是通过**赋值**操作，代价：调用一个析构函数加上一个析构函数 (销毁旧值，复制新值)

**有时候operator\*的返回值的构造和析构可被安全的消除**



# 条款22 将成员变量声明为private

- 如果public接口内全是函数，客户就不需要在打算访问class成员时迷惑是否使用小括号

将成员变量隐藏在函数接口的背后，可以为 “所有可能的实现” 提供弹性

---

假设有一个public成员变量，而我们最终取消了它，有多少代码可能会被破坏？因为public成员变量完全没有封装性。改动protected成员变量将会影响derived class。

- 如果将一个成员变量声明为public或protected而客户开始使用它，就很难改变那个成员变量所涉及的一切，太多代码需要重写、重新测试、重新编译！

- protected并不比public更具有封装性。从封装角度而言，只有两种访问权限：private和其他



# 条款23 宁以non-member、non-friend替换member函数

- 越多函数可以访问它，数据的封装性就越低

如果一个member函数和non-member,non-froend函数之间做抉择，而且两者提供相同机能，那么**导致较大封装性的是后者**，因为它并不增加 “能够访问class内之private成分” 的函数数量。



# 条款24 若所有参数皆需类型转换，请为此采用non-member函数

假如你这样开始你的Rational class：

![image-20220223023425027](dependence/image-20220223023425027.png)

```cpp
Rational oneHalf(1,2);
Rational result = oneHalf * 2;  //成功！
result = 2*onewHalf;	//失败
```

当用函数形式重写  问题便一目了然

```cpp
result = onewHalf.operator*(2);
result = 2.operator*(oneHalf);
```

- oneHalf内含operator*，所以编译器调用该函数。并且发生了隐式类型转换。

- 然而整数2没有这个成员函数。编译器会尝试调用non-member operator*，但是不存在

**只有当参数被列于参数列表中，这个参数才是隐式类型转换的合格参与者**

---

让operator*成为一个non-member函数，就允许编译器在每个实参身上执行隐式类型转换

**如果需要为某个函数的所有参数(包括this指针所指的那个隐喻参数)进行类型转换，那么这个函数必须是non-member的**



# 条款25 考虑写出一个不抛异常的swap函数

- 只要类型T类型支持拷贝(拷贝构造函数和拷贝复制操作符)，swap就可以完成任务

通常我们不允许改变std命名空间内的任何东西，但可以为标准templates制造特化版本，使它专属于我们自己的classes。

**C++只允许对class templates偏特化，不能在function templates上偏特化**

当你打算偏特化一个function template时，惯常做法是为它添加一个重载版本(而不是企图特化它)

**重载标准库的内容是被允许的，但是添加是不被允许的！**

---

如果swap默认实现版效率不足(意味着你的class或template使用了pimpl手法)，尝试：

①提供一个public swap成员函数，在其内高效置换两个对象值

②在你的class或template所在命名空间内提供一个non-member swap，并令它调用swap成员函数

③如果正在编写一个class(而不是一个class template)，为你的class特化std::swap，并令它调用swap成员函数

**如果你调用swap，请确定包含一个using声明式，让std::swap可见**  此处不用std::swap，是为了给出很多选择，让编译器自行去根据合适度进行匹配，而不是固定调用！

# 条款26 尽可能延后变量定义式的出现时间

只要定义了一个变量而其类型带有一个构造函数或析构函数,那么程序控制流到达这个变量定义式时,便要承担构造成本;当它离开作用域时便要承担析构成本。即使这个变量最后并未被使用

**如果程序在中间抛出异常的话，在这之前声明的变量就成了“未被使用而被声明的变量”**

- 不止应该延后变量的定义，甚至应该延后直到能够给它初值实参为止，不仅可以避免构造和析构非必要对象，还可以避免**无意义的default构造行为**

![image-20220226011801223](dependence/image-20220226011801223.png)

| 方法A                   | 方法B                   |
| ----------------------- | ----------------------- |
| 1个构造+1个析构+n个赋值 | n个构造函数+n个析构函数 |

做法A会造成名称w作用域比做法B更大

除非

1. 你的赋值成本比构造+析构成本低
2. 你正在处理的代码中效率高度敏感部分

否则应该用方法B

# 条款27 尽量少做转型动作

- C++提供四种新式转型：

const_cast<T>(expression)    

dynamic_cast<T>(expression)   

reinterpret_cast<T>(expression)    

static_cast<T>(expression) 



- const_cast:将对象的常量性移除，也是唯一有此能力的
- dynamic_cast 主要用来执行“安全向下转型”，是唯一无法由旧式语法执行的动作。可能会耗费重大运行成本。
- reinterpret_cast执行低级转型，实际动作取决于编译器，所以不可移植。e.g.将pointer to int 转型为一个 int
- static_cast用来强迫隐式转换，包括将non-const转换为const，但不可以反过来转换，因为那是const_cast的事。

![image-20220226193722389](E:\sophomore_1st\EffectiveC++\dependence\image-20220226193722389.png)

derived class中的virtual函数首先调用父类的virtual函数，但是此处的调用并不是我们所想要的，而是*this对象的base class成分的暂时副本的onResize。

解决方法是拿掉转型动作，只需要使用Window::onResize();

---

- dynamic_cast的许多实现版本执行速度非常慢

之所以需要dynamic_cast，通常是因为需要在一个认定为derived class对象身上执行derived class操作函数，但是手上却只有一个指向base的pointer或reference。

**避免连串的dynamic_cast的使用，会非常慢**

# 条款28 避免返回handles指向对象内部成分

 返回一个代表对象内部数据的handle，会带来降低对象封装性的风险。他可能会导致“虽然调用const成员函数却造成对象状态被更改"。

![image-20220226203240260](E:\sophomore_1st\EffectiveC++\dependence\image-20220226203240260.png)

# 条款29 为”异常安全“而努力是值得的

异常安全函数提供以下三个保证之一：

- 基本承诺：如果异常被抛出，程序内的任何事物仍然保持在有效状态下。没有任何对象和数据结构会因此而败坏。
- 强烈保证：如果异常被抛出，程序状态不改变。如果函数成功，就是完全成功；如果函数失败，程序就会恢复到调用函数状态之前。
- no-throw保证：承诺绝不抛出异常。

copy and swap：为你打算修改的对象做出一个副本，然后再那个副本身上修改。若有任何修改动作抛出异常，原对象仍保持为改变状态。待所有改变都成功后，再将修改过的那个副本和原对象在一个不抛出异常的操作中swap。

# 条款30 透彻了解inlining的里里外外

B站UP主 双生子佯廖 说过现代编译器会自动inline它认为值得inline的函数，无论是否手动inline。

**在内存有限的机器上，inline造成的代码膨胀会导致额外的换页行为，降低指令高速缓存的命中率**

**在类内声明并定义的成员函数会进行隐式inline申请**

- 对于virtual函数的inline调用会落空，因为virtal意味着等待，直到运行期才确定调用哪个函数。

通常，编译器不对“通过函数指针而进行的调用”实施inline

# 条款31 将文件间的编译依存关系降到最低

- 如果使用前置声明，编译器必须在编译期间知道对象的大小

**这一章仔细看原书！！！！！！**

- 支持编译依存性最小化的一般构想是：依赖于声明式、不要依赖于定义式。基于此构想的两个手段是Handle classes和Interface classes。
- 程序库头文件应该以“完全且仅有声明式”的形式存在。

# 条款32 确定你的public继承塑模出is-a关系

- public继承意味着"is-a"的关系

需要base class对象的地方，也可以给出derived class对象替代

# 条款33 避免遮掩继承而来的名称

derived class的作用域嵌套于base class作用域之内

- derived classes内的名称会遮掩base classes内的名称。

- 如果想要推翻 遮掩继承而来的名称，可以使用using声明式。在derived class中使用using来声明base class中的名称，使它可见！

# 条款34 区分接口继承和实现继承

- public继承分为：函数接口继承和函数实现
- 声明一个pure virtual函数的目的是为了让derived classes只继承函数接口
- 声明impure virtual函数的目的，是让derived classes继承该函数的接口和缺省实现

---

父类的虚函数如果有定义，子类可以不重写；没有定义则要重写。

父类的纯虚函数，子类必须重写，否则会让子类也成为抽象类

- 声明一个non-virtual函数的目的是为了令derived classes继承函数的接口及一份强制性实现

| pure virtual函数 | impure virtual函数     | non_virtual函数        |
| ---------------- | ---------------------- | ---------------------- |
| 只继承接口       | 继承接口和一份缺省实现 | 继承接口和一份强制实现 |



# 条款35 考虑virtual函数以外的其他选择

- 藉由Non-Virtual Interface手法实现Template Method模式

这一设计模式思路，令客户通过public non-virtual成员函数剑姬调用private virtuial函数

我们将这个non-virtual函数称为virtual函数的外覆器(wrapper)

---

std::function对象可以保存任何可调用物，函数指针、函数对象或成员函数指针，只要其签名式兼容于需求端。

此外，这个可调用物的参数可以被隐式转换成function中定义的。

![image-20220228103021416](dependence/image-20220228103021416.png)

# 条款36 绝不重新定义继承而来的non-virtual函数

- non-virtual函数都是静态绑定
- virtual函数是动态绑定

绝不要重新定义继承而来的non-virtual函数



# 条款37 绝不重新定义继承而来的缺省参数值

- virtual函数是动态绑定，而缺省参数值却是**静态绑定**

![image-20220228103516845](dependence/image-20220228103516845.png)![image-20220228103523694](dependence/image-20220228103523694.png)

C++这样运作的原因：运行期效率。

如果缺省参数值是动态绑定，编译器就必须有某种办法在运行期为virtual函数决定适当的参数缺省值。这比在编译器决定的机制更慢且更复杂。

---

一种解决方案是**NVI手法**：令base class没得一个public non-virtual函数调用private virtual函数，后者可以被derived classes重新定义。而且可以在这里**让non-virtual函数指定缺省参数**，而private virtual函数负责真正的工作。



# 条款38 通过复合塑模出has-a或“根据某物实现出”

- 在应用域，复合意味着has-a。在实现域。复合意味着is-implemented-in-terms-of(根据某物实现出)



# 条款39 明智而审慎地使用private继承

- 如果classes之间的继承关系是private，编译器不会自动将一个derived class对象转换为一个base class对象。
- 由private继承而来的所有成员，在derived class中都会变成private属性

**private继承意味着只有实现部分被继承。接口部分应略去**。说明derived class根据base class实现而得。

---

面对**大小为零的独立(非附属)对象**，C++官方勒令默默安排一个char到空对象中。

---

- private继承意味着**根据某物实现出**。它通常比复合的级别低。
- private继承可以造成empty base最优化，可以做到**“对象尺寸最小化”**



# 条款40 明智而审慎地使用多重继承

- C++解析重载函数调用的规则：在看到是否有个函数可取用之前，C++首先确认这个函数对此调用是最佳匹配。找到最佳匹配后才检验其可用性。

**避免菱形继承带来的两份间接基类的数据**，可以令中间的两个直接基类称为virtual base class

![image-20220301082453388](dependence/image-20220301082453388.png)

**IOFile直接继承InputFile和OutputFile在File中的共同成员，再继承他俩各自的新增成员**

virtual继承的那些classes所产生的对象会比non-virtual大；访问virtual base classes的成员变量时，也比访问non-virtual的慢

- 如果virtual继承不带有任何数据，那么将是最具使用价值的情况。



# 条款41 了解隐式接口和编译期多态

![image-20220301084206383](dependence/image-20220301084206383.png)

此处w要满足：支持size()  normalize() swap()和拷贝等等隐式接口；

凡涉及w的任何函数调用，都可能造成template具现化。该具现行为发生在编译期，**编译期多态**。

---

**隐式接口**：w不需要支持operator!=，因为可以：operator!=接受一个类型为X的对象和一个类型为Y的对象，w可以转换成X，someNastyWidget可以转换为Y。

- 只要表达式有效，template能够在编译期成功具现化，那么就实现了编译期多态。



# 条款42 了解typename的双重含义

- template内出现的名称如果依赖于某个template参数，称之为**从属名称**
- 如果从属名称在class内呈嵌套状，那么称为**嵌套从属名称**

**如果解析器在template中遇到嵌套从属名称，它便假设这个名称不是一个类型，除非手动告知它**  所以需要typename关键字对其后的名称进行声明  声明它是一个类型

```cpp
template <typename C>
void f(const C& container,	//不允许使用typename
      typename C::iterator iter);	//必须使用typename
```

C并不是嵌套从属名称(它并非嵌套与任何"取决于template参数"的东西内)；但是C::iterator是个嵌套从属类型名称，所以必须要typename前导。

- 不可以在base class lists或者member initialization list的地方使用typename修饰



# 条款43 学习处理模板化基类内的名称                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    

template继承的类可能无法调用基类的函数(拒绝调用)，因为他知道base class templates可能被特化，而那个特化版本可能不提供和一般template相同的接口。因此往往拒绝在模板化基类内寻找继承而来的名称。

有三种解决方法：

- 在base class函数调用钱加上"this->"
- 使用using声明式；此处是解决编译器不进入base class作用域内查找的问题，而不是继承导致的名称遮盖
- 明确调用baseclass<T>::function(arg...)；但是如果函数是virtual的，会导致关闭"virtual绑定行为"



# 条款44 将与参数无关的代码抽离templates

![image-20220301095148611](dependence/image-20220301095148611.png)

![image-20220301095201527](dependence/image-20220301095201527.png)



# 条款45 运用成员函数模板接受所有兼容类型

- 真实指针(row pointer)支持隐式转换：derived class指针可以转换成base class指针；指向non-const对象的指针可以转换成指向const对象的指针

但是对于我们自定义的智能指针却办不到这一点。

如果针对每一个需要隐式类型转换处都对构造函数进行修改、增补，那么将无穷无尽，因为一个template可以被**无限量具现化，以致生成无限量函数**。

所以我们需要**为这个类写一个构造模板(member function templates)**

![image-20220302085953904](dependence/image-20220302085953904.png)

:point_up_2:对任何类型T和任何类型U，这里可以根据SmartPtr<U>生成一个SmartPtr<T>——因为SmartPtr<T>的构造函数接受一个SmartPtr<U>参数。有时称为**泛化copy构造函数**。

- 声明泛化copy和assignment构造函数之后，还是需要声明正常的copy构造和copy assignment操作符



# 条款46 需要类型转换时请为模板定义非成员函数

在一个class template中，template名称可被用来作为“template和其参数”的简略表达方式，所以在Rational<T>内可以只写Rational。

---

- template实参推导过程中从不将隐式类型转换函数纳入考虑

所以当辨析一个class template，它提供的"与此template相关的"函数支持"所有参数的隐式类型转换"时，将哪些函数定义为**class template内部的friend函数**。而且只能在内部定义，而不能在外部定义！

e.g.

```cpp
template<typename T>
class Rational{
public:
    ...
    friend 
    const Rational operator*(const Rational& lhs,
                          	 const Rational& rhs);
};
template<typename T>
const Rational<T>operator*(const Rational<T>& lhs,
                           const Rational<T>& rhs)
```

该代码可以通过编译，而且支持混合式调用，因为当对象被声明为一个Ration<int>时，class Rational<int>便被具现化出来，而作为过程的一部分，friend函数也被声明出来，此时它便是一个函数，而非函数模板。因此编译器在调用的时候支持隐式类型转换。

但是**无法通过连接**，因为编译器知道我们调用"接受两个Rational<int>参数"的函数，但是它只声明在Rational内，并没有被定义出来。**因为类内声明的那个函数在类具现化的时候就因为模板参数被推导出来而具现化了**，而类外定义的那个在类具现化的时候不为所动，所以毫无关联。无法连接！！！！

**所以将friend函数定义于类内**



# 条款47 请使用traits classes表现类型信息

- advance()用于将迭代器移动给定距离

```cpp
template<typename IterT,typename DistT>
void advance(IterT& iter, DistT d);
```

只有支持random access(随机访问)的迭代器才支持+=操作。对于其他迭代器，advance需要反复执行++或--多次。

---

迭代器有以下几种类型：

![image-20220302113503089](dependence/image-20220302113503089.png)

如果要针对不同类型的迭代器，实现不同的advance操作，就需要**判断迭代器分类**。

traits：允许在编译期取得类型信息；他不是一个C++构件，而是一种技术。

![image-20220302113852581](dependence/image-20220302113852581.png)

iterator_traits的运作方式：针对每一个类型的IterT，在struct iterator_traits<IterT>内声明某个typedef名为iterator_category，用来确定IterT的迭代器分类。

e.g.

```cpp
template<...>
class deque{
public:
    class iterator{
    public:
        typedef random_access_iterator_tag iterator_category;
        ...
    };
    ...
};
```

```cpp
template<...>
class list{
public:
    class iterator{
    public:
        typedef bidirectional_iterator_tag iterator_category;
        ...
    };
    ...
};
```

至于iterator_traits：

```cpp
template<typename IterT>
struct iterator_traits{
  	typedef typename IterT::iterator_category iterator_category;
    ...
};
```



这对用户自定义类型行得通，但是对指针行不通，因为指针不能嵌套typedef。为了支持指针迭代器，需要提供一个偏特化版本：

```cpp
template<typename IterT>
struct iterator_traits<IterT*>{
  	typedef random_access_iterator_tag iterator_category;
    ...
};
```

---

然后需要重载针对每个迭代器类型的操作

![image-20220302114723731](dependence/image-20220302114723731.png)

然后在advance内调用上述函数即可

- 通过重载技术可以在编译期对类型执行操作，而不在运行期；避免了可执行文件的膨胀和执行效率低下



# 条款48 认识模板元编程

没讲啥...



# 条款49 了解new-handler的行为

- 当operator new抛出异常反映一个未获满足的内存需求之前，他会调用客户指定的错误处理函数(new-handler)。用户必须调用set_new_handler(声明于<new>)

![image-20220302145104236](dependence/image-20220302145104236.png)

该处理函数的参数是个指针，指向operator new无法分配足够内存时该被调用的函数

```cpp
void outOfMem(){
    std::cerr<<"Unable to satisfy request for memory\n";
    std::abort();
}
int main(){
    std::set_new_handler(outOfMem);
    int *pBigDataArray = new int [10000000L];
}
```

看晕了，缓一缓再说...



# 条款50 了解new和delete的合理替换时机

- C++要求所有的operator new返回的指针都有适当的对齐。

令operator new返回一个得自malloc的指针是安全的。



# 条款51 编写new和delete时需固守常规

operator new的返回值非常pure，如果能提供客户申请的内存，就返回一个指针指向那块内存；如果不能，就调用处理函数并抛出bad_alloc异常。

- operator new实际上不止一次尝试分配内存，并在每次失败后调用处理函数

如果base classes遗漏virtual析构函数，operator delete可能无法正确运行，**因为传给operator delete 的size_t可能不正确**。



# 条款52 写了placement new 也要写placement delete

```cpp
void* operator new(std::size_t, void* pMemory) throw();
```

这个new的用途质疑是在vector的未使用空间上创建对象

---

编译器在调用new之后会调用operator new和构造函数，如果operator new没有抛出异常，而在构造函数抛出异常，那么就要取消operator new的分配并调用与之对应的operator delete清理内存。**运行期系统寻找参数个数和类型都与operator new相同的operator delete“

![image-20220302155030962](dependence/image-20220302155030962.png)

---

![image-20220302155326643](dependence/image-20220302155326643.png)![image-20220302155335520](dependence/image-20220302155335520.png)

---

默认情况下C++在global作用域内提供以下形式的operator new：

![image-20220302155502173](dependence/image-20220302155502173.png)

编写class的时候可以让成员函数调用他们。









































