#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
using namespace std;

//1 is set as prime
vector<int> prime;

void Eratosthenes(const int &num, vector<int> &prime)
{
    prime.resize(num + 1);
    for (auto &i : prime)
        i = 1;
    prime[0] = 0, prime[1] = 0;
    for (int i = 1; i <= num + 1; ++i)
    {
        if (prime[i] == 0)
            continue;
        //该数是素数，将他的倍数设置为合数
        int composite = i;
        while ((composite += i) <= num)
            prime[composite] = 0;
    }
}

bool judge_exist_root(const int &num)
{
    if (num == 2 || num == 4)
        return true;
    Eratosthenes(num, prime);
    for (int element = 3; element < prime.size(); ++element)
    {
        if (prime[element] == 0)
            continue;
        for (int i = 1;; ++i)
        {
            int target = pow(element, i);
            if (target > num)
                break;
            if (target == num || target * 2 == num)
                return true;
        }
    }
    return false;
}

int Euler(const int &num)
{
    //求欧拉函数
    if (prime[num] == 1)
        return num - 1;
    int result = num, n = num;
    for (int i = 2; i * i <= n; ++i)
    {
        if (n % i == 0)
        {
            //找到一个素因子
            result = result / i * (i - 1);
            while (n % i == 0)
                //将该素因子全部约去
                n /= i;
        }
    }
    if (n > 1)
        //检验有无遗漏
        result = result / n * (n - 1);
    return result;
}

bool coprime(int i, int num)
{
    //欧几里德除法判断互素
    if (i == 1 || num == 1)
        // 两个正整数中，只有其中一个数值为1，两个正整数为互质数
        return true;
    while (1)
    {
        //求出两个数的最大公约数
        int temp = i % num;
        if (temp == 0)
            break;
        else
            i = num, num = temp;
    }
    if (num > 1)
        // 如果最大公约数大于1，表示两个正整数不互质
        return false;
    return true;
}

int quick_pow(const int &a, int b, const int &mod)
{
    //模平方重复法
    int answer = 1, base = a % mod; //answer为模平方重复法中的a，base为b
    while (b != 0)
    {
        //位运算and，计算结果为a的二进制最低一位是否为1,通过右移运算符更新最低位
        if (b & 1)
            answer = (answer * base) % mod;
        base = (base * base) % mod;
        b >>= 1;
    }
    return answer;
}

vector<int> compute_root(const int &num)
{
    vector<int> result;
    int i, j;
    int eur = Euler(num); // 求得欧拉函数
    for (i = 1; i < num; ++i)
    {
        if (!coprime(i, num))
            //确保底数和模互素
            continue;
        for (j = 1; j <= eur; ++j)
            if (quick_pow(i, j, num) == 1)
                break;
        if (j == eur)
            result.push_back(i);
    }
    return result;
}

int main(int argc, char **argv)
{
    int buf;
    fstream test, result;
    int temp = 2, number;
    //输入测试用例的最大值，然后程序会自动生成2~你想要的测试用例之间的所有数值
    cout << "Please input your expected max_number:";
    cin >> number;
    test.open("testcase.txt", ios::out);
    while (temp++ < number)
        test << temp << endl;
    test.close();
    
    test.open("testcase.txt", ios::in);
    result.open("result.txt", ios::out);
    //test.seekg(ios::beg);
    while (test >> buf)
    {
        //每次从文件中读取一行，即一个数字，进行原根存在性判断和原根的求解
        if (judge_exist_root(buf))
        {
            result << buf << " has roots!" << endl
                   << "Here is the root:";
            vector<int> v1 = compute_root(buf);
            for (auto i : v1)
                result << i << ' ';
            result << endl
                   << endl;
        }
        else
            result << buf << " doesn't have roots!" << endl
                   << endl;
    }
    return 0;
}