#pragma once
#include <string>
using namespace std;

constexpr auto MAX_LINE = 1024;
bool exepopen(const string& cmd, string& res);
bool getParams(string& minerid, string& private_key, string& ip, string& disk, string& space);
bool makeEthAccount(const string& private_key);
bool makeShellCmd(const string& minerid, const string& keystore, const string& ip, const string& disk, const string& spaceTb);
bool deployCess();

