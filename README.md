Basic PlayStation 3d
====================

Some very simple 3d work for the original 1995 Sony PlayStation. Just for fun.

Possibly interesting:

- it's purely 640x480, interlaced;
- which means it isn't double buffered in the traditional sense;
- but it effectively is since odd and even fields naturally create alternate non-contiguous sub-buffers; and
- in any case, I've used a triple buffer at the geometry level, with screen painting an essentially asynchronous function of vertical synchronisation.