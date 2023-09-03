#include<iostream>
#include<fstream>
#include<string>
// theo như em xem trên stackoverflow thì phần này để ngăn thêm padding vào struct
// không có thì bị đọc sai
#pragma pack(1)	
		
using namespace std;
struct header
{
	char signature[2];
	unsigned int filesize;
	unsigned int reserved;
	unsigned int Dataoffset;	//Địa chỉ phần bắt đầu lưu dữ liệu điểm ảnh

};

struct DIB
{
	
	int size;		//DIB size
	int Ngang;		//width
	int Cao;		//height
	short planes;	//số lớp màu
	short bpp;		//bit per pixel
	int compression;
	int imgsize;
	char* PhanDu;	//con trỏ chỉ tới vùng lưu phần dư của DIB
};
struct BMPimg
{
	header Header;			//lưu phần header
	DIB dib;				//lưu phần DIB
	unsigned char* data;	//con trỏ chỉ tói vùng lưu dữ liệu điểm ảnh
};

int ReadBMP(char* FileName,BMPimg& bmp,int& padding);
int writeBMP8(char* FileName,BMPimg& bmp);
int ConvertTo8(BMPimg& curBmp, BMPimg& resultBmp, int padding);
void zoom(BMPimg& curBmp, BMPimg& resultBmp, int padding, int s);
int writeBMP(char* FileName, BMPimg& bmp);

