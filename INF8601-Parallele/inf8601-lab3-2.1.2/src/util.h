/*
 * util.h
 *
 *  Created on: 2011-10-07
 *      Author: francis
 */

#ifndef UTIL_H_
#define UTIL_H_

/**
 * TODO:
 *
 * macro to flag unimplemented blocks
 */
#define TODO(msg) 								\
    fprintf(stderr, "%s:%d Unimplemented block %s\n",			\
            __FILE__, __LINE__, msg);

#define ERR_THROW(val, err, msg)             \
    do {                                                \
        if (val != err) {                                     \
        	fprintf(stderr, "%s:%d %d != %d %s\n", __FILE__, __LINE__, val, err, msg); \
            goto error;                                 \
        }                                               \
    } while(0)

#define ERR_ASSERT(cond, msg)                                           \
    if (!(cond)) {                                                   \
        fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, msg); \
        goto error;                                                  \
    }

#define ERR_NOMEM(ptr)                             \
    if (ptr == NULL) {                                          \
        fprintf(stderr, "ENOMEM\n");    \
        goto error;                                      \
    }

#endif /* UTIL_H_ */
