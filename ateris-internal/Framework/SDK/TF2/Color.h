#pragma once
class Color
{
public:
	Color( int red = 0, int green = 0, int blue = 0, int alpha = 255 )
	{
		r = red, g = green, b = blue, a = alpha;
	}

	unsigned char r, g, b, a;
};