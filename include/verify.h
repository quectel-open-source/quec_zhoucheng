#include <cstring>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <stdexcept>
#include <stack>
using namespace std;

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "openssl/err.h"
#include "openssl/pem.h"
#include "openssl/opensslconf.h"
#include "openssl/opensslv.h"

#include <stdio.h>

// intrinsics
#if defined(__GNUC__)    // GCC
#include <cpuid.h>
#elif defined(_MSC_VER)    // MSVC
    #if _MSC_VER >=1400    // VC2005
#include <intrin.h>
    #endif    // #if _MSC_VER >=1400
#else
#error Only supports MSVC or GCC.
#endif    // #if defined(__GNUC__)

void getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue)
{
#if defined(__GNUC__)    // GCC
    __cpuid_count(InfoType, ECXValue, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER)    // MSVC
    #if defined(_WIN64) || _MSC_VER>=1600    // 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
        __cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
    #else
        if (NULL==CPUInfo)    return;
        _asm{
            // load. 读取参数到寄存器.
            mov edi, CPUInfo;    // 准备用edi寻址CPUInfo
            mov eax, InfoType;
            mov ecx, ECXValue;
            // CPUID
            cpuid;
            // save. 将寄存器保存到CPUInfo
            mov    [edi], eax;
            mov    [edi+4], ebx;
            mov    [edi+8], ecx;
            mov    [edi+12], edx;
        }
    #endif
#endif    // #if defined(__GNUC__)
}

/*
@brief : 公钥解密
@para  : cipher_text -[i] 加密的密文
         pub_key     -[i] 公钥
@return: 解密后的数据
**/
std::string RsaPubDecrypt(const std::string & cipher_text, const std::string & pub_key)
{
    std::string decrypt_text;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)pub_key.c_str(), -1);
    RSA *rsa = RSA_new();
   
    // 注意--------使用第1种格式的公钥进行解密
    //rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);
    // 注意--------使用第2种格式的公钥进行解密（我们使用这种格式作为示例）
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    if (!rsa)
    {
        unsigned long err= ERR_get_error(); //获取错误号
        char err_msg[1024] = { 0 };
        ERR_error_string(err, err_msg); // 格式：error:errId:库:函数:原因
        printf("err msg: err:%ld, msg:%s\n", err, err_msg);
        BIO_free_all(keybio);
        return decrypt_text;
    }

    int len = RSA_size(rsa);
    char *text = new char[len + 1];
    memset(text, 0, len + 1);
    // 对密文进行解密
    int ret = RSA_public_decrypt(cipher_text.length(), (const unsigned char*)cipher_text.c_str(), (unsigned char*)text, rsa, RSA_PKCS1_PADDING);
    if (ret >= 0) {
        decrypt_text.append(std::string(text, ret));
    }

    // 释放内存  
    delete text;
    BIO_free_all(keybio);
    RSA_free(rsa);

    return decrypt_text;
}

// crt
int getCertInfo(string path, string &info, string &out_pri_key)
{
	BIO *pbio = BIO_new_file(path.c_str(),"r");
	X509 *cert = PEM_read_bio_X509(pbio,NULL,NULL,NULL);
    BIO_reset(pbio);
    //cout << "q111" << endl;
    EVP_PKEY *rpkey = X509_get_pubkey(cert);
    //cout << "q222" << rpkey << endl;
    RSA *keypair = EVP_PKEY_get1_RSA(rpkey);
    //cout << "q333" << endl;    
    BIO *pri = BIO_new(BIO_s_mem());
    // 生成私钥
    PEM_write_bio_RSA_PUBKEY(pri, keypair);
    int pri_len = BIO_pending(pri);
    char *pri_key = (char *)malloc(pri_len + 1);
    BIO_read(pri, pri_key, pri_len);
    pri_key[pri_len] = '\0';
    out_pri_key = pri_key;
    //cout << "out_pri_key: " << out_pri_key << endl;

    BIO *x509Bio = BIO_new(BIO_s_mem());
    X509_print(x509Bio, cert);
    int num_write = BIO_pending(x509Bio);
    char* szCertInfo = (char*)malloc(num_write+1);
    memset(szCertInfo, 0, num_write+1);
    BIO_read(x509Bio, szCertInfo, num_write);
    info = szCertInfo;
    //cout << "info: " << info << endl;

    if(pbio)
        BIO_free(pbio);
    if(pri)
        BIO_free(pri);
    if(cert)
        X509_free(cert);
    if(x509Bio)
        BIO_free(x509Bio);
    if(szCertInfo)
        free(szCertInfo);
    if(pri_key)
        free(pri_key);
    return 0;
}

