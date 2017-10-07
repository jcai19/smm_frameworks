/* {{{ Copyright etc. */

/**********************************************************************\

  SUSAN Version 2l by Stephen Smith
  Oxford Centre for Functional Magnetic Resonance Imaging of the Brain,
  Department of Clinical Neurology, Oxford University, Oxford, UK
  (Previously in Computer Vision and Image Processing Group - now
  Computer Vision and Electro Optics Group - DERA Chertsey, UK)
Email:    steve@fmrib.ox.ac.uk
WWW:      http://www.fmrib.ox.ac.uk/~steve

(C) Crown Copyright (1995-1999), Defence Evaluation and Research Agency,
Farnborough, Hampshire, GU14 6TD, UK
DERA WWW site:
http://www.dera.gov.uk/
DERA Computer Vision and Electro Optics Group WWW site:
http://www.dera.gov.uk/imageprocessing/dera/group_home.html
DERA Computer Vision and Electro Optics Group point of contact:
Dr. John Savage, jtsavage@dera.gov.uk, +44 1344 633203

A UK patent has been granted: "Method for digitally processing
images to determine the position of edges and/or corners therein for
guidance of unmanned vehicle", UK Patent 2272285. Proprietor:
Secretary of State for Defence, UK. 15 January 1997

This code is issued for research purposes only and remains the
property of the UK Secretary of State for Defence. This code must
not be passed on without this header information being kept
intact. This code must not be sold.

\**********************************************************************/

/* }}} */
/* {{{ Readme First */

