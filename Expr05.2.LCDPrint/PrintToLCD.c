#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "PrintToLCD.h"
#include "Font_8x16.h"

unsigned char * sDataToPrint; //需要打印的字符串
unsigned int iPrintPosX = 0; //打印开始的X坐标
unsigned int iPrintPosY = 0; //打印开始的Y坐标
int IsClearScreen = 1; //指示是否需要清屏
int iOverflowAction = 1; //指示文字内容溢出时的操作
int iForeground = 0x00ffffff; //前景色（文字颜色）
int iBackground = 0x00000000; //背景色（文字背景颜色）

static char * lpFramebuffer = 0; //映射到内存的Framebuffer的起始地址
static unsigned int iXResolution = 0; //屏幕横向分辨率
static unsigned int iYResolution = 0; //屏幕纵向分辨率
static unsigned int iBitsPerPixel = 0; //每像素色深
int hFramebuffer = 0; //Framebuffer设备文件句柄

struct fb_var_screeninfo varScrInfo; //动态信息
struct fb_fix_screeninfo fixScrInfo; //静态信息
long int iScreenSize = 0; //屏幕大小，用字节描述，反映内存映射区域的大小

void PrintManual(); //打印用法
char * MapFramebufferToMemory(); //映射Framebuffer到内存
void UnmapFramebuffer(); //取消映射Framebuffer
int DrawCharToFrameBuffer(unsigned char chrCharacter, unsigned int iPosX, unsigned int iPosY, unsigned int iCharacterWidth, unsigned int iCharacterHeight); //绘制文本

int main(int iArgCount, char * arrArg[]){
	setbuf(stdout, NULL); //关闭标准输出的缓冲
	
	/* 处理用户输入的参数 */
	if (iArgCount <= 1){
		PrintManual();
		return 1;
	}
	switch (iArgCount){
		case 2:
			sDataToPrint=arrArg[1];
			iPrintPosX=0;
			iPrintPosY=0;
			IsClearScreen=1;
			iOverflowAction = 1;
			iForeground = 0x00ffffff;
			iBackground = 0x00000000;
			break;
		case 3:
			sDataToPrint=arrArg[1];
			if (sscanf(arrArg[2], "%u", &iPrintPosX) != 1){
				PrintManual();
				return 1;
			}
			iPrintPosY=0;
			IsClearScreen=1;
			iOverflowAction = 1;
			iForeground = 0x00ffffff;
			iBackground = 0x00000000;
			break;
		case 4:
			sDataToPrint=arrArg[1];
			if (sscanf(arrArg[2], "%u", &iPrintPosX) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[3], "%u", &iPrintPosY) != 1){
				PrintManual();
				return 1;
			}
			IsClearScreen=1;
			iOverflowAction = 1;
			iForeground = 0x00ffffff;
			iBackground = 0x00000000;
			break;
		case 5:
			sDataToPrint=arrArg[1];
			if (sscanf(arrArg[2], "%u", &iPrintPosX) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[3], "%u", &iPrintPosY) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[4], "%d", &IsClearScreen) != 1){
				PrintManual();
				return 1;
			}
			iOverflowAction = 1;
			iForeground = 0x00ffffff;
			iBackground = 0x00000000;
			break;
		case 6:
			sDataToPrint=arrArg[1];
			if (sscanf(arrArg[2], "%u", &iPrintPosX) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[3], "%u", &iPrintPosY) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[4], "%d", &IsClearScreen) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[5], "%d", &iOverflowAction) != 1){
				PrintManual();
				return 1;
			}
			iForeground = 0x00ffffff;
			iBackground = 0x00000000;
			break;
		case 7:
			sDataToPrint=arrArg[1];
			if (sscanf(arrArg[2], "%u", &iPrintPosX) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[3], "%u", &iPrintPosY) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[4], "%d", &IsClearScreen) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[5], "%d", &iOverflowAction) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[6], "%d", &iForeground) != 1){
				PrintManual();
				return 1;
			}
			iBackground = 0x00000000;
			break;
		case 8:
			sDataToPrint=arrArg[1];
			if (sscanf(arrArg[2], "%u", &iPrintPosX) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[3], "%u", &iPrintPosY) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[4], "%d", &IsClearScreen) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[5], "%d", &iOverflowAction) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[6], "%d", &iForeground) != 1){
				PrintManual();
				return 1;
			}
			if (sscanf(arrArg[7], "%d", &iBackground) != 1){
				PrintManual();
				return 1;
			}
			break;
	}
	/* 处理用户输入的参数结束 */
	
	/* 映射Framebuffer到内存 */
	lpFramebuffer=MapFramebufferToMemory();
	if (lpFramebuffer==NULL){
		return -1;
	}
	printf("Framebuffer mapped to virtual address 0x%x.\n", lpFramebuffer);

	/* 确定是否需要清空屏幕 */
	if (IsClearScreen){
		//Initialize framebuffer to 0 (black), added by t.c.d.
		memset(lpFramebuffer, 0, iScreenSize);
	}
	
	/* 打印文本 */
	int i=0;
	unsigned int iNextPosX = iPrintPosX;
	unsigned int iNextPosY = iPrintPosY;
	if (iNextPosX>iXResolution-CHAR_WIDTH_PIX || iNextPosY>iYResolution-CHAR_HEIGHT_PIX){
		printf("Error: Invalid position.\n");
		return -1;
	}
	while (sDataToPrint[i]!='\0'){
		iNextPosX+=DrawCharToFrameBuffer((unsigned char)(sDataToPrint[i]), iNextPosX, iNextPosY, CHAR_WIDTH_PIX, CHAR_HEIGHT_PIX); //打印文字
		//文字超出边界时，进行处理
		if (iNextPosX>=iXResolution-CHAR_WIDTH_PIX){
			if (iOverflowAction==0){
				break;
			}
			else if (iOverflowAction==1){
				iNextPosY+=CHAR_HEIGHT_PIX;
				iNextPosX=iPrintPosX;
			}
			else if (iOverflowAction==2){
				iNextPosY+=CHAR_HEIGHT_PIX;
				iNextPosX=0;
			}
		}
		if (iNextPosY>iYResolution-CHAR_HEIGHT_PIX){
			printf("Error: Invalid position.\n");
			break;
		}
		++i;
	}
	
	UnmapFramebuffer();
	close(hFramebuffer);
	
	return 0;
}

