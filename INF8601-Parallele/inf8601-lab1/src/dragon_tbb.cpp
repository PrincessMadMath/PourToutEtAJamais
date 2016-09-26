/*
 * dragon_tbb.c
 *
 *  Created on: 2011-08-17
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <iostream>

extern "C" {
#include "dragon.h"
#include "color.h"
#include "utils.h"
}
#include "dragon_tbb.h"
#include "tbb/tbb.h"
#include "TidMap.h"

using namespace std;
using namespace tbb;

class DragonLimits {
   
public: 
    piece_t* piece; 
    
    DragonLimits(){
    }
    
    DragonLimits(DragonLimits& dragonLimits, split){
        piece_init(piece);
    }
    
    void operator()(const blocked_range<uint64_t>& range){
        piece_limit(range.begin(), range.end(), piece);        
    }
    
    void join(DragonLimits& otherLimit)
    {
        piece_merge(piece, *(otherLimit.piece));
    }
};


class DragonDraw {
private:
    draw_data info;
    int id = 0;
    
public:
    
    DragonDraw(draw_data drawData){
        info = drawData;
    }
    
    DragonDraw(const DragonDraw& dragonDraw){
        info = dragonDraw.info;
        id = dragonDraw.id + 1;
    }
    
    void operator()(const blocked_range<int>& range) const{
        dragon_draw_raw(range.begin(), range.end(), info.dragon, info.dragon_width, info.dragon_height, info.limits, 0);  
    }    
};


class DragonRender {
private:
    draw_data info;
    
public: 
    
    DragonRender(draw_data drawData){
        info = drawData;
    }
    
    DragonRender(const DragonRender& dragonRender){
        info = dragonRender.info;
    }
    
    void operator()(const blocked_range<int>& range) const{
        scale_dragon(range.begin(), range.end(), info.image, info.image_width, info.image_height, info.dragon, info.dragon_width, info.dragon_height, info.palette);
    }
    
};

class DragonClear {
public:
    char* dragon;
    
    DragonClear(char* p_dragon){
        dragon = p_dragon;
    }
    
    DragonClear(const DragonClear& dragonClear){
        dragon = dragonClear.dragon;
    }
    
    void operator()(const blocked_range<int>& range) const{
            init_canvas(range.begin(), range.end(), dragon, -1);
    }
};


int dragon_draw_tbb(char **canvas, struct rgb *image, int width, int height, uint64_t size, int nb_thread)
{
	struct draw_data data;
	limits_t limits;
	char *dragon = NULL;
	int dragon_width;
	int dragon_height;
	int dragon_surface;
	int scale_x;
	int scale_y;
	int scale;
	int deltaJ;
	int deltaI;

	struct palette *palette = init_palette(nb_thread);
	if (palette == NULL)
		return -1;

	/* 1. Calculer les limites du dragon */
	dragon_limits_tbb(&limits, size, nb_thread);


	dragon_width = limits.maximums.x - limits.minimums.x;
	dragon_height = limits.maximums.y - limits.minimums.y;
	dragon_surface = dragon_width * dragon_height;
	scale_x = dragon_width / width + 1;
	scale_y = dragon_height / height + 1;
	scale = (scale_x > scale_y ? scale_x : scale_y);
	deltaJ = (scale * width - dragon_width) / 2;
	deltaI = (scale * height - dragon_height) / 2;

	dragon = (char *) malloc(dragon_surface);
	if (dragon == NULL) {
		free_palette(palette);
		return -1;
	}

	data.nb_thread = nb_thread;
	data.dragon = dragon;
	data.image = image;
	data.size = size;
	data.image_height = height;
	data.image_width = width;
	data.dragon_width = dragon_width;
	data.dragon_height = dragon_height;
	data.limits = limits;
	data.scale = scale;
	data.deltaI = deltaI;
	data.deltaJ = deltaJ;
	data.palette = palette;
	data.tid = (int *) calloc(nb_thread, sizeof(int));
	
	task_scheduler_init task(nb_thread);
	
	/* 2. Initialiser la surface : DragonClear */
    DragonClear dragonClear = DragonClear(dragon);
    parallel_for(blocked_range<int>(0, dragon_surface), dragonClear);

	/* 3. Dessiner le dragon : DragonDraw */
    DragonDraw dragonDraw = DragonDraw(data);
    parallel_for(blocked_range<int>(0, size-1), dragonDraw);
   
	/* 4. Effectuer le rendu final */
    DragonRender dragonRender = DragonRender(data);
    parallel_for(blocked_range<int>(0, height), dragonRender);

	init.terminate();

	free_palette(palette);
	FREE(data.tid);
	//*canvas = dragon;
	*canvas = NULL;
	return 0;
}

/*
 * Calcule les limites en terme de largeur et de hauteur de
 * la forme du dragon. Requis pour allouer la matrice de dessin.
 */
int dragon_limits_tbb(limits_t *limits, uint64_t size, int nb_thread)
{
	
	DragonLimits lim;
		
	piece_t piece;
	piece_init(&piece);
	
    lim.piece = &piece;
    
    task_scheduler_init task(nb_thread);
    parallel_reduce(blocked_range<uint64_t>(0, size), lim);
		
	*limits = lim.piece->limits;
    
	return 0;
}
