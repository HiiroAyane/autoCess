#include "Control.h"

bool exepopen(const string& cmd, string& res)
{
	char buffer[MAX_LINE] = { 0 };

	FILE* p = popen(cmd.c_str(), "r");
	if (!p) return false;

	while (fgets(buffer, sizeof(buffer), p)) {
		res += buffer;
	}

	if (pclose(p) == -1) { res.clear(); return false; }
	if (res.size()) {
		while (res[res.size() - 1] == '\n')
			res = res.substr(0, res.size() - 1);
	}
	return true;
}

#include <fstream>
#include <iostream>
bool getParams(string& minerid, string& private_key, string& ip, string& disk, string& space)
{
	ifstream fin("/usr/cess-proof-parameters/params.txt");
	if (!fin.is_open()) {
		cout << "缺少params.txt文件！\n";
		return false;
	}

	string strTmp;
	while (getline(fin, strTmp)) {
		if (strTmp.empty()) break;
		while (strTmp[strTmp.size() - 1] == ('\n' || '\r')) {
			strTmp = strTmp.substr(0, strTmp.size() - 1);
		}
		if (strTmp.empty()) break;

		if (minerid.empty()) {
			minerid = strTmp;
		}
		else if (private_key.empty()) {
			private_key = strTmp;
		}
		else if (ip.empty()) {
			ip = strTmp;
		}
		else if (disk.empty()){
			disk = strTmp;
		}
		else {
			space = strTmp;
			break;
		}
	}

	fin.close();
	return true;
}

#include <unistd.h>
bool makeEthAccount(const string& private_key)
{
	ofstream fout("/usr/cess-proof-parameters/priv.txt", ofstream::trunc);
	if (!fout.is_open()) {
		cout << "写入私钥文件失败！\n";
		return false;
	}

	fout << private_key;
	fout.close();

	string strCmd = "cd /usr/cess-proof-parameters && ./geth account import /usr/cess-proof-parameters/priv.txt";
	FILE* p = popen(strCmd.data(), "w");

	if (!p)return false;
	sleep(1);
	fputs("14Keaideni\n", p);
	sleep(1);
	fputs("14Keaideni\n", p);
	sleep(2);
	pclose(p);
	return true;
}

bool makeShellCmd(const string& minerid, const string& keystore, const string& ip, const string& disk, const string& spaceTb)
{
	ofstream fout("/usr/cess-proof-parameters/startcess.sh", ofstream::trunc);
	if (!fout.is_open()) {
		cout << "写入命令失败！\n";
		return false;
	}

	string strCmd = "./cessnode -k /root/.ethereum/keystore/mykey -p 14Keaideni -m ";
	strCmd += disk;
	strCmd += " -n ";
	strCmd += minerid;
	strCmd += " -s ";
	strCmd += spaceTb;
	strCmd += " -i " + ip + ":15001";

	fout << strCmd;
	fout.close();

	strCmd = "cd /usr/cess-proof-parameters && chmod a+x startcess.sh";
	string strRes;
	exepopen(strCmd, strRes);

	return true;
}

bool deployCess()
{
	string strCmd = "mkdir -p /usr/cess-proof-parameters && ufw disable";
	string strRes;
	exepopen(strCmd, strRes);

	cout << "开始更新apt...\n";
	strCmd = "apt update -y > /usr/cess-proof-parameters/1.log 2>&1 && apt upgrade -y > /usr/cess-proof-parameters/1.log 2>&1";
	exepopen(strCmd, strRes);
	cout << "更新apt成功！\n";

	cout << "开始安装依赖...\n";
	strCmd = "apt install -y util-linux ocl-icd-* wget curl screen unzip > /usr/cess-proof-parameters/1.log 2>&1";
	exepopen(strCmd, strRes);
	cout << "安装依赖成功！\n";

	cout << "开始下载必要文件...\n";
	strCmd = "cd /usr/cess-proof-parameters && wget https://gethstore.blob.core.windows.net/builds/geth-linux-amd64-1.10.16-20356e57.tar.gz -O geth.tar.gz && tar -zxvf geth.tar.gz && mv geth-linux-amd64-1.10.16-20356e57/geth .";
	exepopen(strCmd, strRes);
	strCmd = "cd /usr/cess-proof-parameters && wget https://d2gxbb5i8u5h7r.cloudfront.net/node_v0.4.3 -O cessnode";
	exepopen(strCmd, strRes);
	strCmd = "cd /usr/cess-proof-parameters && wget -nc https://d2gxbb5i8u5h7r.cloudfront.net/parameterfile.zip";
	exepopen(strCmd, strRes);
	strCmd = "cd /usr/cess-proof-parameters && unzip parameterfile.zip && mv parameterfile/v28-* .";
	exepopen(strCmd, strRes);
	strCmd = "cd /usr/cess-proof-parameters && chmod 777 -R .";
	exepopen(strCmd, strRes);
	cout << "必要文件下载成功！\n";

	cout << "开始获取Params.txt内参数\n";
	string strMiner, strPriv, strIp, strDisk, strSpace;
	if (!getParams(strMiner, strPriv, strIp, strDisk, strSpace))
		return false;

	if (!makeEthAccount(strPriv))
		return false;

	strCmd = "ls /root/.ethereum/keystore/";
	exepopen(strCmd, strRes);
	string strKey = strRes;

	strCmd = "mv /root/.ethereum/keystore/" + strKey + " /root/.ethereum/keystore/mykey";
	exepopen(strCmd, strRes);

	if (!makeShellCmd(strMiner, strKey, strIp, strDisk, strSpace)) {
		return false;
	}

	strCmd = "screen -dmS cess";
	exepopen(strCmd, strRes);
	strCmd = "screen -x -S cess -p 0 -X stuff \"cd /usr/cess-proof-parameters\"";
	exepopen(strCmd, strRes);
	strCmd = "screen -x -S cess -p 0 -X stuff \"./startcess.sh\"";
	exepopen(strCmd, strRes);
	strCmd = "screen -x -S cess -p 0 -X stuff '\n'";
	exepopen(strCmd, strRes);
	
	return true;
}
