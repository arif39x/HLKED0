/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// Brutally hacked version of xwd to provide basic screen capture on
// current X11 display to memory buffer, w/o frills.  For DFRWS GPU
// Malware Forensics Challenge, condensed into one source file, all
// printable strings and error routines removed, etc., by Golden
// G. Richard III (@nolaforensix), 12/2014.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

//
// Original copyright message for xwd follows:
//

/*

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/

/*
 * xwd.c MIT Project Athena, X Window system window raster image dumper.
 *
 * This program will dump a raster image of the contents of a window into a
 * file for output on graphics printers or for other uses.
 *
 *  Author:	Tony Della Fera, DEC
 *		17-Jun-85
 *
 *  Modification history:
 *
 *  11/14/86 Bill Wyatt, Smithsonian Astrophysical Observatory
 *    - Removed Z format option, changing it to an XY option. Monochrome
 *      windows will always dump in XY format. Color windows will dump
 *      in Z format by default, but can be dumped in XY format with the
 *      -xy option.
 *
 *  11/18/86 Bill Wyatt
 *    - VERSION 6 is same as version 5 for monchrome. For colors, the
 *      appropriate number of Color structs are dumped after the header,
 *      which has the number of colors (=0 for monochrome) in place of the
 *      V5 padding at the end. Up to 16-bit displays are supported. I
 *      don't yet know how 24- to 32-bit displays will be handled under
 *      the Version 11 protocol.
 *
 *  6/15/87 David Krikorian, MIT Project Athena
 *    - VERSION 7 runs under the X Version 11 servers, while the previous
 *      versions of xwd were are for X Version 10.  This version is based
 *      on xwd version 6, and should eventually have the same color
 *      abilities. (Xwd V7 has yet to be tested on a color machine, so
 *      all color-related code is commented out until color support
 *      becomes practical.)
 */

/*%
 *%    This is the format for commenting out color-related code until
 *%  color can be supported.
%*/

#define cplusplus // avoid "class" field name in struct Visual // GGRIII
#include <stdio.h>
#include <errno.h>
#include <X11/Xos.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "X11/XWDFile.h"

typedef unsigned long Pixel;

typedef struct {
  VisualID	visualID;		/* The VisualID of the overlay visual */
  int		transparentType;	/* Can be None, TransparentPixel or
					 * TransparentMask */
  Pixel		value;			/* Pixel value */
  int		layer;			/* Overlay planes will always be in
					 * layer 1 */
} OverlayVisualPropertyRec;

typedef struct {
  XVisualInfo	*pOverlayVisualInfo;	/* Pointer to the XVisualInfo struct */
  int		transparentType;	/* Can be None, TransparentPixel or
					 * TransparentMask */
  Pixel		value;			/* Pixel value */
  int		layer;			/* Overlay planes will always be in
					 * layer 1 */
} OverlayInfo;


#ifndef None
#define None 0
#endif
#ifndef TransparentPixel
#define TransparentPixel	1
#endif
#ifdef XKB
#include <X11/extensions/XKBbells.h>
#endif

static int format = ZPixmap;
static Bool use_installed = False;
static Display *dpy;                         /* The current display */
static int screen;                           /* The current screen */

void screenshot(unsigned char *buffer, unsigned int *bufsize);
static int Image_Size(XImage *);
static int Get_XColors(XWindowAttributes *, XColor **);
static void _swapshort(register char *, register unsigned);
static void _swaplong(register char *, register unsigned);
static int Get24bitDirectColors(XColor **);
static int ReadColors(Visual *, Colormap, XColor **);

static int Get24bitDirectColors(XColor **colors) {
    int i , ncolors = 256 ;
    XColor *tcol ;

    *colors = tcol = (XColor *)malloc(sizeof(XColor) * ncolors) ;

    for(i=0 ; i < ncolors ; i++)
    {
	tcol[i].pixel = i << 16 | i << 8 | i ;
	tcol[i].red = tcol[i].green = tcol[i].blue = i << 8   | i ;
    }

    return ncolors ;
}


