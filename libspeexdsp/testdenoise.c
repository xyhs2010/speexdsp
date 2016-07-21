#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "speex/speex_preprocess.h"
#include <stdio.h>

#define NN 1024

struct fhead
{
 char a[4];//四个字节存放'R','I','F','F'
 int b;        //整个文件的长度-8;每个Chunk的size字段，都是表示除了本Chunk的ID和SIZE字段外的长度;
 char c[4];//四个字节存放'W','A','V','E'

 char d[4];//四个字节存放'f','m','t',''
 int e;       //16后没有附加消息，18后有附加消息；一般为16，其他格式转来的话为18
 short int f;       //编码方式，一般为0x0001;
 short int g;       //声道数目，1单声道，2双声道;
 int h;        //采样频率;
 int i;        //每秒所需字节数;
 short int j;       //每个采样需要多少字节，若声道是双，则两个一起考虑;
 short int k;       //即量化位数

 char p[4];//四个字节存放'd','a','t','a'
 int q;        //语音数据部分长度，不包括文件头的任何部分
};//定义WAVE文件的文件头结构体

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: testdenoise <inputfile> <outputfile>");
        return 0;
    }
    freopen(argv[1], "rb", stdin);
    freopen(argv[2], "wb", stdout);
   short in[NN];
   int i;
   SpeexPreprocessState *st;
   int count=0;
   float f;

   st = speex_preprocess_state_init(NN, 16000);
   i=1;
   speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DENOISE, &i);
   i=0;
   speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC, &i);
   i=8000;
   speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
   i=0;
   speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB, &i);
   f=.0;
   speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
   f=.0;
   speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);

   fread(in, sizeof(struct fhead), 1, stdin);
   fwrite(in, sizeof(struct fhead), 1, stdout);
   while (1)
   {
      int vad;
      fread(in, sizeof(short), NN, stdin);
      if (feof(stdin))
         break;
      vad = speex_preprocess_run(st, in);
      /*fprintf (stderr, "%d\n", vad);*/
      fwrite(in, sizeof(short), NN, stdout);
      count++;
   }
   speex_preprocess_state_destroy(st);
   return 0;
}
