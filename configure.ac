AC_INIT([hello-metal], [0.0.1], [andrew@andrewsoutar.com])

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl We need the c compiler...
AC_PROG_CC
dnl ...and the assembler
AM_PROG_AS

dnl Find a grub-mkrescue binary
AC_ARG_VAR([GRUB_MKRESCUE], ["grub-mkrescue binary"])
AC_CHECK_PROGS([GRUB_MKRESCUE], [grub2-mkrescue grub-mkrescue])
AM_CONDITIONAL([HAVE_GRUB_MKRESCUE], [test "x$GRUB_MKRESCUE" != "x"])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