/**********************************************************************\

  SUSAN Version 2l
  SUSAN = Smallest Univalue Segment Assimilating Nucleus

Email:    steve@fmrib.ox.ac.uk
WWW:      http://www.fmrib.ox.ac.uk/~steve

Related paper:
@article{Smith97,
author = "Smith, S.M. and Brady, J.M.",
title = "{SUSAN} - A New Approach to Low Level Image Processing",
journal = "Int. Journal of Computer Vision",
pages = "45--78",
volume = "23",
number = "1",
month = "May",
year = 1997}

To be registered for automatic (bug) updates of SUSAN, send an email.

Compile with:
gcc -O4 -o susan susan2l.c -lm

See following section for different machine information. Please
report any bugs (and fixes). There are a few optional changes that
can be made in the "defines" section which follows shortly.

Usage: type "susan" to get usage. Only PGM format files can be input
and output. Utilities such as the netpbm package and XV can be used
to convert to and from other formats. Any size of image can be
processed.

This code is written using an emacs folding mode, making moving
around the different sections very easy. This is why there are
various marks within comments and why comments are indented.


SUSAN QUICK:

This version of the SUSAN corner finder does not do all the
false-corner suppression and thus is faster and produced some false
positives, particularly on strong edges. However, because there are
less stages involving thresholds etc., the corners that are
correctly reported are usually more stable than those reported with
the full algorithm. Thus I recommend at least TRYING this algorithm
for applications where stability is important, e.g., tracking.

THRESHOLDS:

There are two thresholds which can be set at run-time. These are the
brightness threshold (t) and the distance threshold (d).

SPATIAL CONTROL: d

In SUSAN smoothing d controls the size of the Gaussian mask; its
default is 4.0. Increasing d gives more smoothing. In edge finding,
a fixed flat mask is used, either 37 pixels arranged in a "circle"
(default), or a 3 by 3 mask which gives finer detail. In corner
finding, only the larger 37 pixel mask is used; d is not
variable. In smoothing, the flat 3 by 3 mask can be used instead of
a larger Gaussian mask; this gives low smoothing and fast operation.

BRIGHTNESS CONTROL: t

In all three algorithms, t can be varied (default=20); this is the
main threshold to be varied. It determines the maximum difference in
greylevels between two pixels which allows them to be considered
part of the same "region" in the image. Thus it can be reduced to
give more edges or corners, i.e. to be more sensitive, and vice
versa. In smoothing, reducing t gives less smoothing, and vice
versa. Set t=10 for the test image available from the SUSAN web
page.

ITERATIONS:

With SUSAN smoothing, more smoothing can also be obtained by
iterating the algorithm several times. This has a different effect
from varying d or t.

FIXED MASKS:

37 pixel mask:    ooo       3 by 3 mask:  ooo
ooooo                    ooo
ooooooo                   ooo
ooooooo
ooooooo
ooooo
ooo

CORNER ATTRIBUTES dx, dy and I
(Only read this if you are interested in the C implementation or in
 using corner attributes, e.g., for corner matching)

Corners reported in the corner list have attributes associated with
them as well as positions. This is useful, for example, when
attempting to match corners from one image to another, as these
attributes can often be fairly unchanged between images. The
attributes are dx, dy and I. I is the value of image brightness at
the position of the corner. In the case of susan_corners_quick, dx
and dy are the first order derivatives (differentials) of the image
brightness in the x and y directions respectively, at the position
of the corner. In the case of normal susan corner finding, dx and dy
are scaled versions of the position of the centre of gravity of the
USAN with respect to the centre pixel (nucleus).

BRIGHTNESS FUNCTION LUT IMPLEMENTATION:
(Only read this if you are interested in the C implementation)

    The SUSAN brightness function is implemented as a LUT
    (Look-Up-Table) for speed. The resulting pointer-based code is a
    little hard to follow, so here is a brief explanation. In
    setup_brightness_lut() the LUT is setup. This mallocs enough space
    for *bp and then repositions the pointer to the centre of the
    malloced space. The SUSAN function e^-(x^6) or e^-(x^2) is
    calculated and converted to a uchar in the range 0-100, for all
    possible image brightness differences (including negative
	    ones). Thus bp[23] is the output for a brightness difference of 23
    greylevels. In the SUSAN algorithms this LUT is used as follows:

    p=in + (i-3)*x_size + j - 1;
    p points to the first image pixel in the circular mask surrounding
    point (x,y).

    cp=bp + in[i*x_size+j];
    cp points to a position in the LUT corresponding to the brightness
    of the centre pixel (x,y).

    now for every pixel within the mask surrounding (x,y),
    n+=*(cp-*p++);
the brightness difference function is found by moving the cp pointer
down by an amount equal to the value of the pixel pointed to by p,
     thus subtracting the two brightness values and performing the
     exponential function. This value is added to n, the running USAN
     area.

     in SUSAN smoothing, the variable height mask is implemented by
     multiplying the above by the moving mask pointer, reset for each new
     centre pixel.
     tmp = *dpt++ * *(cp-brightness);

     \**********************************************************************/

     /* }}} */
     /* {{{ Machine Information */

     /**********************************************************************\

       Success has been reported with the following:

       MACHINE  OS         COMPILER

       Sun      4.1.4      bundled C, gcc

       Next

       SGI      IRIX       SGI cc

       DEC      Unix V3.2+ 

       IBM RISC AIX        gcc

       PC                  Borland 5.0

       PC       Linux      gcc-2.6.3

       PC       Win32      Visual C++ 4.0 (Console Application)

       PC       Win95      Visual C++ 5.0 (Console Application)
       Thanks to Niu Yongsheng <niuysbit@163.net>:
       Use the FOPENB option below

       PC       DOS        djgpp gnu C
       Thanks to Mark Pettovello <mpettove@umdsun2.umd.umich.edu>:
       Use the FOPENB option below

       HP       HP-UX      bundled cc
       Thanks to Brian Dixon <briand@hpcvsgen.cv.hp.com>:
       in ksh:
       export CCOPTS="-Aa -D_HPUX_SOURCE | -lM"
       cc -O3 -o susan susan2l.c

       \**********************************************************************/

     /* }}} */
     /* {{{ History */

     /**********************************************************************\

       SUSAN Version 2l, 12/2/99
       Changed GNUDOS option to FOPENB.
       (Thanks to Niu Yongsheng <niuysbit@163.net>.)
       Took out redundant "sq=sq/2;".

       SUSAN Version 2k, 19/8/98:
       In corner finding:
       Changed if(yy<sq) {...} else if(xx<sq) {...} to
       if(yy<xx) {...} else {...}
       (Thanks to adq@cim.mcgill.edu - Alain Domercq.)

       SUSAN Version 2j, 22/10/97:
       Fixed (mask_size>x_size) etc. tests in smoothing.
       Added a couple of free() calls for cgx and cgy.
       (Thanks to geoffb@ucs.ed.ac.uk - Geoff Browitt.)

       SUSAN Version 2i, 21/7/97:
       Added information about corner attributes.

       SUSAN Version 2h, 16/12/96:
       Added principle (initial enhancement) option.

       SUSAN Version 2g, 2/7/96:
       Minor superficial changes to code.

       SUSAN Version 2f, 16/1/96:
       Added GNUDOS option (now called FOPENB; see options below).

       SUSAN Version 2e, 9/1/96:
       Added -b option.
       Fixed 1 pixel horizontal offset error for drawing edges.

       SUSAN Version 2d, 27/11/95:
       Fixed loading of certain PGM files in get_image (again!)

       SUSAN Version 2c, 22/11/95:
       Fixed loading of certain PGM files in get_image.
       (Thanks to qu@San-Jose.ate.slb.com - Gongyuan Qu.)

       SUSAN Version 2b, 9/11/95:
       removed "z==" error in edges routines.

       SUSAN Version 2a, 6/11/95:
       Removed a few unnecessary variable declarations.
       Added different machine information.
       Changed "header" in get_image to char.

       SUSAN Version 2, 1/11/95: first combined version able to take any
       image sizes.

       SUSAN "Versions 1", circa 1992: the various SUSAN algorithms were
       developed during my doctorate within different programs and for
       fixed image sizes. The algorithms themselves are virtually unaltered
       between "versions 1" and the combined program, version 2.

       \**********************************************************************/

     /* }}} */
     /* {{{ defines, includes and typedefs */

     /* ********** Optional settings */

