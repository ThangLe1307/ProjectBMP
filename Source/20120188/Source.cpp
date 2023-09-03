#include "Header.h"
#pragma pack(1)	
int main(int argc, char* argv[])
{
	int padding = 0;
	BMPimg curBMP,resultBMP;
	if (argc < 4 || argc>5)
	{
		cout << "nhap sai cu phap!";
		return 1;
	}
	if (argv[1] == string("-conv"))
	{
		int kt;
		kt = ReadBMP(argv[2], curBMP, padding);
		if (kt == 0)
		{
			ConvertTo8(curBMP, resultBMP, padding);
			writeBMP8(argv[3], resultBMP);
		}
	}
	else
		if (argv[1] == string("-zoom"))
		{
			//chuyển argv[4] thành số
			int s = atoi(string(argv[4]).c_str());
			int kt;
			kt = ReadBMP(argv[2], curBMP, padding);
			if (kt == 0)
			{
				zoom(curBMP, resultBMP, padding, s);
				//nếu ảnh là 8bpp thì cần thêm bảng màu
				if (curBMP.dib.bpp == 8)
					writeBMP8(argv[3], resultBMP);
				//ảnh 24bpp, 32bpp thì không cần thêm bảng màu
				else
					writeBMP(argv[3], resultBMP);
			}
		}
		else
		{
			cout << "nhap sai cu phap!";
			return 1;
		}
	return 0;
	system("pause");
}