//打印函数用法
void PrintManual(){
	printf("Prints text to LCD.\n");
	printf("\n");
	printf("Usage:\n");
	printf("     PrintToLCD string [xpos=0] [ypos=0] [clearscreen=1] [overflow=1] [foreground=16777215] [background=-1]\n\n");
	printf("     \'clearscreen\' indicates if we will clear the whole screen before printing.\n");
	printf("     \'foreground\' is the color of the printed text, in (r,g,b) sequence.\n");
	printf("     \'background\' is the background color of the printed text, in (r,g,b) sequence. Set it to -1 to use transparent background.\n");
	printf("     \'overflow\' indicate the overflow operation. 0 - no operation (stops the program); 1 - break and start printing from xpos; 2 - break and start printing from 0.\n");
	printf("\n");
	return;
}

char * MapFramebufferToMemory(){
	//打开帧缓存设备文件
	hFramebuffer = open(FRAMEBUFFER_DEVICE_PATH, O_RDWR);
	if (!hFramebuffer){
		printf("Error: cannot open framebuffer device.\n");
		return NULL;
	}
	
	//获取屏幕信息
	if (ioctl(hFramebuffer, FBIOGET_FSCREENINFO, &fixScrInfo)){
		printf("Error：reading fixed information.\n");
		return NULL;
	}
	if (ioctl(hFramebuffer, FBIOGET_VSCREENINFO, &varScrInfo)){
		printf("Error: reading variable information.\n");
		return NULL;
	}

	printf("Screen size = %dx%d, BitsPerPixel = %d\n", varScrInfo.xres, varScrInfo.yres, varScrInfo.bits_per_pixel);
	iXResolution = varScrInfo.xres;
	iYResolution = varScrInfo.yres;
	iBitsPerPixel = varScrInfo.bits_per_pixel;

	//计算屏幕的总大小（字节）
	iScreenSize = varScrInfo.xres * varScrInfo.yres * varScrInfo.bits_per_pixel / 8;
	printf("ScreenSize=%d byte\n",iScreenSize);

	//对象映射
	lpFramebuffer = (char *)mmap(0, iScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, hFramebuffer, 0);
	if ((int)lpFramebuffer == -1){
		printf("Error: failed to map framebuffer device to memory.\n");
		return NULL;
	}

	return lpFramebuffer;
}

void UnmapFramebuffer(){
	munmap(lpFramebuffer, iScreenSize);
	return;
}