#ifndef PPC
     typedef int        TOTAL_TYPE; /* this is faster for "int" but should be "float" for large d masks */
#else
     typedef float      TOTAL_TYPE; /* for my PowerPC accelerator only */
#endif

     /*#define FOPENB*/           /* uncomment if using djgpp gnu C for DOS or certain Win95 compilers */
#define SEVEN_SUPP           /* size for non-max corner suppression; SEVEN_SUPP or FIVE_SUPP */
#define MAX_CORNERS   15000  /* max corners per frame */

     /* ********** Leave the rest - but you may need to remove one or both of sys/file.h and malloc.h lines */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/file.h>    /* may want to remove this line */
#include <malloc.h>      /* may want to remove this line */
#include <assert.h>
#define  exit_error(IFB,IFC) { fprintf(stderr,IFB,IFC); exit(0); }
#define  FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )
     typedef  unsigned char uchar;
     typedef  struct {int x,y,info, dx, dy, I;} CORNER_LIST[MAX_CORNERS];


     /* }}} */
     /* {{{ usage() */

usage()
{
    printf("Usage: susan <in.pgm> <out.pgm> [options]\n\n");

    printf("-s : Smoothing mode (default)\n");
    printf("-e : Edges mode\n");
    printf("-c : Corners mode\n\n");

    printf("See source code for more information about setting the thresholds\n");
    printf("-t <thresh> : Brightness threshold, all modes (default=20)\n");
    printf("-d <thresh> : Distance threshold, smoothing mode, (default=4) (use next option instead for flat 3x3 mask)\n");
    printf("-3 : Use flat 3x3 mask, edges or smoothing mode\n");
    printf("-n : No post-processing on the binary edge map (runs much faster); edges mode\n");
    printf("-q : Use faster (and usually stabler) corner mode; edge-like corner suppression not carried out; corners mode\n");
    printf("-b : Mark corners/edges with single black points instead of black with white border; corners or edges mode\n");
    printf("-p : Output initial enhancement image only; corners or edges mode (default is edges mode)\n");

    printf("\nSUSAN Version 2l (C) 1995-1997 Stephen Smith, DRA UK. steve@fmrib.ox.ac.uk\n");

    exit(0);
}

/* }}} */
/* {{{ get_image(filename,in,x_size,y_size) */

/* {{{ int getint(fp) derived from XV */

int getint(fd)
    FILE *fd;
{
    int c, i;
    char dummy[10000];

    c = getc(fd);
    while (1) /* find next integer */
    {
	if (c=='#')    /* if we're at a comment, read to end of line */
	    fgets(dummy,9000,fd);
	if (c==EOF)
	    exit_error("Image %s not binary PGM.\n","is");
	if (c>='0' && c<='9')
	    break;   /* found what we were looking for */
	c = getc(fd);
    }

    /* we're at the start of a number, continue until we hit a non-number */
    i = 0;
    while (1) {
	i = (i*10) + (c - '0');
	c = getc(fd);
	if (c==EOF) return (i);
	if (c<'0' || c>'9') break;
    }

    return (i);
}

/* }}} */

