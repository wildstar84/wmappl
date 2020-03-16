bin_PROGRAMS=wmappl
wmappl_SOURCES=wmappl.c dockapp.c xpmbutton.c rcparser.c args.c fileexists.c options.c pixmap.c
noinst_HEADERS=dockapp.h xpmbutton.h rcparser.h args.h fileexists.h options.h pixmap.h
pkgdata_DATA=../icons/*
wmappl_LDADD=@LIBOBJS@

EXTRA_DIST=$(pkgdata_DATA) gnugetopt.h leftarr.xpm rightarr.xpm interface.xpm

