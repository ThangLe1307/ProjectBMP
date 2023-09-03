#include "Header.h"
#pragma pack(1)	
//hàm đọc
int ReadBMP(char* FileName, BMPimg& bmp,int& padding)
{

	ifstream fi(FileName,  ios::binary);
	if (fi.is_open())
	{		//đọc phần header và một số phần quan trọng của DIB:
		fi.read((char*)&bmp,38);			
			//cấp phát vùng nhớ để đọc phần không quan trọng của DIB:
		bmp.dib.PhanDu = new char[bmp.dib.size-24];		
			// đọc phần không quan trọng của DIB:
		fi.read((char*)bmp.dib.PhanDu,sizeof(bmp.dib.PhanDu));	
			//tính số padding 
		padding = 4 - ((bmp.dib.Ngang* (bmp.dib.bpp/8)) %4);
		if (padding == 4) padding = 0;
			//tính imgsize vì một số file có imgsize = 0
		bmp.dib.imgsize = bmp.dib.Cao * bmp.dib.Ngang * (bmp.dib.bpp / 8)+padding* bmp.dib.Cao;
			//chuyển con trỏ đến dataoffset để đọc pixel data
		fi.seekg(bmp.Header.Dataoffset);
		bmp.data = new unsigned char[bmp.dib.imgsize];
		fi.read((char*)bmp.data, bmp.dib.imgsize);
		fi.close();
		cout << "doc file thanh cong!" << endl;;
		return 0;
	}
	else
	{
		cout << "doc file khong thanh cong!" << endl;;
		return 1;
	}
}