void get_image(filename,in,x_size,y_size)
    char           filename[200];
    unsigned char  **in;
    int            *x_size, *y_size;
{
    FILE  *fd;
    char header [100];
    int  tmp;

#ifdef FOPENB
    if ((fd=fopen(filename,"rb")) == NULL)
#else
	if ((fd=fopen(filename,"r")) == NULL)
#endif
	    exit_error("Can't input image %s.\n",filename);

    /* {{{ read header */

    header[0]=fgetc(fd);
    header[1]=fgetc(fd);
    if(!(header[0]=='P' && header[1]=='5'))
	exit_error("Image %s does not have binary PGM header.\n",filename);

    *x_size = getint(fd);
    *y_size = getint(fd);
    tmp = getint(fd);

    /* }}} */

    *in = (uchar *) malloc(*x_size * *y_size);

    if (fread(*in,1,*x_size * *y_size,fd) == 0)
	exit_error("Image %s is wrong size.\n",filename);

    fclose(fd);
}

/* }}} */
/* {{{ put_image(filename,in,x_size,y_size) */

put_image(filename,in,x_size,y_size)
    char filename [100],
    *in;
int  x_size,
     y_size;
{
    FILE  *fd;

#ifdef FOPENB
    if ((fd=fopen(filename,"wb")) == NULL) 
#else
	if ((fd=fopen(filename,"w")) == NULL) 
#endif
	    exit_error("Can't output image%s.\n",filename);

    fprintf(fd,"P5\n");
    fprintf(fd,"%d %d\n",x_size,y_size);
    fprintf(fd,"255\n");

    if (fwrite(in,x_size*y_size,1,fd) != 1)
	exit_error("Can't write image %s.\n",filename);

    fclose(fd);
}

/* }}} */
/* {{{ int_to_uchar(r,in,size) */

int_to_uchar(r,in,size)
    uchar *in;
    int   *r, size;
{
    int i,
	max_r=r[0],
	min_r=r[0];

    for (i=0; i<size; i++)
    {
	if ( r[i] > max_r )
	    max_r=r[i];
	if ( r[i] < min_r )
	    min_r=r[i];
    }

    /*printf("min=%d max=%d\n",min_r,max_r);*/

    max_r-=min_r;

    for (i=0; i<size; i++)
	in[i] = (uchar)((int)((int)(r[i]-min_r)*255)/max_r);
}

/* }}} */
/* {{{ setup_brightness_lut(bp,thresh,form) */

void setup_brightness_lut(bp,thresh,form)
    uchar **bp;
    int   thresh, form;
{
    int   k;
    float temp;

    *bp=(unsigned char *)malloc(516);
    *bp=*bp+258;

    for(k=-256;k<257;k++)
    {
	temp=((float)k)/((float)thresh);
	temp=temp*temp;
	if (form==6)
	    temp=temp*temp*temp;
	temp=100.0*exp(-temp);
	*(*bp+k)= (uchar)temp;
    }
}

/* }}} */
/* {{{ corners */

/* {{{ corner_draw(in,corner_list,drawing_mode) */

corner_draw(in,corner_list,x_size,drawing_mode)
    uchar *in;
    CORNER_LIST corner_list;
    int x_size, drawing_mode;
{
    uchar *p;
    int   n=0;

    while(corner_list[n].info != 7)
    {
	if (drawing_mode==0)
	{
	    p = in + (corner_list[n].y-1)*x_size + corner_list[n].x - 1;
	    *p++=255; *p++=255; *p=255; p+=x_size-2;
	    *p++=255; *p++=0;   *p=255; p+=x_size-2;
	    *p++=255; *p++=255; *p=255;
	    n++;
	}
	else
	{
	    p = in + corner_list[n].y*x_size + corner_list[n].x;
	    *p=0;
	    n++;
	}
    }
}

/* }}} */
/* {{{ susan(in,r,sf,max_no,corner_list) */

