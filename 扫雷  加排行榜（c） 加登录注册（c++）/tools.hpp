#include<graphics.h>
#pragma once
typedef struct _ARGB
{
	byte a;
	byte r;
	byte g;
	byte b;
}ARGB;
ARGB color2Argb(DWORD c)
{
	ARGB res;
	res.r = (byte)c;
	res.g = (byte)(c >> 8);
	res.b = (byte)(c >> 16);
	res.a = (byte)(c >> 24);
	return res;
}
DWORD argb2Color(ARGB c)
{
	DWORD t = RGB(c.r, c.g, c.b);
	return((DWORD)c.a) << 24 | t;
}
void toGray(IMAGE* src)
{
	DWORD* psrc = GetImageBuffer(src);
	for (int i = 0; i < src->getwidth() * src->getheight(); i++)
	{
		ARGB t = color2Argb(psrc[i]);
		byte arv = (t.r + t.g + t.b) / 3;
		ARGB res = { t.a,arv,arv,arv };
		psrc[i] = argb2Color(res);
	}
}
void drawImg(int x, int y, IMAGE* src)
{
	DWORD* pwin = GetImageBuffer();
	DWORD* psrc = GetImageBuffer(src);
	int win_w = getwidth();
	int win_h = getheight();
	int src_w = src->getwidth();
	int src_h = src->getheight();

	int real_w = (x + src_w > win_w) ? win_w - x : src_w;
	int real_h = (x + src_h > win_h) ? win_h - x : src_h;
	if (x < 0) { psrc += -x;  real_w -= -x;  x = 0; }
	if (y < 0) { psrc += (src_w * -y);  real_h -= -y; y = 0; }

	pwin += (win_w * y + x);

	for (int iy = 0; iy < real_h; iy++)
	{
		for (int ix = 0; ix < real_w; ix++)
		{
			byte a = (byte)(psrc[ix] >> 24);
			if (a > 100)
			{
				pwin[ix] = psrc[ix];
			}
		}
		pwin += win_w;
		psrc += src_w;
	}
}