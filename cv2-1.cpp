#include <iostream>
#include <Windows.h>
#include <malloc.h>
#include <vector>
#include<stdlib.h>
//【注意！】本项目 因为使用了scanf() 需要在项目属性中关闭SDL检查才能正常编译
using namespace std;

string imgPath = "R-C2.bmp";//"test3.bmp";
string saveImgPath = "test3.mod.bmp";




//typedef struct tagBITMAPFILEHEADER {
//    WORD    bfType;
//    DWORD   bfSize;
//    WORD    bfReserved1;
//    WORD    bfReserved2;
//    DWORD   bfOffBits; //偏移量 偏移之后才是记录的索引或者真实数据 调色板数据等于 偏移-头大小（14+40）（注意 每个调色板数据包括四个字节）
//} BITMAPFILEHEADER

//typedef struct tagBITMAPINFOHEADER{
//        DWORD      biSize;
//        LONG       biWidth;
//        LONG       biHeight;
//        WORD       biPlanes;
//        WORD       biBitCount;
//        DWORD      biCompression;
//        DWORD      biSizeImage;
//        LONG       biXPelsPerMeter;
//        LONG       biYPelsPerMeter;
//        DWORD      biClrUsed;
//        DWORD      biClrImportant;
//} BITMAPINFOHEADER

//以上结构体无意义 使用windows函数库中文件头和bmp头结构体

typedef unsigned char Uchar; //改正 char

typedef struct {
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    vector<vector<Uchar>> imgData;
}ImgInfo; 

//因为调色板最大256种调色 所以建立256元素2维数组
Uchar colorTable[256][4] = { NULL }; 

void CL(ImgInfo imgInfo, FILE* fp) { //调色板读取函数
    //printf("test"); 仅用作函数执行标记
    fseek(fp, 54, SEEK_SET); //重置指针指向调色板开始处
    for (int i = 0; i < 256; i++) {
        fread(&(colorTable[i][0]), 1, 1, fp);
        fread(&(colorTable[i][1]), 1, 1, fp);
        fread(&(colorTable[i][2]), 1, 1, fp);
        fread(&(colorTable[i][3]), 1, 1, fp);
        //colorTable[i][j]; 
    }

}


//根据图片路径读取Bmp图像，生成ImgInfo对象
ImgInfo readBitmap(string imgPath) {
    ImgInfo imgInfo;
    Uchar* buf;                                              //定义文件读取缓冲区
    Uchar* p;

    FILE* fp;
    fopen_s(&fp, imgPath.c_str(), "rb");
    if (fp == NULL) {
        cout << "打开文件失败!" << endl;
        exit(0);
    }

    fread(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fp);

    if (imgInfo.bi.biBitCount != 24)
    {
        cout << "非24位bmp图" << endl;
        CL(imgInfo, fp);
        //下面是处理非24 位的过程
        fseek(fp, imgInfo.bf.bfOffBits, 0); //将指针重置为索引开始处

        buf = (Uchar*)malloc(imgInfo.bi.biWidth * imgInfo.bi.biHeight);
       
        fread(buf, 1, imgInfo.bi.biWidth * imgInfo.bi.biHeight, fp);

        p = buf; //索引数组开始的地方

        vector<vector<Uchar>> imgData;  //创建 索引到真实图像的映射 imgData实际上是一维数组
        for (int y = 0; y < imgInfo.bi.biHeight; y++) {
            for (int x = 0; x < imgInfo.bi.biWidth; x++) {
                vector<Uchar> vRGB;

                vRGB.push_back(colorTable[*(p)][0]);     //blue
                vRGB.push_back(colorTable[*(p)][1]);     //green
                vRGB.push_back(colorTable[*(p)][2]);     //red 保留字省略 所以调色板设成全局数组
                p++; //下一组索引转换
                if (x == imgInfo.bi.biWidth - 1)
                {
                    for (int k = 0; k < imgInfo.bi.biWidth % 4; k++) p++; //4字节对齐将自动填充的数据跳过
                }  
                imgData.push_back(vRGB);
            }
        }

        //到这里为止
        fclose(fp);
        imgInfo.imgData = imgData;

        //exit(0);
    }
    else
    {
        fseek(fp, imgInfo.bf.bfOffBits, 0); //将指针重置为数据或者索引开始处

        buf = (Uchar*)malloc(imgInfo.bi.biWidth * imgInfo.bi.biHeight * 3);
        fread(buf, 1, imgInfo.bi.biWidth * imgInfo.bi.biHeight * 3, fp);//

        p = buf; //

        vector<vector<Uchar>> imgData;  //
        for (int y = 0; y < imgInfo.bi.biHeight; y++) {
            for (int x = 0; x < imgInfo.bi.biWidth; x++) {
                vector<Uchar> vRGB; // 

                vRGB.push_back(*(p++));     //blue
                vRGB.push_back(*(p++));     //green
                vRGB.push_back(*(p++));     //red

                if (x == imgInfo.bi.biWidth - 1)
                {
                    for (int k = 0; k < imgInfo.bi.biWidth % 4; k++) p++;
                }
                imgData.push_back(vRGB);
            }
        }
        fclose(fp);
        imgInfo.imgData = imgData;
    }
    //fclose(fp);
    //imgInfo.imgData = imgData; 
    return imgInfo;
}

