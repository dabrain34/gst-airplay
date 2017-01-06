plugin_LTLIBRARIES = libgstairplay.la


# sources used to compile this plug-in
libgstairplay_la_SOURCES = gstairplaysrc.c gstairplaysrc.h

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstairplay_la_CFLAGS = $(GST_AIRPLAY_CFLAGS)
libgstairplay_la_LIBADD = $(GST_AIRPLAY_LIBS)
libgstairplay_la_LDFLAGS = $(GST_AIRPLAY_LDFLAGS) -avoid-version
libgstairplay_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstairplaysrc.h