susan_corners(in,r,bp,max_no,corner_list,x_size,y_size)
    uchar       *in, *bp;
    int         *r, max_no, x_size, y_size;
    CORNER_LIST corner_list;
{
    int   n,x,y,sq,xx,yy,
	  i,j,*cgx,*cgy;
    float divide;
    uchar c,*p,*cp;

    memset (r,0,x_size * y_size * sizeof(int));

    cgx=(int *)malloc(x_size*y_size*sizeof(int));
    cgy=(int *)malloc(x_size*y_size*sizeof(int));

    for (i=5;i<y_size-5;i++)
	for (j=5;j<x_size-5;j++) {
	    n=100;
	    p=in + (i-3)*x_size + j - 1;
	    cp=bp + in[i*x_size+j];

	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p);
	    p+=x_size-3; 

	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p);
	    p+=x_size-5;

	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p);
	    p+=x_size-6;

	    n+=*(cp-*p++);
	    n+=*(cp-*p++);
	    n+=*(cp-*p);
	    if (n<max_no){    /* do this test early and often ONLY to save wasted computation */
		p+=2;
		n+=*(cp-*p++);
		if (n<max_no){
		    n+=*(cp-*p++);
		    if (n<max_no){
			n+=*(cp-*p);
			if (n<max_no){
			    p+=x_size-6;

			    n+=*(cp-*p++);
			    if (n<max_no){
				n+=*(cp-*p++);
				if (n<max_no){
				    n+=*(cp-*p++);
				    if (n<max_no){
					n+=*(cp-*p++);
					if (n<max_no){
					    n+=*(cp-*p++);
					    if (n<max_no){
						n+=*(cp-*p++);
						if (n<max_no){
						    n+=*(cp-*p);
						    if (n<max_no){
							p+=x_size-5;

							n+=*(cp-*p++);
							if (n<max_no){
							    n+=*(cp-*p++);
							    if (n<max_no){
								n+=*(cp-*p++);
								if (n<max_no){
								    n+=*(cp-*p++);
								    if (n<max_no){
									n+=*(cp-*p);
									if (n<max_no){
									    p+=x_size-3;

									    n+=*(cp-*p++);
									    if (n<max_no){
										n+=*(cp-*p++);
										if (n<max_no){
										    n+=*(cp-*p);

										    if (n<max_no)
										    {
											x=0;y=0;
											p=in + (i-3)*x_size + j - 1;

											c=*(cp-*p++);x-=c;y-=3*c;
											c=*(cp-*p++);y-=3*c;
											c=*(cp-*p);x+=c;y-=3*c;
											p+=x_size-3; 

											c=*(cp-*p++);x-=2*c;y-=2*c;
											c=*(cp-*p++);x-=c;y-=2*c;
											c=*(cp-*p++);y-=2*c;
											c=*(cp-*p++);x+=c;y-=2*c;
											c=*(cp-*p);x+=2*c;y-=2*c;
											p+=x_size-5;

											c=*(cp-*p++);x-=3*c;y-=c;
											c=*(cp-*p++);x-=2*c;y-=c;
											c=*(cp-*p++);x-=c;y-=c;
											c=*(cp-*p++);y-=c;
											c=*(cp-*p++);x+=c;y-=c;
											c=*(cp-*p++);x+=2*c;y-=c;
											c=*(cp-*p);x+=3*c;y-=c;
											p+=x_size-6;

											c=*(cp-*p++);x-=3*c;
											c=*(cp-*p++);x-=2*c;
											c=*(cp-*p);x-=c;
											p+=2;
											c=*(cp-*p++);x+=c;
											c=*(cp-*p++);x+=2*c;
											c=*(cp-*p);x+=3*c;
											p+=x_size-6;

											c=*(cp-*p++);x-=3*c;y+=c;
											c=*(cp-*p++);x-=2*c;y+=c;
											c=*(cp-*p++);x-=c;y+=c;
											c=*(cp-*p++);y+=c;
											c=*(cp-*p++);x+=c;y+=c;
											c=*(cp-*p++);x+=2*c;y+=c;
											c=*(cp-*p);x+=3*c;y+=c;
											p+=x_size-5;

											c=*(cp-*p++);x-=2*c;y+=2*c;
											c=*(cp-*p++);x-=c;y+=2*c;
											c=*(cp-*p++);y+=2*c;
											c=*(cp-*p++);x+=c;y+=2*c;
											c=*(cp-*p);x+=2*c;y+=2*c;
											p+=x_size-3;

											c=*(cp-*p++);x-=c;y+=3*c;
											c=*(cp-*p++);y+=3*c;
											c=*(cp-*p);x+=c;y+=3*c;

											xx=x*x;
											yy=y*y;
											sq=xx+yy;
											if ( sq > ((n*n)/2) )
											{
											    if(yy<xx) {
												divide=(float)y/(float)abs(x);
												sq=abs(x)/x;
												sq=*(cp-in[(i+FTOI(divide))*x_size+j+sq]) +
												    *(cp-in[(i+FTOI(2*divide))*x_size+j+2*sq]) +
												    *(cp-in[(i+FTOI(3*divide))*x_size+j+3*sq]);}
											    else {
												divide=(float)x/(float)abs(y);
												sq=abs(y)/y;
												sq=*(cp-in[(i+sq)*x_size+j+FTOI(divide)]) +
												    *(cp-in[(i+2*sq)*x_size+j+FTOI(2*divide)]) +
												    *(cp-in[(i+3*sq)*x_size+j+FTOI(3*divide)]);}

											    if(sq>290){
												r[i*x_size+j] = max_no-n;
												cgx[i*x_size+j] = (51*x)/n;
												cgy[i*x_size+j] = (51*y)/n;}
											}
										    }
										}}}}}}}}}}}}}}}}}}}

    /* to locate the local maxima */
    n=0;
    for (i=5;i<y_size-5;i++)
	for (j=5;j<x_size-5;j++) {
	    x = r[i*x_size+j];
	    if (x>0)  {
		/* 5x5 mask */
#ifdef FIVE_SUPP
		if (
			(x>r[(i-1)*x_size+j+2]) &&
			(x>r[(i  )*x_size+j+1]) &&
			(x>r[(i  )*x_size+j+2]) &&
			(x>r[(i+1)*x_size+j-1]) &&
			(x>r[(i+1)*x_size+j  ]) &&
			(x>r[(i+1)*x_size+j+1]) &&
			(x>r[(i+1)*x_size+j+2]) &&
			(x>r[(i+2)*x_size+j-2]) &&
			(x>r[(i+2)*x_size+j-1]) &&
			(x>r[(i+2)*x_size+j  ]) &&
			(x>r[(i+2)*x_size+j+1]) &&
			(x>r[(i+2)*x_size+j+2]) &&
			(x>=r[(i-2)*x_size+j-2]) &&
			(x>=r[(i-2)*x_size+j-1]) &&
			(x>=r[(i-2)*x_size+j  ]) &&
			(x>=r[(i-2)*x_size+j+1]) &&
			(x>=r[(i-2)*x_size+j+2]) &&
			(x>=r[(i-1)*x_size+j-2]) &&
			(x>=r[(i-1)*x_size+j-1]) &&
			(x>=r[(i-1)*x_size+j  ]) &&
			(x>=r[(i-1)*x_size+j+1]) &&
			(x>=r[(i  )*x_size+j-2]) &&
			(x>=r[(i  )*x_size+j-1]) &&
			(x>=r[(i+1)*x_size+j-2]) )
#endif
#ifdef SEVEN_SUPP
			if ( 
				(x>r[(i-3)*x_size+j-3]) &&
				(x>r[(i-3)*x_size+j-2]) &&
				(x>r[(i-3)*x_size+j-1]) &&
				(x>r[(i-3)*x_size+j  ]) &&
				(x>r[(i-3)*x_size+j+1]) &&
				(x>r[(i-3)*x_size+j+2]) &&
				(x>r[(i-3)*x_size+j+3]) &&

				(x>r[(i-2)*x_size+j-3]) &&
				(x>r[(i-2)*x_size+j-2]) &&
				(x>r[(i-2)*x_size+j-1]) &&
				(x>r[(i-2)*x_size+j  ]) &&
				(x>r[(i-2)*x_size+j+1]) &&
				(x>r[(i-2)*x_size+j+2]) &&
				(x>r[(i-2)*x_size+j+3]) &&

				(x>r[(i-1)*x_size+j-3]) &&
				(x>r[(i-1)*x_size+j-2]) &&
				(x>r[(i-1)*x_size+j-1]) &&
				(x>r[(i-1)*x_size+j  ]) &&
				(x>r[(i-1)*x_size+j+1]) &&
				(x>r[(i-1)*x_size+j+2]) &&
				(x>r[(i-1)*x_size+j+3]) &&

				(x>r[(i)*x_size+j-3]) &&
				(x>r[(i)*x_size+j-2]) &&
				(x>r[(i)*x_size+j-1]) &&
				(x>=r[(i)*x_size+j+1]) &&
				(x>=r[(i)*x_size+j+2]) &&
				(x>=r[(i)*x_size+j+3]) &&

				(x>=r[(i+1)*x_size+j-3]) &&
				(x>=r[(i+1)*x_size+j-2]) &&
				(x>=r[(i+1)*x_size+j-1]) &&
				(x>=r[(i+1)*x_size+j  ]) &&
				(x>=r[(i+1)*x_size+j+1]) &&
				(x>=r[(i+1)*x_size+j+2]) &&
				(x>=r[(i+1)*x_size+j+3]) &&

				(x>=r[(i+2)*x_size+j-3]) &&
				(x>=r[(i+2)*x_size+j-2]) &&
				(x>=r[(i+2)*x_size+j-1]) &&
				(x>=r[(i+2)*x_size+j  ]) &&
				(x>=r[(i+2)*x_size+j+1]) &&
				(x>=r[(i+2)*x_size+j+2]) &&
				(x>=r[(i+2)*x_size+j+3]) &&

				(x>=r[(i+3)*x_size+j-3]) &&
				(x>=r[(i+3)*x_size+j-2]) &&
				(x>=r[(i+3)*x_size+j-1]) &&
				(x>=r[(i+3)*x_size+j  ]) &&
				(x>=r[(i+3)*x_size+j+1]) &&
				(x>=r[(i+3)*x_size+j+2]) &&
				(x>=r[(i+3)*x_size+j+3]) )
#endif
				{
				    corner_list[n].info=0;
				    corner_list[n].x=j;
				    corner_list[n].y=i;
				    corner_list[n].dx=cgx[i*x_size+j];
				    corner_list[n].dy=cgy[i*x_size+j];
				    corner_list[n].I=in[i*x_size+j];
				    n++;
				    if(n==MAX_CORNERS){
					fprintf(stderr,"Too many corners.\n");
					exit(1);
				    }}}}
    corner_list[n].info=7;

    free(cgx);
    free(cgy);

}

