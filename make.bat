@cls
@echo Building Tetris...
gcc -o3 -w -o tetris.elf tetris.c sprite.c video.c
objcopy -O binary tetris.elf tetris_gba.gba
VisualBoyAdvance.exe tetris_gba.gba