int DrawCharToFrameBuffer(unsigned char chrCharacter, unsigned int iPosX, unsigned int iPosY, unsigned int iCharacterWidth, unsigned int iCharacterHeight){
	if (iPosX+iCharacterWidth>iXResolution){
		return 0;
	}
	if (iPosY+iCharacterHeight>iYResolution){
		return 0;
	}
	unsigned int base = chrCharacter * CHAR_SIZE_BY_DATATYPE;
	unsigned long idx;
	unsigned int w, h;
	unsigned int row;
	unsigned CHAR_MATRIX_DATATYPE rank;

	for (h = iPosY; h < iPosY + iCharacterHeight; ++h){
		row = base + h - iPosY;
		for (w = iPosX; w < iPosX + iCharacterWidth; ++w){
			// 显存抽象的像素点索引位置
			idx = (h*iXResolution + w)*(iBitsPerPixel/8);
			//printf("Will draw at 0x%x+0x%x.\n", lpFramebuffer, idx);
			//printf("h=%u, w=%u, base=%u, row=%u.\n", h, w, base, row);
			// 该点在点阵中的位置
			rank = 1 << (ROW_SIZE_BIT - (w - iPosX));
			if (FONT_MATRIX_NAME[row] & rank){ // 如果该点在点阵中是1，就用前景色描绘该像素
				// 适配屏幕分辨率
				if (iBitsPerPixel == 32){
					// 如果是32位色，则一个像素用32位描绘
					if (iForeground>=0x00ffffff){ //确保数值合法
						iForeground=0x00ffffff;
					}
					if (iForeground<0){
						iForeground=0;
					}
					*(lpFramebuffer + idx + 0) = (char)((iForeground&0x000000ff));
					*(lpFramebuffer + idx + 1) = (char)((iForeground&0x0000ff00)>>8);
					*(lpFramebuffer + idx + 2) = (char)((iForeground&0x00ff0000)>>16);
					*(lpFramebuffer + idx + 3) = (char)(0x00);
				}
				else if (iBitsPerPixel == 24){
					// 如果是24位色，则一个像素用24位描绘
					if (iForeground>=0x00ffffff){ //确保数值合法
						iForeground=0x00ffffff;
					}
					if (iForeground<0){
						iForeground=0;
					}
					*(lpFramebuffer + idx + 0) = (char)((iForeground&0x000000ff));
					*(lpFramebuffer + idx + 1) = (char)((iForeground&0x0000ff00)>>8);
					*(lpFramebuffer + idx + 2) = (char)((iForeground&0x00ff0000)>>16);		
				}
				else if(iBitsPerPixel == 16){
					// 如果是16位色，则一个像素用16位描绘
					if (iForeground>=0x0000ffff){ //确保数值合法
						iForeground=0x0000ffff;
					}
					if (iForeground<0){
						iForeground=0;
					}
					*(lpFramebuffer + idx + 0) = (char)((iForeground&0x000000ff));
					*(lpFramebuffer + idx + 1) = (char)((iForeground&0x0000ff00)>>8);
				}
				else if(iBitsPerPixel == 8){
					// 如果是8位色，则一个像素用8位描绘
					if (iForeground>=0x000000ff){ //确保数值合法
						iForeground=0x000000ff;
					}
					if (iForeground<0){
						iForeground=0;
					}
					*(lpFramebuffer + idx + 0) = (char)(iForeground);
				}
			}
			else{ // 如果该点在点阵中是0，就用背景色描绘该像素
				// 适配屏幕分辨率！
				if (iBitsPerPixel == 32){
					// 如果是32位色，则一个像素用32位描绘
					if (iBackground>=0 && iBackground<=0x00ffffff){ //如果是合法色值则绘制背景，若为非法色值，绘制透明背景
						*(lpFramebuffer + idx + 0) = (char)((iBackground&0x000000ff));
						*(lpFramebuffer + idx + 1) = (char)((iBackground&0x0000ff00)>>8);
						*(lpFramebuffer + idx + 2) = (char)((iBackground&0x00ff0000)>>16);
						*(lpFramebuffer + idx + 3) = (char)(0x00);	
					}
				}
				else if (iBitsPerPixel == 24){
					// 如果是24位色，则一个像素用24位描绘
					if (iBackground>=0 && iBackground<=0x00ffffff){ //如果是合法色值则绘制背景，若为非法色值，绘制透明背景
						*(lpFramebuffer + idx + 0) = (char)((iBackground&0x000000ff));
						*(lpFramebuffer + idx + 1) = (char)((iBackground&0x0000ff00)>>8);
						*(lpFramebuffer + idx + 2) = (char)((iBackground&0x00ff0000)>>16);
					}		
				}
				else if(iBitsPerPixel == 16){
					// 如果是16位色，则一个像素用16位描绘
					if (iBackground>=0 && iBackground<=0x0000ffff){ //如果是合法色值则绘制背景，若为非法色值，绘制透明背景
						*(lpFramebuffer + idx + 0) = (char)((iBackground&0x000000ff));
						*(lpFramebuffer + idx + 1) = (char)((iBackground&0x0000ff00)>>8);
					}
				}
				else if(iBitsPerPixel == 8){
					// 如果是8位色，则一个像素用8位描绘
					if (iBackground>=0 && iBackground<=0x000000ff){ //如果是合法色值则绘制背景，若为非法色值，绘制透明背景
						*(lpFramebuffer + idx + 0) = (char)(iBackground);
					}
				}
			}
		}
	}
	// 返回字符的宽度。
	return iCharacterWidth;
}
