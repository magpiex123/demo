// Clientdemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。


#include <iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib");

using namespace std;

const char DEFAULT_P0RT[] = "5000";
const int SEND_BUF_SIZE = 256;

//Client端,主要是socket和connect
int main()
{
    WSADATA wsadata; 
    int i_result;   //接受winsocket的返回值
    SOCKET soc_clt = INVALID_SOCKET;
    addrinfo* result = nullptr;
    addrinfo hints;
    //获取客户端winsocket运行的状态
    i_result = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (i_result != 0) {
        cout << "socket启动失败" << endl;
        system("pause");
        exit(1);
    }
    //用0填充一块内存
    SecureZeroMemory(&hints, sizeof hints);
    //初始化协议族，协议类型和套接字类型
    hints.ai_family = AF_UNSPEC;  //任意协议族
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;  //于服务器端保持一致

    //从服务器主机上获得addr 的相关信息
    i_result = getaddrinfo("127.0.0.1", DEFAULT_P0RT, &hints, &result);
    if (i_result != 0) {
        cout << "getaddrinfo启动失败，错误信息：" << WSAGetLastError() << endl;
        WSACleanup(); // 释放空间，防止野指针
        system("pause");
        exit(1);
    }
    //创建套接字
    soc_clt = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (soc_clt == INVALID_SOCKET) {
        cout << "创建套接字失败，错误信息：" << WSAGetLastError() << endl;
        WSACleanup();
        system("pause");
        exit(1);
    }
    //创建成功，则下一步就是链接到服务器端
    i_result = connect(soc_clt, result->ai_addr, result->ai_addrlen);
    if (i_result ==SOCKET_ERROR) {
        cout << "链接失败，错误信息：" << WSAGetLastError() << endl;
        WSACleanup(); // 释放空间，防止野指针
        system("pause");
        exit(1);
    }
    cout << "链接成功" << endl;

    //成功后就不需要服务器的地址信息了，因此释放
    freeaddrinfo(result);
    
    //发送数据或者接受数据
    char send_buf[SEND_BUF_SIZE];
    int recv_res = 0;

    do {
        cout << "请输入要发送的信息：" << flush; 
        cin.getline(send_buf, SEND_BUF_SIZE);
        //发送数据
        i_result = send(soc_clt, send_buf, strlen(send_buf), 0); //发送给服务器
        if (i_result == SOCKET_ERROR) {
            cout << "send失败，错误代码为：" << WSAGetLastError() << endl;
            closesocket(soc_clt);
            WSACleanup();
            system("pause");
            return 1;
        }
        else cout << "发送的字节数：" << i_result << endl;
        //发送后，也可以接受来自服务器的信息
        recv_res = recv(soc_clt, send_buf, SEND_BUF_SIZE, 0);
        if (recv_res > 0) {
            cout << "服务器的反馈：" << send_buf << endl;
        }
        else if (recv_res == 0) {
            cout << "关闭链接" << endl;
        }
        else {
            cout << "无法从服务器获取反馈，错误代码为：" << WSAGetLastError() << endl;
            closesocket(soc_clt);
            WSACleanup();
            system("pause");
            return 1;
        }
    } while (recv_res>0);

    
    //最后，关闭套接字
    i_result = shutdown(soc_clt, SD_SEND); //禁止在链路上进行数据的发送和接受
    if (i_result == SOCKET_ERROR) {
        cout << "禁止失败，错误代码为：" << WSAGetLastError() << endl;
        closesocket(soc_clt);
        WSACleanup();
        system("pause");
        return 1;
    }

    closesocket(soc_clt);
    WSACleanup();
    cout << "关闭成功" << endl;
    system("pause");

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