//hàm tạo thêm bảng màu và ghi file 8bpp
int writeBMP8(char* FileName, BMPimg& bmp)
{
	ofstream fo(FileName, ios::out | ios::binary);
	if (!fo.is_open())
	{
		cout << "ghi file khong thanh cong!" << endl;;
		return 1;
	}
	bmp.Header.reserved = 4;
	fo.write((char*)&bmp.Header, 14);
	fo.write((char*)&bmp.dib, bmp.dib.size);
	unsigned char clr[1024];
	// set giá trị mặc định cho bảng màu 
	memset(clr, 0, 1024);
	// tạo bảng màu
	for (int i = 0; i < 256;i++)
	{
		//bảng màu xám thì R=G=B
		clr[i * 4 + 1] = clr[i * 4 + 2] = clr[i * 4 + 0]=(unsigned char)i;
		clr[i * 4 + 3] = 0;
	}
	fo.write((char*)clr, 1024);
	fo.write((char*)bmp.data, bmp.dib.imgsize);
	cout << "ghi file thanh cong!" << endl;
	return 0;
}
//hàm ghi file BMP 24bpp,32bpp
int writeBMP(char* FileName, BMPimg& bmp)
{
	ofstream fo(FileName, ios::out | ios::binary);
	if (!fo.is_open())
	{
		cout << "ghi file khong thanh cong!" << endl;
		return 1;
	}

	fo.write((char*)&bmp.Header, 14);
	fo.write((char*)&bmp.dib, bmp.dib.size);
	fo.write((char*)bmp.data, bmp.dib.imgsize);
	cout << "ghi file thanh cong!" << endl;
	return 0;
}
//hàm chuyển sang 8bpp
int ConvertTo8(BMPimg& curBmp, BMPimg& resultBmp, int padding)
{
	//nếu ảnh sẵn 8bpp thì không cần chuyển
	if (curBmp.dib.bpp == 8) 
	{ 
		resultBmp = curBmp; 
		return 1; 
	}
	//tạo và set lại giá trị cho file bmp mới
	//bytepp: byte per pixel
	int bytepp=curBmp.dib.bpp/8;
	resultBmp.Header = curBmp.Header;
	resultBmp.dib = curBmp.dib;
	resultBmp.dib.bpp = 8;
	resultBmp.Header.Dataoffset += 1024;
	//tính padding của file kết quả
	int padding1;
	padding1 = 4 - ((resultBmp.dib.Ngang * (resultBmp.dib.bpp / 8)) % 4);
	if (padding1 == 4) padding1 = 0;
	//tính imgsize của file kết quả
	resultBmp.dib.imgsize = resultBmp.dib.Cao * resultBmp.dib.Ngang+ padding1 * resultBmp.dib.Cao;
	resultBmp.data = new unsigned char[resultBmp.dib.imgsize];
	resultBmp.Header.filesize = sizeof(resultBmp);
	//k là chỉ số của mỗi pixel file kết quả
	int k = 0;
	//xử lý pixel data
	for (int i=0;i<curBmp.dib.Cao;i++)
		for (int j = 0; j < curBmp.dib.Ngang*bytepp; j += bytepp)
		{
			//tìm vị trí theo i, j
			int pos = i * (curBmp.dib.Ngang) * bytepp + padding*i + j;
			unsigned char avg =(unsigned char) curBmp.data[pos]/3+ curBmp.data[pos+1]/3+ curBmp.data[pos+2]/3;
			resultBmp.data[k] = avg;
			//khi k chạm tới padding thì thêm padding vào bằng cách tăng k lên
			if (k % (resultBmp.dib.Ngang+padding1)== (resultBmp.dib.Ngang-1) && k !=0) k =k+ padding1+1;
			else
			//ngược lại tăng k lên để gán pixel tiếp theo vào
			k++;
		}
	return 0;
}
//hàm thu nhỏ theo tỉ lệ s
void zoom(BMPimg& curBmp, BMPimg& resultBmp, int padding, int s)
{
	//tạo và set lại giá trị cho file bmp mới
	//bytepp: byte per pixel
	int bytepp = curBmp.dib.bpp / 8;
	resultBmp.Header = curBmp.Header;
	resultBmp.dib = curBmp.dib;
	resultBmp.dib.Ngang /= s;
	resultBmp.dib.Cao /= s;

	//nếu with, height không chia hết cho S thì thêm 1 pixel để tính trung bình phần thừa
	if (curBmp.dib.Ngang % s != 0) resultBmp.dib.Ngang++;
	if (curBmp.dib.Cao % s != 0) resultBmp.dib.Cao++;

	//tính padding của file kết quả
	int padding1;
	padding1 = 4 - ((resultBmp.dib.Ngang * (resultBmp.dib.bpp / 8)) % 4);
	if (padding1 == 4) padding1 = 0;

	//tính imgsize của file kết quả
	resultBmp.dib.imgsize = resultBmp.dib.Cao * resultBmp.dib.Ngang*bytepp + padding1 * resultBmp.dib.Cao;
	resultBmp.data = new unsigned char[resultBmp.dib.imgsize];
	resultBmp.Header.filesize = sizeof(resultBmp);

	//xu ly pixel data
	//k là chỉ số của từng pixel trong file kết quả
	int k = 0;
	for (int i = 0; i < resultBmp.dib.Cao; i++)
		for (int j = 0; j < resultBmp.dib.Ngang; j += 1)
		{
			//avg: các giá trị trung bình của từng ô vuông SxS
			float avgG=0,avgR=0,avgB=0,avgA=0;
			//xStar, xEnd, yStar, yEnd: các chỉ số để xử lý pixel data, lấy ra từng ô SxS
			int xStar, xEnd, yStar, yEnd;
			xStar = i * s;
			yStar = j * s;
			//trường hợp with, height không chia hết cho S, ta chỉ xét đến with-1,height-1 
			if ((i + 1) * s < curBmp.dib.Cao) xEnd = (i + 1) * s;
			else xEnd = curBmp.dib.Cao - 1;
			if ((j + 1) * s < curBmp.dib.Ngang) yEnd = (j + 1) * s;
			else yEnd = curBmp.dib.Ngang - 1;
			//duyệt qua từng ô SxS (hoặc ô thừa ở bìa)
			for (int i1 = xStar; i1 < xEnd; i1++)
				for (int j1 = yStar; j1 < yEnd; j1++)
				{
					//pos là chỉ số pixel dòng i1 cột j1 trong source pixel data 
					//case 4,3,1 ứng với ảnh 32bpp, 24bpp, 8bpp
					int pos = i1 * (curBmp.dib.Ngang) * bytepp + padding * i1 + j1*bytepp;
					switch (bytepp)
					{
					case 4:
						avgA += (float)curBmp.data[pos + 3] / (s * s);
					case 3:
						avgG += (float)curBmp.data[pos + 1] / (s * s);
						avgB += (float)curBmp.data[pos + 2] / (s * s);
					case 1:
						avgR += (float)curBmp.data[pos] / (s * s);
					}
				}
			//gán từng giá trị trung bình vào kết quả
			//case 4,3,1 ứng với ảnh 32bpp, 24bpp, 8bpp
			switch(bytepp)
			{
			case 4:
				resultBmp.data[k + 3] = (unsigned char)avgA;
			case 3:
				resultBmp.data[k + 1] = (unsigned char)avgG;
				resultBmp.data[k + 2] = (unsigned char)avgB;
			case 1:
				resultBmp.data[k] = (unsigned char)avgR;
			}
			//khi k chạm tới padding thì thêm padding vào bằng cách tăng k lên
			if (j== resultBmp.dib.Ngang-1) k = k + padding1+bytepp;
			else
			//ngược lại, tăng k lên để gán pixel tiếp theo
			k+=bytepp;

		}
}