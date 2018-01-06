#include <stdio.h>
#include "stdlib.h"
#include "math.h"
#include <iostream>

#define PI 3.14159//圆周率宏定义
#define LENGTH_NAME_BMP 30//bmp图片文件名的最大长度

using namespace std;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

//位图文件头定义;
//其中不包含文件类型信息（由于结构体的内存结构决定，
//要是加了的话将不能正确读取文件信息）
typedef struct  tagBITMAPFILEHEADER{
    //WORD bfType;//文件类型，必须是0x424D，即字符“BM”
    DWORD bfSize;//文件大小
    WORD bfReserved1;//保留字
    WORD bfReserved2;//保留字
    DWORD bfOffBits;//从文件头到实际位图数据的偏移字节数
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    DWORD biSize;//信息头大小
    LONG biWidth;//图像宽度
    LONG biHeight;//图像高度
    WORD biPlanes;//位平面数，必须为1
    WORD biBitCount;//每像素位数
    DWORD  biCompression; //压缩类型
    DWORD  biSizeImage; //压缩图像大小字节数
    LONG  biXPelsPerMeter; //水平分辨率
    LONG  biYPelsPerMeter; //垂直分辨率
    DWORD  biClrUsed; //位图实际用到的色彩数
    DWORD  biClrImportant; //本位图中重要的色彩数
}BITMAPINFOHEADER; //位图信息头定义

typedef struct tagRGBQUAD{
    BYTE rgbBlue; //该颜色的蓝色分量
    BYTE rgbGreen; //该颜色的绿色分量
    BYTE rgbRed; //该颜色的红色分量
    BYTE rgbReserved; //保留值
}RGBQUAD;//调色板定义

//像素信息
typedef struct tagIMAGEDATA
{
    BYTE blue;
    //BYTE green;
    //BYTE red;
}IMAGEDATA;

//变量定义
BITMAPFILEHEADER strHead;
RGBQUAD strPla[256];//256色调色板
BITMAPINFOHEADER strInfo;

//显示位图文件头信息
void showBmpHead(BITMAPFILEHEADER pBmpHead){
    cout<<"位图文件头:"<<endl;
    cout<<"文件大小:"<<pBmpHead.bfSize<<endl;
    cout<<"保留字_1:"<<pBmpHead.bfReserved1<<endl;
    cout<<"保留字_2:"<<pBmpHead.bfReserved2<<endl;
    cout<<"实际位图数据的偏移字节数:"<<pBmpHead.bfOffBits<<endl<<endl;
}

void showBmpInforHead(tagBITMAPINFOHEADER pBmpInforHead){
    cout<<"位图信息头:"<<endl;
    cout<<"结构体的长度:"<<pBmpInforHead.biSize<<endl;
    cout<<"位图宽:"<<pBmpInforHead.biWidth<<endl;
    cout<<"位图高:"<<pBmpInforHead.biHeight<<endl;
    cout<<"biPlanes平面数:"<<pBmpInforHead.biPlanes<<endl;
    cout<<"biBitCount采用颜色位数:"<<pBmpInforHead.biBitCount<<endl;
    cout<<"压缩方式:"<<pBmpInforHead.biCompression<<endl;
    cout<<"biSizeImage实际位图数据占用的字节数:"<<pBmpInforHead.biSizeImage<<endl;
    cout<<"X方向分辨率:"<<pBmpInforHead.biXPelsPerMeter<<endl;
    cout<<"Y方向分辨率:"<<pBmpInforHead.biYPelsPerMeter<<endl;
    cout<<"使用的颜色数:"<<pBmpInforHead.biClrUsed<<endl;
    cout<<"重要颜色数:"<<pBmpInforHead.biClrImportant<<endl;
}