void screenshot(unsigned char *buf, unsigned int *bufsize) {
  unsigned long swaptest = 1;
  XColor *colors;
  size_t win_name_size;
  CARD32 header_size;
  int ncolors, i;
  char *win_name;
  char default_win_name[] = "gumborocks";
  Bool got_win_name;
  XWindowAttributes win_info;
  XImage *image;
  int absx, absy;
  unsigned width, height;
  int dwidth, dheight;
  Window dummywin;
  XWDFileHeader header;
  XWDColor xwdcolor;
  Visual		*vis ;
  Window window;
  
  dpy = XOpenDisplay(NULL);
  screen = XDefaultScreen(dpy);
  window = RootWindow(dpy, screen);
  
  if(!XGetWindowAttributes(dpy, window, &win_info)) {
    exit(1);
  }
  
  if (!XTranslateCoordinates (dpy, window, RootWindow (dpy, screen), 0, 0,
			      &absx, &absy, &dummywin)) {
    exit (1);
  }
  win_info.x = absx;
  win_info.y = absy;
  width = win_info.width;
  height = win_info.height;
  dwidth = DisplayWidth (dpy, screen);
  dheight = DisplayHeight (dpy, screen);
  absx -= win_info.border_width;
  absy -= win_info.border_width;
  width += (2 * win_info.border_width);
  height += (2 * win_info.border_width);
  
  if (absx < 0) width += absx, absx = 0;
  if (absy < 0) height += absy, absy = 0;
  if (absx + width > dwidth) width = dwidth - absx;
  if (absy + height > dheight) height = dheight - absy;
  
  XFetchName(dpy, window, &win_name);
  if (!win_name || !win_name[0]) {
    win_name = default_win_name;
    got_win_name = False;
  } else {
    got_win_name = True;
  }
  
  win_name_size = strlen(win_name) + sizeof(char);
    
  image = XGetImage (dpy, RootWindow(dpy, screen), absx, absy,
		     width, height, AllPlanes, format);
  
  if (!image) {
    exit(1);
  }
  
  ncolors = Get_XColors(&win_info, &colors);
  vis = win_info.visual;
  header_size = SIZEOF(XWDheader) + (CARD32) win_name_size;
  *bufsize = header_size + sizeof(XWDColor) * ncolors + Image_Size(image);
  
  header.header_size = (CARD32) header_size;
  header.file_version = (CARD32) XWD_FILE_VERSION;
  header.pixmap_format = (CARD32) format;
  header.pixmap_depth = (CARD32) image->depth;
  header.pixmap_width = (CARD32) image->width;
  header.pixmap_height = (CARD32) image->height;
  header.xoffset = (CARD32) image->xoffset;
  header.byte_order = (CARD32) image->byte_order;
  header.bitmap_unit = (CARD32) image->bitmap_unit;
  header.bitmap_bit_order = (CARD32) image->bitmap_bit_order;
  header.bitmap_pad = (CARD32) image->bitmap_pad;
  header.bits_per_pixel = (CARD32) image->bits_per_pixel;
  header.bytes_per_line = (CARD32) image->bytes_per_line;
  header.visual_class = (CARD32) vis->c_class;
  header.red_mask = (CARD32) vis->red_mask;
  header.green_mask = (CARD32) vis->green_mask;
  header.blue_mask = (CARD32) vis->blue_mask;
  header.bits_per_rgb = (CARD32) vis->bits_per_rgb;
  header.colormap_entries = (CARD32) vis->map_entries;
  
  header.ncolors = ncolors;
  header.window_width = (CARD32) win_info.width;
  header.window_height = (CARD32) win_info.height;
  header.window_x = absx;
  header.window_y = absy;
  header.window_bdrwidth = (CARD32) win_info.border_width;
  
  if (*(char *) &swaptest) {
    _swaplong((char *) &header, sizeof(header));
    for (i = 0; i < ncolors; i++) {
      _swaplong((char *) &colors[i].pixel, sizeof(CARD32));
      _swapshort((char *) &colors[i].red, 3 * sizeof(short));
    }
  }
  
  memcpy(buf, &header, SIZEOF(XWDheader));
  memcpy(buf+SIZEOF(XWDheader), win_name, win_name_size);
  
  for (i = 0; i < ncolors; i++) {
    xwdcolor.pixel = colors[i].pixel;
    xwdcolor.red = colors[i].red;
    xwdcolor.green = colors[i].green;
    xwdcolor.blue = colors[i].blue;
    xwdcolor.flags = colors[i].flags;
    memcpy(buf + header_size + i*SIZEOF(XWDColor), &xwdcolor, SIZEOF(XWDColor));
  }
  
  memcpy(buf + header_size + ncolors * SIZEOF(XWDColor), image->data, Image_Size(image));
  
  if(ncolors > 0) free(colors);
  if (got_win_name) XFree(win_name);
  XDestroyImage(image);
  XCloseDisplay(dpy);

}


int Image_Size(XImage *image) {
  if (image->format != ZPixmap)
    return(image->bytes_per_line * image->height * image->depth);
  
  return(image->bytes_per_line * image->height);
}

#define lowbit(x) ((x) & (~(x) + 1))

static int ReadColors(Visual *vis, Colormap cmap, XColor **colors) {
  int i,ncolors ;
  
  ncolors = vis->map_entries;
  
  if (!(*colors = (XColor *) malloc (sizeof(XColor) * ncolors))) {
    exit(1);
  }
  
  if (vis->c_class == DirectColor ||
      vis->c_class == TrueColor) {
    Pixel red, green, blue, red1, green1, blue1;
    
    red = green = blue = 0;
    red1 = lowbit(vis->red_mask);
    green1 = lowbit(vis->green_mask);
    blue1 = lowbit(vis->blue_mask);
    for (i=0; i<ncolors; i++) {
      (*colors)[i].pixel = red|green|blue;
      (*colors)[i].pad = 0;
      red += red1;
      if (red > vis->red_mask)
	red = 0;
      green += green1;
      if (green > vis->green_mask)
	green = 0;
      blue += blue1;
      if (blue > vis->blue_mask)
	blue = 0;
    }
  } else {
    for (i=0; i<ncolors; i++) {
      (*colors)[i].pixel = i;
      (*colors)[i].pad = 0;
    }
  }
  
  XQueryColors(dpy, cmap, *colors, ncolors);
  
  return(ncolors);
}

int Get_XColors(XWindowAttributes *win_info, XColor **colors) {
  int i, ncolors;
  Colormap cmap = win_info->colormap;
  
  if (use_installed)
    cmap = XListInstalledColormaps(dpy, win_info->root, &i)[0];
  if (!cmap)
    return(0);
  ncolors = ReadColors(win_info->visual,cmap,colors) ;
  return ncolors ;
}

void _swapshort (register char *bp, register unsigned n) {
  register char c;
  register char *ep = bp + n;
  
  while (bp < ep) {
    c = *bp;
    *bp = *(bp + 1);
    bp++;
    *bp++ = c;
  }
}

void _swaplong (register char *bp, register unsigned n) {
  register char c;
  register char *ep = bp + n;
  
  while (bp < ep) {
    c = bp[3];
    bp[3] = bp[0];
    bp[0] = c;
    c = bp[2];
    bp[2] = bp[1];
    bp[1] = c;
    bp += 4;
  }
}
