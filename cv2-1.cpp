#include <iostream>
#include <Windows.h>
#include <malloc.h>
#include <vector>
#include<stdlib.h>
//��ע�⣡������Ŀ ��Ϊʹ����scanf() ��Ҫ����Ŀ�����йر�SDL��������������
using namespace std;

string imgPath = "R-C2.bmp";//"test3.bmp";
string saveImgPath = "test3.mod.bmp";




//typedef struct tagBITMAPFILEHEADER {
//    WORD    bfType;
//    DWORD   bfSize;
//    WORD    bfReserved1;
//    WORD    bfReserved2;
//    DWORD   bfOffBits; //ƫ���� ƫ��֮����Ǽ�¼������������ʵ���� ��ɫ�����ݵ��� ƫ��-ͷ��С��14+40����ע�� ÿ����ɫ�����ݰ����ĸ��ֽڣ�
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

//���Ͻṹ�������� ʹ��windows���������ļ�ͷ��bmpͷ�ṹ��

typedef unsigned char Uchar; //���� char

typedef struct {
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    vector<vector<Uchar>> imgData;
}ImgInfo; 

//��Ϊ��ɫ�����256�ֵ�ɫ ���Խ���256Ԫ��2ά����
Uchar colorTable[256][4] = { NULL }; 

void CL(ImgInfo imgInfo, FILE* fp) { //��ɫ���ȡ����
    //printf("test"); ����������ִ�б��
    fseek(fp, 54, SEEK_SET); //����ָ��ָ���ɫ�忪ʼ��
    for (int i = 0; i < 256; i++) {
        fread(&(colorTable[i][0]), 1, 1, fp);
        fread(&(colorTable[i][1]), 1, 1, fp);
        fread(&(colorTable[i][2]), 1, 1, fp);
        fread(&(colorTable[i][3]), 1, 1, fp);
        //colorTable[i][j]; 
    }

}


//����ͼƬ·����ȡBmpͼ������ImgInfo����
ImgInfo readBitmap(string imgPath) {
    ImgInfo imgInfo;
    Uchar* buf;                                              //�����ļ���ȡ������
    Uchar* p;

    FILE* fp;
    fopen_s(&fp, imgPath.c_str(), "rb");
    if (fp == NULL) {
        cout << "���ļ�ʧ��!" << endl;
        exit(0);
    }

    fread(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fp);

    if (imgInfo.bi.biBitCount != 24)
    {
        cout << "��24λbmpͼ" << endl;
        CL(imgInfo, fp);
        //�����Ǵ����24 λ�Ĺ���
        fseek(fp, imgInfo.bf.bfOffBits, 0); //��ָ������Ϊ������ʼ��

        buf = (Uchar*)malloc(imgInfo.bi.biWidth * imgInfo.bi.biHeight);
       
        fread(buf, 1, imgInfo.bi.biWidth * imgInfo.bi.biHeight, fp);

        p = buf; //�������鿪ʼ�ĵط�

        vector<vector<Uchar>> imgData;  //���� ��������ʵͼ���ӳ�� imgDataʵ������һά����
        for (int y = 0; y < imgInfo.bi.biHeight; y++) {
            for (int x = 0; x < imgInfo.bi.biWidth; x++) {
                vector<Uchar> vRGB;

                vRGB.push_back(colorTable[*(p)][0]);     //blue
                vRGB.push_back(colorTable[*(p)][1]);     //green
                vRGB.push_back(colorTable[*(p)][2]);     //red ������ʡ�� ���Ե�ɫ�����ȫ������
                p++; //��һ������ת��
                if (x == imgInfo.bi.biWidth - 1)
                {
                    for (int k = 0; k < imgInfo.bi.biWidth % 4; k++) p++; //4�ֽڶ��뽫�Զ�������������
                }  
                imgData.push_back(vRGB);
            }
        }

        //������Ϊֹ
        fclose(fp);
        imgInfo.imgData = imgData;

        //exit(0);
    }
    else
    {
        fseek(fp, imgInfo.bf.bfOffBits, 0); //��ָ������Ϊ���ݻ���������ʼ��

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
    HWND hWindow;                                                //���ھ��
    HDC hDc;                                                     //��ͼ�豸�������
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
        // RGB�� ˳�� -- R��G��B
        // ��vector������ȡ�������ص����ͨ��ȡֵ
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
    fwrite(&imgInfo.bf, sizeof(BITMAPFILEHEADER), 1, fpw);  //д���ļ�ͷ
    fwrite(&imgInfo.bi, sizeof(BITMAPINFOHEADER), 1, fpw);  //д���ļ�ͷ��Ϣ

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
    else //��24λbmpֱ�Ӵ�������ֵ ����RGB����ֵ����������ɫ������ 
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
    cout << "�ѱ���ͼ����: " + saveImgPath << endl;
}

//����: 3ͨ��ת��ͨ�������� ����
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
            value += operatr[i * radius + j] * block[i * radius + j]; //ͼ����ģ���Ӧ���
        }
    }
    return value;
}//���ؾ����

