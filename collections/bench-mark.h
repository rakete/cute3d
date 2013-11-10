/*
 * Copyright (C) 2010 by Joseph A. Marrero.  http://www.manvscode.com/
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef _BENCH_MARK_H_
#define _BENCH_MARK_H_


#ifdef __cplusplus
extern "C" {
#endif 

struct lc_bench_mark;
typedef struct lc_bench_mark* lc_bench_mark_t;

lc_bench_mark_t bench_mark_create  ( const char *description );
void            bench_mark_destroy ( lc_bench_mark_t bm );
void            bench_mark_start   ( lc_bench_mark_t bm );
void            bench_mark_end     ( lc_bench_mark_t bm );
void            bench_mark_report  ( lc_bench_mark_t bm );


#ifdef __cplusplus
}
#endif 
#endif /* _BENCH_MARK_H_ */
