/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/*
 * Please fill in the following team struct
 */
team_t team = {
    "sours", /* Team name */

    "Ido Zeira",         /* First member full name */
    "ido.ze2@gmail.com", /* First member email address */

    "Allen Bronshtein",          /* Second member full name (leave blank if none) */
    "allen.bronshtein@gmail.com" /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/*
 * naive_rotate - The naive baseline version of rotate
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}

/*
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (j = 0; j < dim; j++)
        for (i = 0; i < dim; i++)
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_rotate_functions()
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate, rotate_descr);
    /* ... Register additional test functions here */
}

/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct
{
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/*
 * initialize_pixel_sum - Initializes all fields of sum to 0
 */
static void initialize_pixel_sum(pixel_sum *sum)
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/*
 * accumulate_sum - Accumulates field values of p in corresponding
 * fields of sum
 */
static void accumulate_sum(pixel_sum *sum, pixel p)
{
    sum->red += (int)p.red;
    sum->green += (int)p.green;
    sum->blue += (int)p.blue;
    sum->num++;
    return;
}

/*
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum)
{
    current_pixel->red = (unsigned short)(sum.red / sum.num);
    current_pixel->green = (unsigned short)(sum.green / sum.num);
    current_pixel->blue = (unsigned short)(sum.blue / sum.num);
    return;
}

/*
 * avg - Returns averaged pixel value at (i,j)
 */
static pixel avg(int dim, int i, int j, pixel *src)
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for (ii = max(i - 1, 0); ii <= min(i + 1, dim - 1); ii++)
        for (jj = max(j - 1, 0); jj <= min(j + 1, dim - 1); jj++)
            accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

char smooth5_descr[] = "smooth5: Optimized version by Ido";
void smooth5(int dim, pixel *src, pixel *dst)
{
    int rid; // RDIX-format location in the matrix.
    pixel_sum sum;

    //Four corners   ---------------------------------
    //(0 , 0)
    // rd = 0;
    initialize_pixel_sum(&sum);
    accumulate_sum(&sum, src[0]);       //(0 , 0)
    accumulate_sum(&sum, src[1]);       //(0 , 1)
    accumulate_sum(&sum, src[dim]);     //(1 , 0)
    accumulate_sum(&sum, src[dim + 1]); //(1 , 1)
    assign_sum_to_pixel(dst, sum);

    //(0 , dim - 1)
    // rd = RIDX(0, dim - 1, dim);
    rid = dim - 1;
    initialize_pixel_sum(&sum);
    accumulate_sum(&sum, src[rid]);           //(0 , dim - 1)
    accumulate_sum(&sum, src[rid - 1]);       //(0 , dim - 2)
    accumulate_sum(&sum, src[rid + dim]);     //(1 , dim - 1)
    accumulate_sum(&sum, src[rid + dim - 1]); //(1 , dim - 2)
    assign_sum_to_pixel(dst + rid, sum);

    // (dim - 1 , 0)
    // rd = RIDX(dim - 1, 0, dim);
    rid = (dim - 1) * dim;
    initialize_pixel_sum(&sum);
    accumulate_sum(&sum, src[rid]);           //(dim - 1 , 0)
    accumulate_sum(&sum, src[rid + 1]);       //(dim - 1 , 1)
    accumulate_sum(&sum, src[rid - dim]);     //(dim - 2 , 0)
    accumulate_sum(&sum, src[rid - dim + 1]); //(dim - 2 , 1)
    assign_sum_to_pixel(dst + rid, sum);

    //(dim - 1 , dim - 1)
    // rd = RDIX(dim - 1, dim - 1, dim);
    rid = dim * dim - 1;
    initialize_pixel_sum(&sum);
    accumulate_sum(&sum, src[rid]);           //(dim - 1 , dim - 1)
    accumulate_sum(&sum, src[rid - 1]);       //(dim - 1 , dim - 2)
    accumulate_sum(&sum, src[rid - dim]);     //(dim - 2 , dim - 1)
    accumulate_sum(&sum, src[rid - dim - 1]); //(dim - 2 , dim - 2)
    assign_sum_to_pixel(dst + rid, sum);

    //(0 , 1) ... (0, dim-2) [Left Column]
    for (rid = 1; rid < dim - 1; rid++)
    {
        initialize_pixel_sum(&sum);
        accumulate_sum(&sum, src[rid - 1]);       // (x , y - 1)
        accumulate_sum(&sum, src[rid]);           // (x , y)
        accumulate_sum(&sum, src[rid + 1]);       // (x , y + 1)
        accumulate_sum(&sum, src[rid + dim - 1]); // (x + 1 , y - 1)
        accumulate_sum(&sum, src[rid + dim]);     // (x + 1 , y)
        accumulate_sum(&sum, src[rid + dim + 1]); // (x + 1 , y + 1)
        assign_sum_to_pixel(dst + rid, sum);
    }

    //(dim - 1 , 1) ... (dim - 1, dim-2) [Right Column]
    for (rid = RIDX(dim - 1, 1, dim); rid < (dim * dim) - 1; rid++)
    {
        initialize_pixel_sum(&sum);
        accumulate_sum(&sum, src[rid - 1]);       // (x , y - 1)
        accumulate_sum(&sum, src[rid]);           // (x , y)
        accumulate_sum(&sum, src[rid + 1]);       // (x , y + 1)
        accumulate_sum(&sum, src[rid - dim - 1]); // (x - 1 , y - 1)
        accumulate_sum(&sum, src[rid - dim]);     // (x - 1 , y)
        accumulate_sum(&sum, src[rid - dim + 1]); // (x - 1 , y + 1)
        assign_sum_to_pixel(dst + rid, sum);
    }

    //(1 , 0) ... (dim - 2, 0) [Upper Row]
    for (rid = dim; rid < (dim - 1) * dim; rid += dim)
    {
        initialize_pixel_sum(&sum);
        accumulate_sum(&sum, src[rid + dim]);     // (x + 1 , y)
        accumulate_sum(&sum, src[rid]);           // (x , y)
        accumulate_sum(&sum, src[rid - dim]);     // (x - 1 , y)
        accumulate_sum(&sum, src[rid + dim + 1]); // (x + 1 , y + 1)
        accumulate_sum(&sum, src[rid + 1]);       // (x , y + 1)
        accumulate_sum(&sum, src[rid - dim + 1]); // (x - 1 , y + 1)
        assign_sum_to_pixel(dst + rid, sum);
    }

    //(1 , dim - 1) ... (dim - 2, dim - 1) [Lower Row]
    for (rid = RIDX(1, dim - 1, dim); rid < (dim * dim) - 1; rid += dim)
    {
        initialize_pixel_sum(&sum);
        accumulate_sum(&sum, src[rid + dim]);     // (x + 1 , y)
        accumulate_sum(&sum, src[rid]);           // (x , y)
        accumulate_sum(&sum, src[rid - dim]);     // (x - 1 , y)
        accumulate_sum(&sum, src[rid + dim - 1]); // (x + 1 , y - 1)
        accumulate_sum(&sum, src[rid - 1]);       // (x , y - 1)
        accumulate_sum(&sum, src[rid - dim - 1]); // (x - 1 , y - 1)
        assign_sum_to_pixel(dst + rid, sum);
    }

    //Center   ------------------------------------------------------------------
    int i, j;
    for (i = 1; i < dim - 1; i++)
        for (j = 1; j < dim - 1; j++)
        {
            rid = RIDX(i, j, dim);
            initialize_pixel_sum(&sum);
            accumulate_sum(&sum, src[rid + dim]);     // (x + 1 , y)        right neighbour
            accumulate_sum(&sum, src[rid]);           // (x , y)            current pixel
            accumulate_sum(&sum, src[rid - dim]);     // (x - 1 , y)        left neighbour
            accumulate_sum(&sum, src[rid + dim - 1]); // (x + 1 , y - 1)    upper-right neighbour
            accumulate_sum(&sum, src[rid - 1]);       // (x , y - 1)        upper neighbour
            accumulate_sum(&sum, src[rid - dim - 1]); // (x - 1 , y - 1)    upper-left neighbour
            accumulate_sum(&sum, src[rid + dim + 1]); // (x + 1 , y + 1)    lower-right neighbour
            accumulate_sum(&sum, src[rid + 1]);       // (x , y + 1)        lower neighbour
            accumulate_sum(&sum, src[rid - dim + 1]); // (x - 1 , y + 1)    lower-left neighbour
            assign_sum_to_pixel(dst + rid, sum);
        }
}

/*
 * smooth - Your current working version of smooth.
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst)
{
    smooth5(dim, src, dst);
}

/*********************************************************************
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_smooth_functions()
{
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}
