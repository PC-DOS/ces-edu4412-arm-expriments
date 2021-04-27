# LCD打印点阵文字

向Framebuffer中打印点阵文本。是用来自Linux内核的点阵字体。

用法：

```
Prints text to LCD.

Usage:
     PrintToLCD string [xpos=0] [ypos=0] [clearscreen=1] [overflow=1] [foreground=16777215] [background=-1]

     'clearscreen' indicates if we will clear the whole screen before printing.
     'foreground' is the color of the printed text, in (r,g,b) sequence.
     'background' is the background color of the printed text, in (r,g,b) sequence. Set it to -1 to use transparent background.
     'overflow' indicate the overflow operation. 0 - no operation (stops the program); 1 - break and start printing from xpos; 2 - break and start printing from 0.
```
