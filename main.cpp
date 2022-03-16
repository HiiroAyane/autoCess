#include "Control.h"


int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("-d:deploy cess by params.txt\nparams.txt格式:\n矿工号\n第二账户私钥\n本机公网IP\n挖矿硬盘挂载路径\n挖矿硬盘容量（TB）\n");
        printf("如：\n");
        printf("C419\n0000000000000000000000000000000000000000000000000000000000000000\n113.207.1.1\n/lotus_cache\n10\n");
    }
    else if (argc == 2) {
        string strArgs = argv[1];
        if (strArgs == "-d") {
            deployCess();
        }
    }
    return 0;
}