//�����Ǿ�ֵ�˲�
void meanFliter(vector<Uchar>& image, int width, int height, vector<Uchar>& outImg) {
    int smth[9] = { 1,1,1,1,1,1,1,1,1 };//�ݶ�radius=3 ģ���С��9
    Uchar block[9];
    int value = 0;
    int i, j, m, n;
    //for (i = 0; i < 9; i++) {
    //    smth[i] = 1; //��ʼ���˲�ģ��
    //}

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (i == 0 || j == 0 || i == height - 1 || j == width - 1)
                outImg[i * width + j] = 0;
            else {
                for (m = -1; m < 2; m++)
                    for (n = -1; n < 2; n++)
                        block[(m + 1) * 3 + n + 1] = image[(i + m) * width + j + n]; //���ͼ����� 
                value = convolution(smth, block, radius); //��������
                outImg[i * width + j] = Uchar(value / 9);//�����ֵ ����Ͻ�
                //outImg[i * width + j] = Uchar(value);//������˹

            }
        }
    }
    cout << "��ֵ�˲�" << endl;
}

//��ֵ�˲��������ú���

vector<Uchar> callMeanF(vector<Uchar>& oImg, int width, int height, ImgInfo imgInfo) {

    vector<Uchar> outImgmod(imgInfo.bi.biWidth * imgInfo.bi.biHeight); //�����ͼ��洢
    meanFliter(oImg, width, height, outImgmod);
    return outImgmod;
}

int midValueFind(int num, Uchar* d); //
//��ֵ�˲�
void midFindFlitering(vector<Uchar>& imdge, int width, int height, vector<Uchar>& outImg)
{
    int radius = 3;//ģ��뾶
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
    cout << "��ֵ�˲�" << endl;
}


//��ֵѰ�Һ��� ����������ʹ��
int midValueFind(int num, Uchar* d)//ѡ������
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
return d[num / 2]; //������ֵ

}

vector<Uchar> callMvf(vector<Uchar>& oImg, int width, int height, ImgInfo imgInfo) {

    vector<Uchar> outImgmod(imgInfo.bi.biWidth * imgInfo.bi.biHeight); //�����ͼ��洢
    meanFliter(oImg, width, height, outImgmod);
    return outImgmod;
}

//�����Ƕ�̬ģ��
void dyCore(vector<Uchar>& image, int width, int height, vector<Uchar>& outImg,int * core) {
    //int *core = core;//�ݶ�radius=3 ģ���С��9
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
                        block[(m + 1) * 3 + n + 1] = image[(i + m) * width + j + n]; //���ͼ����� 
                value = convolution(core, block, radius); //��������
                outImg[i * width + j] = Uchar(value);//������˹

            }
        }
    }
    cout << "�Զ����˲�" << endl;
}

vector<Uchar> calldycore(vector<Uchar>& oImg, int width, int height, ImgInfo imgInfo,int* core) {

    vector<Uchar> outImgmod(imgInfo.bi.biWidth * imgInfo.bi.biHeight); //�����ͼ��洢
    dyCore(oImg, width, height, outImgmod,core);
    return outImgmod;
}


int main() {
    ImgInfo imgInfo = readBitmap(imgPath); //��ȡ rgb��ͼ
    int showflag = 0;
    cout << "��ʾԭͼ������  1  ����ʾ��ֵ�˲�ͼ������  2  , ��ʾ��ֵ�˲�ͼ������  3  �������Զ���ģ��ϵ����������  4  " << endl;
    if (scanf("%d", &showflag) != 1) {
        printf("�������");
        return 0;
    }
    if (showflag == 1) {
        showBitmap(imgInfo);
        
    }
    if (showflag == 2)
    {
        vector<Uchar>oImg = T2O(imgInfo);//�Ҷ�ת��
        vector<Uchar>outImg = callMeanF(oImg, imgInfo.bi.biWidth, imgInfo.bi.biHeight, imgInfo);//��ȡ����֮��� �Ҷ�
        showBitmapGray(imgInfo, outImg);
       
    }
    if (showflag == 3)
    {
        vector<Uchar>oImg = T2O(imgInfo);//�Ҷ�ת��
        vector<Uchar>outImg = callMvf(oImg, imgInfo.bi.biWidth, imgInfo.bi.biHeight, imgInfo);//��ȡ����֮��� �Ҷ�
        showBitmapGray(imgInfo, outImg);
      
    }
    if (showflag==4) 
    {   
        int core[9] = { 0 };
        cout << "����9λģ��ϵ���Զ��ŷָ�" << endl;
        scanf("%d,%d,%d,%d,%d,%d,%d,%d,%d,", &core[0], &core[1], &core[2], &core[3], &core[4], &core[5], &core[6], &core[7], &core[8]);
        vector<Uchar>oImg = T2O(imgInfo);//�Ҷ�ת��
        vector<Uchar>outImg = calldycore(oImg, imgInfo.bi.biWidth, imgInfo.bi.biHeight, imgInfo,core);//��ȡ����֮��� �Ҷ�
        showBitmapGray(imgInfo, outImg);
    }
    //system("pause");
    //saveBitmap(imgInfo);//
    return 0;
}
