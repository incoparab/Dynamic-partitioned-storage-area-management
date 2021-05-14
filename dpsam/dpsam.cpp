//Dynamic partitioned storage area management 动态分区式存贮区管理
#include <iostream>
#include <String>
#define maxsize 512 - 1
using namespace std;
struct rd //分区描述器
{ 
    string name;
    bool flag;                        //占用标识
    int size;                         //占用空间
    rd *next;                         //下块分区
    rd() {}                           //默认构造函数
    rd(int w_size) { size = w_size; } //构造函数
    rd(string w_name, bool w_flag, int w_size, rd *w_next)
    { //构造函数
        name = w_name;
        flag = w_flag;
        size = w_size;
        next = w_next;
    }
    rd(bool w_flag, int w_size, rd *w_next)
    { //构造函数
        name = "free";
        flag = w_flag;
        size = w_size;
        next = w_next;
    }
};
struct PCB //进程控制器
{
    string name;
    int begin; //起始地址
    int size;  //占用空间
    PCB *next; //下个进程
    PCB() {}   //默认构造函数
    PCB(string w_name, int w_begin, int w_size, PCB *w_next)
    { //构造函数
        name = w_name;
        begin = w_begin;
        size = w_size;
        next = w_next;
    }
};

class manage
{
public:
    rd *r_first;  //分区描述器头指针
    PCB *P_first; //进程分配器头指针

public:
    manage()
    {
        rd *r_new = new rd(0, maxsize - 1, NULL);
        r_first = new rd("rd", 1, 1, r_new);       //头指针即分区描述器
        P_first = new PCB("rd", maxsize, 1, NULL); //创建分区描述器的空间
        Menu();
    }
    void Menu();       //菜单
    int Menu_1();      //菜单选项
    void first_time(); //首次
    void best();       //最佳
    void worst();      //最坏
    void free();       //内存回收
    void Show();       //显示结果
};
void manage::Show()
{
    int s = 0;
    int addr = maxsize;
    rd *p = r_first;
    while (p)
    {
        s++;
        cout << "partition" << s << ":" << ' ' << p->name << ": " << addr << "~" << addr - p->size << endl;
        addr -= p->size;
        p = p->next;
    }
}

int manage::Menu_1()
{
    cout << "[1] First time" << endl
         << "[2] Best" << endl
         << "[3] Worse" << endl
         << "[4] Free" << endl
         << "[0] exit" << endl;
    cout << "Please choose:";
    int ch;
    cin >> ch;
    return ch;
}