/* }}} */
/* }}} */
/* {{{ main(argc, argv) */

main(argc, argv)
    int   argc;
    char  *argv [];
{
    /* {{{ vars */

    FILE   *ofp;
    char   filename [80],
	   *tcp;
    uchar  *in, *bp, *mid;
    float  dt=4.0;
    int    *r,
	   argindex=3,
	   bt=20,
	   principle=0,
	   thin_post_proc=1,
	   three_by_three=0,
	   drawing_mode=0,
	   susan_quick=0,
	   max_no_corners=1850,
	   max_no_edges=2650,
	   mode = 0, i,
	   x_size, y_size;
    CORNER_LIST corner_list;

    /* }}} */

    if (argc<3)
	usage();

    get_image(argv[1],&in,&x_size,&y_size);

    /* {{{ look at options */

    while (argindex < argc)
    {
	tcp = argv[argindex];
	if (*tcp == '-')
	    switch (*++tcp)
	    {
		case 's': /* smoothing */
		    mode=0;
		    break;
		case 'e': /* edges */
		    mode=1;
		    break;
		case 'c': /* corners */
		    mode=2;
		    break;
		case 'p': /* principle */
		    principle=1;
		    break;
		case 'n': /* thinning post processing */
		    thin_post_proc=0;
		    break;
		case 'b': /* simple drawing mode */
		    drawing_mode=1;
		    break;
		case '3': /* 3x3 flat mask */
		    three_by_three=1;
		    break;
		case 'q': /* quick susan mask */
		    susan_quick=1;
		    break;
		case 'd': /* distance threshold */
		    if (++argindex >= argc){
			printf ("No argument following -d\n");
			exit(0);}
		    dt=atof(argv[argindex]);
		    if (dt<0) three_by_three=1;
		    break;
		case 't': /* brightness threshold */
		    if (++argindex >= argc){
			printf ("No argument following -t\n");
			exit(0);}
		    bt=atoi(argv[argindex]);
		    break;
	    }	    
	else
	    usage();
	argindex++;
    }

    if ( (principle==1) && (mode==0) )
	mode=1;

    /* }}} */
    /* {{{ main processing */

		    /* {{{ corners */

    r   = (int *) malloc(x_size * y_size * sizeof(int));
    setup_brightness_lut(&bp,bt,6);

    assert(!principle && !susan_quick);
    susan_corners(in,r,bp,max_no_corners,corner_list,x_size,y_size);
    corner_draw(in,corner_list,x_size,drawing_mode);


    /* }}} */

    put_image(argv[2],in,x_size,y_size);

}

/* }}} */
