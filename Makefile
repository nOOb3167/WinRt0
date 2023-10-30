.PHONY : all deps

all : out deps out/main.exe

deps : out
	cp /win/mxe/usr/x86_64-w64-mingw32.shared/bin/*.dll out/

out :
	mkdir out || true

out/main.exe : out main.cpp
	/win/mxe/usr/bin/x86_64-w64-mingw32.shared-g++ -std=c++20 -I . -o out/main.exe main.cpp -lruntimeobject -lrpcrt4