void manage::Menu()
{
    cout << "------Dynamic partitioned storage area management------" << endl;
    int ch;
    while (ch = Menu_1())
    {
        switch (ch)
        {
        case 1:
            first_time();
            Show();
            break;
        case 2:
            best();
            Show();
            break;
        case 3:
            worst();
            Show();
            break;
        case 4:
            free();
            Show();
            break;
        default:
            Show();
            cout << "exit" << endl;
            return;
        }
    }
}
void manage::first_time()
{
    cout << "Please input Process and it's Size:" << endl;
    string name;
    int size;
    cin >> name >> size; //输入进程名称和内存大小
    int addr = maxsize;  //从高地址到低地址,指向上边界
    rd *p = r_first;
    while (p != NULL)
    {
        if (p->flag == 0) //当前分区为空
        {
            if (p->size > size) //当前分区大小大于需要的内存，分配
            {
                p->flag = 1;
                p->name = name;
                rd *r_new = new rd(0, p->size - size, p->next); //分区剩余的空间
                p->size = size;                                 //*p分区分配
                p->next = r_new;
                break;
            }
            else if (p->size == size)
            {
                p->flag = 1;
                p->name = name;
                break;
            }
            else
            {
                addr -= p->size;
                p = p->next;
            }
        }
        else
        {
            addr -= p->size;
            p = p->next;
        }
    }
    if (p == NULL)
        cout << "Distribution error!" << endl; //分配错误提示
    else
    {
        PCB *q = P_first;
        PCB *P_new = new PCB(name, addr, size, NULL);
        while (q->next != NULL)
            q = q->next;
        q->next = P_new;
    }
}
void manage::best()
{
    cout << "Please input Process and it's Size:" << endl;
    string name;
    int size;
    cin >> name >> size;
    int addr = maxsize, t_addr;
    rd *p = r_first;
    int Error = INT_MAX; //记录偏差
    rd *temp = new rd;   //记录当前最佳位置
    while (p != NULL)
    {
        if (p->flag == 0) //分区为空
        {
            if (p->size >= size && p->size - size <= Error) //当前更适合分配
            {
                Error = p->size - size; //更新偏差
                temp = p;               //记录当前分区位置
                t_addr = addr;          //当前分区上边界
            }
            addr -= p->size;
            p = p->next;
        }
        else //分区不为空则查找下一个分区
        {
            addr -= p->size;
            p = p->next;
        }
    }
    if (Error == INT_MAX)
        cout << "Distribution error!" << endl;
    else
    {
        //更新分区描述器
        temp->flag = 1;
        temp->name = name;
        if (temp->size != size)
        {
            rd *r_new = new rd(0, temp->size - size, temp->next); //分区剩余的空间
            temp->size = size;                                    //*temp分区分配
            temp->next = r_new;
        }
        //跟新进程控制器
        PCB *q = P_first;
        PCB *P_new = new PCB(name, t_addr, size, NULL);
        while (q->next != NULL)
            q = q->next;
        q->next = P_new;
    }
}
void manage::worst()
{
    cout << "Please input Process and it's Size:" << endl;
    string name;
    int size;
    cin >> name >> size;
    int addr = maxsize, t_addr;
    rd *p = r_first;
    rd *temp = new rd;
    int Error = INT_MIN;
    while (p != NULL)
    {
        if (p->flag == 0)
        {
            if (p->size >= size && p->size > Error)
            {
                temp = p;
                t_addr = addr;
                Error = p->size;
            }
            addr -= p->size;
            p = p->next;
        }
        else
        {
            addr -= p->size;
            p = p->next;
        }
    }
    if (Error == INT_MIN)
        cout << "Distribution error!" << endl;
    else
    {
        //更新分区描述器
        temp->flag = 1;
        temp->name = name;
        if (temp->size != size)
        {
            rd *r_new = new rd(0, temp->size - size, temp->next); //分区剩余的空间
            temp->size = size;                                    //*temp分区分配
            temp->next = r_new;
        }
        //跟新进程控制器
        PCB *q = P_first;
        PCB *P_new = new PCB(name, t_addr, size, NULL);
        while (q->next != NULL)
            q = q->next;
        q->next = P_new;
    }
}

void manage::free()
{
    cout << "Please input the process'name that will be killed:" << endl;
    string name;
    cin >> name;
    PCB *p = P_first;
    while (p->next->next != NULL && p->next->name != name) //p->next为查找进程
        p = p->next;
    if (!p->next)
        cout << "Don't have this process!" << endl;
    else
    {
        PCB *t = p->next;
        int addr = maxsize;
        rd *q = r_first;
        addr -= q->size;
        while (q->next != NULL) //q->next为查找进程空间
        {
            if (t->begin == addr)
                break;
            q = q->next;
            addr -= q->size;
        }
        q->next->flag = 0;
        q->next->name = "free";
        if (q->next->next->flag == 0)
        { //q->next后空间为空
            q->next->size += q->next->next->size;
            q->next->next = q->next->next->next;
        }
        if (q->flag == 0)
        { //q->next前空间为空
            q->size += q->next->size;
            q->next = q->next->next;
        }
        if (t->next)
        {
            p->next = t->next;
            delete t;
        }
        else
            delete t;
    }
}

int main()
{
    manage M;
    system("pause");
    return 0;
}