void showBitmap(ImgInfo imgInfo) {
    HWND hWindow;                                                //窗口句柄
    HDC hDc;                                                     //绘图设备环境句柄
    int yOffset = 150;
    hWindow = GetForegroundWindow();
    hDc = GetDC(hWindow);

    int posX, posY;
    for (int i = 0; i < imgInfo.imgData.size(); i++) {
        Uchar blue = imgInfo.imgData.at(i).at(0);
        Uchar green = imgInfo.imgData.at(i).at(1);
        Uchar red = imgInfo.imgData.at(i).at(2);

        posX = i % imgInfo.bi.biWidth;
        posY = imgInfo.bi.biHeight - i / imgInfo.bi.biWidth + yOffset;
        SetPixel(hDc, posX, posY, RGB(red, green, blue));
    }
    //system("pause");
}

void showBitmapGray(ImgInfo bmpImg, vector<Uchar> gray) {
    HWND hWindow;
    HDC hDc;
    int yOffset = 200;
    hWindow = GetForegroundWindow();
    hDc = GetDC(hWindow);

    int posX, posY;
    int size = bmpImg.imgData.size();

    for (int i = 0; i < size; i++) {
        // RGB表 顺序 -- R、G、B
        // 从vector中依次取出各像素点的三通道取值
        Uchar t_gray = gray[i];

        posX = i % bmpImg.bi.biWidth;
        posY = bmpImg.bi.biHeight - i / bmpImg.bi.biWidth + yOffset;
        SetPixel(hDc, posX, posY, RGB(t_gray, t_gray, t_gray));
    }
    //system("pause");
}

void saveBitmap(ImgInfo imgInfo) {
    FILE* fpw;
    fopen_s(&fpw, saveImgPath.c_str(), "wb");
    fwrite(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fpw);  //写入文件头
    fwrite(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fpw);  //写入文件头信息

    int size = imgInfo.bi.biWidth * imgInfo.bi.biHeight;

    if (imgInfo.bi.biBitCount == 24)
    {
        for (int i = 0; i < size; i++) {
            fwrite(&imgInfo.imgData.at(i).at(0), 1, 1, fpw);
            fwrite(&imgInfo.imgData.at(i).at(1), 1, 1, fpw);
            fwrite(&imgInfo.imgData.at(i).at(2), 1, 1, fpw);

            if (i % imgInfo.bi.biWidth == imgInfo.bi.biWidth - 1) {
                Uchar ch = '0';
                for (int j = 0; j < imgInfo.bi.biWidth % 4; j++) {
                    fwrite(&ch, 1, 1, fpw);
                }
            }
        }
    }
    else //非24位bmp直接存入索引值 利用RGB三个值方向索引调色板数据 
    {
        for (int i = 0; i < size; i++) {
            fwrite(&imgInfo.imgData.at(i).at(0), 1, 1, fpw);
            fwrite(&imgInfo.imgData.at(i).at(1), 1, 1, fpw);
            fwrite(&imgInfo.imgData.at(i).at(2), 1, 1, fpw);

            if (i % imgInfo.bi.biWidth == imgInfo.bi.biWidth - 1) {
                Uchar ch = '0';
                for (int j = 0; j < imgInfo.bi.biWidth % 4; j++) {
                    fwrite(&ch, 1, 1, fpw);
                }
            }
        }
    }
    fclose(fpw);
    cout << "已保存图像至: " + saveImgPath << endl;
}