//_compare
int verify(int &nflag, string bootDir)
{
    string user_crt = bootDir + "/server.crt";//用户证书
    string info;
    string out_pri_key;
    getCertInfo(user_crt, info, out_pri_key);
    unsigned char result_hash_crt[33];
    memset(result_hash_crt, 0, 33);
    SHA256( (const unsigned char *)info.c_str(), info.length(), result_hash_crt);   
    //unsigned char crt_file[] = "f1f8e599e6159e278f25e5db55dfcce7b188d19bdb48228326e59691e0900fce00";
    unsigned char crt_file[] = "24124822915323021158391433722921985223204231177136209155219723413138229150145224144152060";
    //unsigned char crt_file[] = "239732221851561567862201552031471861009311719612101232473513522221241119541481812161090";

    char buf[2048];
    char* str_d = buf;
    //输出证书hash
    for (int i = 0; i < 33; i++) {
        sprintf(str_d, "%d", int(result_hash_crt[i]));
        for(;*str_d;) str_d++;
    }
    std::string str_crt_hash_org = (char*)crt_file;
    std::string str_crt_hash_input = (char*)buf;    
    // //输出证书hash
    // printf("%s\n", crt_file);
    // for (int i = 0; i < 33; i++) {
    //     printf("%02x", crt_file[i]);
    // }
    // cout << endl;	
    // //输出证书hash
    // for (int i = 0; i < 33; i++) {
    //     printf("%02x", result_hash_crt[i]);
    // }
    // cout << endl;	

    if (strcmp(str_crt_hash_org.c_str(), str_crt_hash_input.c_str()) == 0){
        cout << "证书验签成功" << endl;
    }
    else{
        cout << "证书验签失败" << endl;
        nflag = 1;
    }

    char pub[] = "-----BEGIN PUBLIC KEY-----\nMFowDQYJKoZIhvcNAQEBBQADSQAwRgJBAL49JwqyBrMpwOAj1VekVBUQUZWkFuMe\nmULEOvTVisJrLI6jNuP8ME+anWwJZNM6Yi3L3jGuv2fZpHnT/a/I5ekCAQM=\n-----END PUBLIC KEY-----\n\0";
    std::string pub_key = pub;
    if (strcmp(pub_key.c_str(), out_pri_key.c_str()) == 0){
        cout << "公钥验签成功" << endl;
    }
    else{
        cout << "公钥验签失败" << endl;
        nflag = 1;
    }

    //hash 明文 cpuid
    unsigned int dwBuf[4];
    getcpuidex(dwBuf, 1, 0);
    char szTmp[33] = { NULL };
    sprintf(szTmp, "%08X%08X", dwBuf[3], dwBuf[0]);

    string str=szTmp;
    unsigned char result[33];
    memset(result, 0, 33);
    SHA256( (const unsigned char *)str.c_str(), str.length(),  result);
    std::string src_text = (char*)result;

    unsigned char result_pri2[128];    
    FILE * fp_encoded_bin_pri2 = fopen((bootDir + "/quectel_license.bin").c_str(), "r");
    for (size_t n = 0; n < 128; n++)
    {
        fscanf(fp_encoded_bin_pri2, "%02x", &(result_pri2[n]));
    }    
    fclose(fp_encoded_bin_pri2);  
    std::string encrypt_text = (char*)result_pri2;

    // 公钥解密
    std::string decrypt_text = RsaPubDecrypt(encrypt_text, pub_key);
    //printf("decrypt: len=%d\n", decrypt_text.length());
    //unsigned char *p=(unsigned char *)decrypt_text.c_str();
    // for (int i = 0; i < 33; i++) {
    //     printf("%02x ", p[i]);
    // }
    //cout << endl;	
    if (strcmp(src_text.c_str(), decrypt_text.c_str()) == 0){
        cout << "摘要验签成功" << endl;
    }
    else{
        cout << "摘要验签失败" << endl;
        nflag = 1;
    }
    //printf("decrypt: %s\n", decrypt_text.c_str());
    return 0;
}

void getBinary1(int n) 
{ 
	for (int i=0; i <32; i++) 
	{ 
		int bjudge = (n >> i) & 1;
		if(bjudge == 1) {
			switch (i)
			{
			case 0:
				cout << i << " NG" << endl;
				break;
			case 1:
				cout << i << " OK" << endl;
				break;	
			case 2:
				cout << i << " NULL" << endl;
				break;				
			case 3:
				cout << i << " 不均匀" << endl;
				break;
			case 4:
				cout << i << " 碰伤" << endl;
				break;
			case 5:
				cout << i << " 脏污" << endl;
				break;
			case 6:
				cout << i << " 毛糙" << endl;
				break;
			case 7:
				cout << i << " 无盖" << endl;
				break;				
			case 8:
				cout << i << " 凹坑" << endl;
				break;	
			case 9:
				cout << i << " 划痕" << endl;
				break;								
			default:
				break;
			}
		}
	} 
} 
