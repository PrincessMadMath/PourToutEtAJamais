/*
 * bilinear_kernel.cl
 *
 *  Created on: 2011-10-14
 *      Author: francis
 */

/*
 * FIXME: Determinez les arguments a passer au noyau
 *        Le code de la fonction value_color doit etre recopie dans ce fichier et adapte
 *
 *        Pour traiter des type de donnees char, le pragma suivant doit etre utilise:
 *        #pragma OPENCL EXTENSION cl_khr_byte_addressable_store: enable
 */

#ifndef M_PI
#define M_PI 3.14159265358979323846264338328
#endif

typedef struct sinoscope sinoscope_t;

struct sinoscope {
	unsigned char *buf;
	int width;
	int height;
	int interval;
	int taylor;
	float interval_inv;
	float time;
	float max;
	float phase0;
	float phase1;
	float dx;
	float dy;
};

struct rgb {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

__constant struct rgb black = { .r = 0, .g = 0, .b = 0 };
__constant struct rgb white = { .r = 255, .g = 255, .b = 255 };

void value_color(struct rgb *color, float value, int interval, float interval_inv)
{
	#pragma OPENCL EXTENSION cl_khr_byte_addressable_store: enable
	if (isnan(value)) {
		*color = black;
		return;
	}
	struct rgb c;
	int x = (((int)value % interval) * 255) * interval_inv;
	int i = value * interval_inv;
	switch(i) {
	case 0:
		c.r = 0;
		c.g = x;
		c.b = 255;
		break;
	case 1:
		c.r = 0;
		c.g = 255;
		c.b = 255 - x;
		break;
	case 2:
		c.r = x;
		c.g = 255;
		c.b = 0;
		break;
	case 3:
		c.r = 255;
		c.g = 255 - x;
		c.b = 0;
		break;
	case 4:
		c.r = 255;
		c.g = 0;
		c.b = x;
		break;
	default:
		c = white;
		break;
	}
	*color = c;
}


__kernel void sinoscope_kernel(__global unsigned char *output, const int width, const int interval, const int taylor, const float interval_inv, const float time, const float phase0, const float phase1, const float dx, const float dy)
{
	int x = get_global_id(0);
	int y = get_global_id(1);

	float px = dx * y - 2 * M_PI;
	float py = dy * x - 2 * M_PI;
	
	// Calcul de la valeur
	float val = 0.0f;
	for (int taylor_increment = 1; taylor_increment <= taylor; taylor_increment += 2) {
		val += sin(px * taylor_increment * phase1 + time) / taylor_increment + cos(py * taylor * phase0) / taylor_increment;
	}
	val = (atan(1.0 * val) - atan(-1.0 * val)) / (M_PI);
	val = (val + 1) * 100;
		
	// Calcul de l'index du pixel a colorié
	int index = (y * 3) + (x * 3) * width;
	
	// Calcul couleur resultant du calcul
	struct rgb c;
	value_color(&c, val, interval, interval_inv);
	
	// Écrit dans output pour que l'hote soit capable de le lire
	output[index + 0] = c.r;
	output[index + 1] = c.g;
	output[index + 2] = c.b;	
}

