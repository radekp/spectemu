TEMPLATE = app
TARGET = spectemu
CONFIG += qtopia
QT += xml network
LIBS += -lX11
DEFINES += HAVE_MITSHM \
    HAVE_SHMQUERY \
    Z80C \
    SPECT_MEM \
    HAVE_SOUND \
    STDC_HEADERS

# I18n info
STRING_LANGUAGE = en_US
LANGUAGES = en_US

RESOURCES = resources.qrc

# Input files
HEADERS = qspectemu.h
SOURCES = interf.c \
    memdmps.c \
    rom48.c \
    spconf.c \
    spjoy.c \
    spperif.c \
    sptape.c \
    tapefile.c \
    z80_op1.c \
    z80_op4.c \
    z80_op6.c \
    keynames.c \
    misc.c \
    scrshot.c \
    spect.c \
    spkey.c \
    spscr.c \
    sptiming.c \
    z80.c \
    z80_op3.c \
    z80_step.c \
    compr.c \
    load_z80.c \
    opname.c \
    snapshot.c \
    spectkey.c \
    spmain.c \
    spsound.c \
    z80optab.c \
    z80_op2.c \
    z80_op5.c \
    qspectemu.cpp \
    main.cpp
