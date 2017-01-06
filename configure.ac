dnl required version of autoconf
AC_PREREQ([2.53])

dnl TODO: fill in your package name and package version here
AC_INIT([airplay-src],[1.0.0])


AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

dnl required version of automake
AM_INIT_AUTOMAKE([1.10])

dnl enable mainainer mode by default
AM_MAINTAINER_MODE([enable])

dnl check for tools (compiler etc.)
AC_PROG_CC

dnl required version of libtool
LT_PREREQ([2.2.6])
LT_INIT

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if ! $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   AC_MSG_ERROR([pkg-config >= 0.22 is required])
fi

### Checks for libraries
gst_airplay_requirements_cflags=""
gst_airplay_requirements_libs=""
gst_airplay_requirements_pc="libshairport"


gstreamer_api_default="0.10"
AC_ARG_WITH(gstreamer-api,
   AS_HELP_STRING([--with-gstreamer-api],
       [manually set the gstreamer API version 0.10 or 1.0 are valid values]),
       [gstreamer_api="${withval}"])

if test "x$gstreamer_api" = "x" ; then
  gstreamer_api="${gstreamer_api_default}"
fi

case "$gstreamer_api" in
  1.0)
  gst_airplay_requirements_pc+=" gstreamer-1.0 gstreamer-base-1.0"
  AC_DEFINE_UNQUOTED(HAVE_GST_1, [1], [Use GStreamer 1.0])
  ;;
  0.10)
  gst_airplay_requirements_pc+=" gstreamer-0.10 gstreamer-base-0.10"
  AC_DEFINE_UNQUOTED(HAVE_GST_0, [1], [Use GStreamer 0.10])
  ;;
  *)
  AC_MSG_ERROR([Unsupported gstreamer version "$gstreamer_api"])
  ;;
esac

dnl make GST_MAJORMINOR available in Makefile.am
GST_MAJORMINOR=$gstreamer_api
AC_SUBST(GST_MAJORMINOR)

PKG_CHECK_MODULES([GST_AIRPLAY], [${gst_airplay_requirements_pc}])

GST_AIRPLAY_LIBS="${GST_AIRPLAY_LIBS} ${gst_airplay_requirements_libs}"
GST_AIRPLAY_CFLAGS="${GST_AIRPLAY_CFLAGS} ${gst_airplay_requirements_cflags}"

dnl check if compiler understands -Wall (if yes, add -Wall to GST_AIRPLAY_CFLAGS)
AC_MSG_CHECKING([to see if compiler understands -Wall])
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([ ], [ ])], [
  GST_AIRPLAY_CFLAGS="$GST_AIRPLAY_CFLAGS -Wall"
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
])

dnl set the plugindir where plugins should be installed (for src/Makefile.am)
if test "x${prefix}" = "x$HOME"; then
  plugindir="$HOME/.gstreamer-$gstreamer_api/plugins"
else
  plugindir="\$(libdir)/gstreamer-$gstreamer_api"
fi
AC_SUBST(plugindir)


AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

