/*
 * dragon_pthread.c
 *
 *  Created on: 2011-08-17
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "dragon.h"
#include "color.h"
#include "dragon_pthread.h"
#include "utils.h"

pthread_mutex_t mutex_stdout;

void printf_threadsafe(char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	pthread_mutex_lock(&mutex_stdout);
	vprintf(format, ap);
	pthread_mutex_unlock(&mutex_stdout);
	va_end(ap);
}

void *dragon_draw_worker(void *data)
{
    struct draw_data *info = (struct draw_data *) data;
    
    clock_t start_time = clock();
    
	/* 1. Initialiser la surface */
    
    int areaChunk = (info->dragon_width * info->dragon_height) / info->nb_thread;
    init_canvas(areaChunk*info->id, areaChunk*(info->id + 1), info->dragon, -1);
    
    pthread_barrier_wait(info->barrier);
    
	/* 2. Dessiner le dragon */
    uint64_t start = info->id * info->size / info->nb_thread;
    uint64_t end = (info->id + 1) * info->size / info->nb_thread;
    dragon_draw_raw(start, end, info->dragon, info->dragon_width, info->dragon_height, info->limits, info->id);
    
    
    pthread_barrier_wait(info->barrier);
    
	/* 3. Effectuer le rendu final */
    
    // Scale dragon to fit the final image
    int heightChunk = (info->image_height) / info->nb_thread;
    
	scale_dragon(heightChunk * info->id, heightChunk * (info->id + 1), info->image, info->image_width, info->image_height, info->dragon, info->dragon_width, info->dragon_height, info->palette);
    
    printf_threadsafe("Thread %d: %d ms\n",  gettid(), (clock() - start_time)*1000/CLOCKS_PER_SEC);
    
	return NULL;
}

int dragon_draw_pthread(char **canvas, struct rgb *image, int width, int height, uint64_t size, int nb_thread)
{
	pthread_t *threads = NULL;
	pthread_barrier_t barrier;
	limits_t lim;
	struct draw_data info;
	char *dragon = NULL;
	int scale_x;
	int scale_y;
	struct draw_data *data = NULL;
	struct palette *palette = NULL;
	int ret = 0;

	palette = init_palette(nb_thread);
	if (palette == NULL)
		goto err;

	/* 1. Initialiser barrier. */
    pthread_barrier_init(&barrier, NULL, nb_thread);

	if (dragon_limits_pthread(&lim, size, nb_thread) < 0)
		goto err;

	info.dragon_width = lim.maximums.x - lim.minimums.x;
	info.dragon_height = lim.maximums.y - lim.minimums.y;

	if ((dragon = (char *) malloc(info.dragon_width * info.dragon_height)) == NULL) {
		printf("malloc error dragon\n");
		goto err;
	}

	if ((data = malloc(sizeof(struct draw_data) * nb_thread)) == NULL) {
		printf("malloc error data\n");
		goto err;
	}

	if ((threads = malloc(sizeof(pthread_t) * nb_thread)) == NULL) {
		printf("malloc error threads\n");
		goto err;
	}

	info.image_height = height;
	info.image_width = width;
	scale_x = info.dragon_width / width + 1;
	scale_y = info.dragon_height / height + 1;
	info.scale = (scale_x > scale_y ? scale_x : scale_y);
	info.deltaJ = (info.scale * width - info.dragon_width) / 2;
	info.deltaI = (info.scale * height - info.dragon_height) / 2;
	info.nb_thread = nb_thread;
	info.dragon = dragon;
	info.image = image;
	info.size = size;
	info.limits = lim;
	info.barrier = &barrier;
	info.palette = palette;
	info.dragon = dragon;
	info.image = image;

	/* 2. Lancement du calcul parallèle principal avec draw_dragon_worker */
	

    
    for(int draw_index = 0; draw_index < nb_thread; ++draw_index)
    {
        info.id = draw_index;
        data[draw_index] = info;
        
        pthread_create(&threads[draw_index], NULL, dragon_draw_worker, (void*)&data[draw_index]);
    }

	/* 3. Attendre la fin du traitement */
    for(int draw_index = 0; draw_index < nb_thread; ++draw_index)
    {
        pthread_join(threads[draw_index], NULL);
    }
    
    
	/* 4. Destruction des variables. */ 
    pthread_barrier_destroy(&barrier);

done:
	FREE(data);
	FREE(threads);
	free_palette(palette);
	*canvas = dragon;
	return ret;

err:
	FREE(dragon);
	ret = -1;
	goto done;
}

void *dragon_limit_worker(void *data)
{
	struct limit_data *lim = (struct limit_data *) data;
	piece_limit(lim->start, lim->end, &lim->piece);
	return NULL;
}

/*
 * Calcule les limites en terme de largeur et de hauteur de
 * la forme du dragon. Requis pour allouer la matrice de dessin.
 */
int dragon_limits_pthread(limits_t *limits, uint64_t size, int nb_thread)
{
	int ret = 0;
	pthread_t *threads = NULL;
	struct limit_data *thread_data = NULL;
	piece_t master;

	piece_init(&master);

	/* 1. ALlouer de l'espace pour threads et threads_data. */
    
    threads = malloc(nb_thread * sizeof(pthread_t));
    thread_data = malloc(nb_thread * sizeof(struct limit_data));
    
	/* 2. Lancement du calcul en parallèle avec dragon_limit_worker. */
    
    uint64_t chunkSize;
    uint64_t leftOver;
    if(nb_thread == 1){
        chunkSize = 0;
        leftOver = size;
    }
    else{
        chunkSize = size / (nb_thread);
        leftOver = size % (nb_thread) + chunkSize;
    }
    
    for(int threadIndex = 0; threadIndex < nb_thread; ++ threadIndex){
        
        uint64_t start = threadIndex * chunkSize;
        uint64_t end = start + chunkSize;
        
        if(threadIndex == (nb_thread-1)){
            end += leftOver;
        }
        struct limit_data temp = {threadIndex, start, end, master};
        thread_data[threadIndex] = temp;
        
        pthread_create(&threads[threadIndex], NULL, dragon_limit_worker, (void*)&thread_data[threadIndex]);
    }
    
	/* 3. Attendre la fin du traitement. */
    
    for(int threadIndex = 0; threadIndex < nb_thread; ++ threadIndex){
        pthread_join(threads[threadIndex], NULL);
        
        if(threadIndex == 0){
            master = thread_data[0].piece;
        }
        else{
            piece_merge(&master, thread_data[threadIndex].piece);
        }
    }

	FREE(threads);
	FREE(thread_data);
	*limits = master.limits;
	return ret;
}
