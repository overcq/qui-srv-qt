#*******************************************************************************
# ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒9‒8 V
#*******************************************************************************
qt_modules := $(addprefix Qt6,Core Gui Qml Quick QuickControls2 Widgets)
#===============================================================================
.PHONY: all build install uninstall clean
.DEFAULT: all
all: build
#===============================================================================
build: a.out
a.out: Makefile signals.h 0.c++
	/usr/lib64/qt6/libexec/moc $(filter %.h,$^) -o $(patsubst %.h,moc_%.c++,$(filter %.h,$^))
	$(CXX) -Wall -Wextra -Wno-unused-parameter -std=gnu++17 -fPIC -Os -s $$(pkg-config --cflags $(qt_modules)) -o $@ $(filter %.c++,$^) $(patsubst %.h,moc_%.c++,$(filter %.h,$^)) $$(pkg-config --libs $(qt_modules))
install:
	install -C -m 755 a.out /usr/bin/gui-srv-qt
uninstall:
	rm /usr/bin/gui-srv-qt
clean:
	rm -f a.out $(patsubst %.h,moc_%.c++,$(wildcard *.h))
#*******************************************************************************
