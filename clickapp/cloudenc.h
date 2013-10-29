#ifndef _cloudenc_h
#define _cloudenc_h

#ifdef  __cplusplus
extern "C" {
#endif

//base64编码
extern int baseenc(char *from,char *to,int len);
extern int basedec(char *from,char *to,int len);

//rsa加解密
extern int rsaenc(char *from,char *to,char *pubkeyfile);
extern int rsadec(char *from,char *to,char *prikeyfile);

//des加解密
extern int desenc(char *from,char *to,char *ukey);
extern int desdec(char *from,char *to,char *ukey);

#ifdef __cplusplus
}
#endif

#endif // CSGENERIC_H_INCLUDED