//待做: 3通道转单通道处理函数 已做
vector<Uchar> T2O(ImgInfo imgInfo)
{
    vector<Uchar> oImg;
    for (int i = 0; i < imgInfo.imgData.size(); i++) {
        Uchar blue = imgInfo.imgData.at(i).at(0);
        Uchar green = imgInfo.imgData.at(i).at(1);
        Uchar red = imgInfo.imgData.at(i).at(2);
        Uchar gray = ((blue + green + red) / 3); //
        oImg.push_back(gray); //
    }
    return oImg;
}




int radius = 3;//
int convolution(int* operatr, Uchar* block, int radius) {
    int value = 0;
    for (int i = 0; i < radius; i++) {
        for (int j = 0; j < radius; j++) {
            value += operatr[i * radius + j] * block[i * radius + j]; //图像块和模板对应相乘
        }
    }
    return value;
}//返回卷积和

//下面是均值滤波
void meanFliter(vector<Uchar>& image, int width, int height, vector<Uchar>& outImg) {
    int smth[9] = { 1,1,1,1,1,1,1,1,1 };//暂定radius=3 模板大小是9
    Uchar block[9];
    int value = 0;
    int i, j, m, n;
    //for (i = 0; i < 9; i++) {
    //    smth[i] = 1; //初始化滤波模板
    //}

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (i == 0 || j == 0 || i == height - 1 || j == width - 1)
                outImg[i * width + j] = 0;
            else {
                for (m = -1; m < 2; m++)
                    for (n = -1; n < 2; n++)
                        block[(m + 1) * 3 + n + 1] = image[(i + m) * width + j + n]; //填充图像处理块 
                value = convolution(smth, block, radius); //计算卷积和
                outImg[i * width + j] = Uchar(value / 9);//计算均值 填到左上角
                //outImg[i * width + j] = Uchar(value);//拉普拉斯

            }
        }
    }
    cout << "均值滤波" << endl;
}

//均值滤波函数调用函数

vector<Uchar> callMeanF(vector<Uchar>& oImg, int width, int height, ImgInfo imgInfo) {

    vector<Uchar> outImgmod(imgInfo.bi.biWidth * imgInfo.bi.biHeight); //处理后图像存储
    meanFliter(oImg, width, height, outImgmod);
    return outImgmod;
}

int midValueFind(int num, Uchar* d); //
//中值滤波
void midFindFlitering(vector<Uchar>& imdge, int width, int height, vector<Uchar>& outImg)
{
    int radius = 3;//模板半径
    int i, j, m, n;
    Uchar blockt[9]; 
    int value;
    int blockNum = 9;
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
        {
            if (i == 0 || j == 0 || i == height - 1 || j == width - 1)
            {
                outImg[i * width + j] = 0;
            }
            else
            {
                for (m = -1; m < 2; m++) {
                    for (n = -1; n < 2; n++) {
                        blockt[(m + 1) * 3 + (n + 1)] = imdge[(i + m) * width + (j + n)];
                    }

                }
                value = midValueFind(blockNum, blockt);
                outImg[i * width + j] = value;
            }
        }
    cout << "中值滤波" << endl;
}