int main(){
	freopen("in.txt", "r", stdin);

    char strFile[LENGTH_NAME_BMP];//bmp文件名
    IMAGEDATA *imagedata = NULL;//动态分配存储原图片的像素信息的二维数组
    IMAGEDATA *imagedataRot = NULL;//动态分配存储旋转后的图片的像素信息的二维数组
    int width,height;//图片的宽度和高度
    cout<<"请输入所要读取的文件名:"<<endl;
    cin>>strFile;
    FILE *fpi,*fpw;
    fpi=fopen(strFile,"rb");
    if(fpi != NULL){
        //先读取文件类型
        WORD bfType;
        fread(&bfType,1,sizeof(WORD),fpi);
        if(0x4d42!=bfType)
        {
            cout<<"the file is not a bmp file!"<<endl;
            return NULL;
        }
        //读取bmp文件的文件头和信息头
        fread(&strHead,1,sizeof(tagBITMAPFILEHEADER),fpi);
        //showBmpHead(strHead);//显示文件头
        fread(&strInfo,1,sizeof(tagBITMAPINFOHEADER),fpi);
        //showBmpInforHead(strInfo);//显示文件信息头

        //读取调色板
        for(unsigned int nCounti=0;nCounti<strInfo.biClrUsed;nCounti++)
        {
            fread((char *)&(strPla[nCounti].rgbBlue),1,sizeof(BYTE),fpi);
            fread((char *)&(strPla[nCounti].rgbGreen),1,sizeof(BYTE),fpi);
            fread((char *)&(strPla[nCounti].rgbRed),1,sizeof(BYTE),fpi);
            fread((char *)&(strPla[nCounti].rgbReserved),1,sizeof(BYTE),fpi);
        }

        width = strInfo.biWidth;
        height = strInfo.biHeight;
        //图像每一行的字节数必须是4的整数倍
        width = (width * sizeof(IMAGEDATA) + 3) / 4 * 4;
        //imagedata = (IMAGEDATA*)malloc(width * height * sizeof(IMAGEDATA));
        imagedata = (IMAGEDATA*)malloc(width * height);
        imagedataRot = (IMAGEDATA*)malloc(2 * width * 2 * height * sizeof(IMAGEDATA));
        //初始化原始图片的像素数组
        for(int i = 0;i < height;++i)
        {
            for(int j = 0;j < width;++j)
            {
                (*(imagedata + i * width + j)).blue = 0;
                //(*(imagedata + i * width + j)).green = 0;
                //(*(imagedata + i *  width + j)).red = 0;
            }
        }
        //初始化旋转后图片的像素数组
        for(int i = 0;i < 2 * height;++i)
        {
            for(int j = 0;j < 2 * width;++j)
            {
                (*(imagedataRot + i * 2 * width + j)).blue = 0;
                //(*(imagedataRot + i * 2 * width + j)).green = 0;
                //(*(imagedataRot + i * 2 * width + j)).red = 0;
            }
        }
        //fseek(fpi,54,SEEK_SET);
        //读出图片的像素数据
        fread(imagedata,sizeof(struct tagIMAGEDATA) * width,height,fpi);
        fclose(fpi);
    }
    else
    {
        cout<<"file open error!"<<endl;
        return NULL;
    }

    //图片旋转处理
    int RotateAngle;//要旋转的角度数
    double angle;//要旋转的弧度数
    int midX_pre,midY_pre,midX_aft,midY_aft;//旋转所围绕的中心点的坐标
    midX_pre = width / 2;
    midY_pre = height / 2;
    midX_aft = width;
    midY_aft = height;
    int pre_i,pre_j,after_i,after_j;//旋转前后对应的像素点坐标
    cout<<"输入要旋转的角度（0度到360度，逆时针旋转）："<<endl;
    cin>>RotateAngle;
    angle = 1.0 * RotateAngle * PI / 180;
    for(int i = 0;i < 2 * height;++i)
    {
        for(int j = 0;j < 2 * width;++j)
        {
            after_i = i - midX_aft;//坐标变换
            after_j = j - midY_aft;
            pre_i = (int)(cos((double)angle) * after_i - sin((double)angle) * after_j) + midX_pre;
            pre_j = (int)(sin((double)angle) * after_i + cos((double)angle) * after_j) + midY_pre;
            if(pre_i >= 0 && pre_i < height && pre_j >= 0 && pre_j < width)//在原图范围内
                *(imagedataRot + i * 2 * width + j) = *(imagedata + pre_i * width + pre_j);
        }
    }

    //保存bmp图片
    if((fpw=fopen("b.bmp","wb"))==NULL)
    {
        cout<<"create the bmp file error!"<<endl;
        return NULL;
    }
    WORD bfType_w=0x4d42;
    fwrite(&bfType_w,1,sizeof(WORD),fpw);
    //fpw +=2;
    fwrite(&strHead,1,sizeof(tagBITMAPFILEHEADER),fpw);
    strInfo.biWidth = 2 * width;
    strInfo.biHeight = 2 * height;
    fwrite(&strInfo,1,sizeof(tagBITMAPINFOHEADER),fpw);
    //保存调色板数据
    for(unsigned int nCounti=0;nCounti<strInfo.biClrUsed;nCounti++)
    {
        fwrite(&strPla[nCounti].rgbBlue,1,sizeof(BYTE),fpw);
        fwrite(&strPla[nCounti].rgbGreen,1,sizeof(BYTE),fpw);
        fwrite(&strPla[nCounti].rgbRed,1,sizeof(BYTE),fpw);
        fwrite(&strPla[nCounti].rgbReserved,1,sizeof(BYTE),fpw);
    }
    //保存像素数据
    for(int i =0;i < 2 * height;++i)
    {
        for(int j = 0;j < 2 * width;++j)
        {
            fwrite( &((*(imagedataRot + i * 2 * width + j)).blue),1,sizeof(BYTE),fpw);
            //fwrite( &((*(imagedataRot + i * 2 * width + j)).green),1,sizeof(BYTE),fpw);
            //fwrite( &((*(imagedataRot + i * 2 * width + j)).red),1,sizeof(BYTE),fpw);
        }
    }
    fclose(fpw);

    //释放内存
    delete[] imagedata;
    delete[] imagedataRot;
}
