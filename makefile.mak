# ----------------------------
# PlayStation 1 Psy-Q MAKEFILE
# ----------------------------
all:

	cls
	del main.iso
	ccpsx -O3 -Xo$80010000 Main.c Graphics.c Object.c Model.c Memory.c -oiso\main.cpe
	cpe2x /ce iso\main.cpe
	del iso\main.cpe
	c:\psyq\bin\isotools\mkisofs -xa -o main.iso iso