//中值寻找函数 和上面的配合使用
int midValueFind(int num, Uchar* d)//选择排序
{
int value;
int i, j;
int temp;
for (i = 0; i < num - 1; i++)
    for (j = i + 1; j < num; j++)
    {
        if (d[i] < d[j])
        {
            temp = d[i];
            d[i] = d[j];
            d[j] = temp;
        }
    }
return d[num / 2]; //返回中值

}

vector<Uchar> callMvf(vector<Uchar>& oImg, int width, int height, ImgInfo imgInfo) {

    vector<Uchar> outImgmod(imgInfo.bi.biWidth * imgInfo.bi.biHeight); //处理后图像存储
    meanFliter(oImg, width, height, outImgmod);
    return outImgmod;
}

//下面是动态模板
void dyCore(vector<Uchar>& image, int width, int height, vector<Uchar>& outImg,int * core) {
    //int *core = core;//暂定radius=3 模板大小是9
    Uchar block[9];
    int value = 0;
    int i, j, m, n;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (i == 0 || j == 0 || i == height - 1 || j == width - 1)
                outImg[i * width + j] = 0;
            else {
                for (m = -1; m < 2; m++)
                    for (n = -1; n < 2; n++)
                        block[(m + 1) * 3 + n + 1] = image[(i + m) * width + j + n]; //填充图像处理块 
                value = convolution(core, block, radius); //计算卷积和
                outImg[i * width + j] = Uchar(value);//拉普拉斯

            }
        }
    }
    cout << "自定义滤波" << endl;
}

vector<Uchar> calldycore(vector<Uchar>& oImg, int width, int height, ImgInfo imgInfo,int* core) {

    vector<Uchar> outImgmod(imgInfo.bi.biWidth * imgInfo.bi.biHeight); //处理后图像存储
    dyCore(oImg, width, height, outImgmod,core);
    return outImgmod;
}


int main() {
    ImgInfo imgInfo = readBitmap(imgPath); //读取 rgb彩图
    int showflag = 0;
    cout << "显示原图像输入  1  ，显示均值滤波图像输入  2  , 显示中值滤波图像输入  3  ，进行自定义模板系数处理输入  4  " << endl;
    if (scanf("%d", &showflag) != 1) {
        printf("输入错误！");
        return 0;
    }
    if (showflag == 1) {
        showBitmap(imgInfo);
        
    }
    if (showflag == 2)
    {
        vector<Uchar>oImg = T2O(imgInfo);//灰度转化
        vector<Uchar>outImg = callMeanF(oImg, imgInfo.bi.biWidth, imgInfo.bi.biHeight, imgInfo);//获取处理之后的 灰度
        showBitmapGray(imgInfo, outImg);
       
    }
    if (showflag == 3)
    {
        vector<Uchar>oImg = T2O(imgInfo);//灰度转化
        vector<Uchar>outImg = callMvf(oImg, imgInfo.bi.biWidth, imgInfo.bi.biHeight, imgInfo);//获取处理之后的 灰度
        showBitmapGray(imgInfo, outImg);
      
    }
    if (showflag==4) 
    {   
        int core[9] = { 0 };
        cout << "输入9位模板系数以逗号分割" << endl;
        scanf("%d,%d,%d,%d,%d,%d,%d,%d,%d,", &core[0], &core[1], &core[2], &core[3], &core[4], &core[5], &core[6], &core[7], &core[8]);
        vector<Uchar>oImg = T2O(imgInfo);//灰度转化
        vector<Uchar>outImg = calldycore(oImg, imgInfo.bi.biWidth, imgInfo.bi.biHeight, imgInfo,core);//获取处理之后的 灰度
        showBitmapGray(imgInfo, outImg);
    }
    //system("pause");
    //saveBitmap(imgInfo);//
    return 0;
}
