INCLUDE_FLAGS = -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vcms_host/linux -I/opt/vc/include
LIB_FLAGS = -L/opt/vc/lib -lEGL -lGLESv2 -lbcm_host -ljpeg -lpthread

all:	glspeed_line_dashed	glspeed_line_gradient	glspeed_line	glspeed	vgspeed
default:	all

build-dir:
	mkdir -p "./build/"

glspeed:	build-dir	./src/glspeed.c
	gcc $(INCLUDE_FLAGS) src/glspeed.c -o ./build/glspeed $(LIB_FLAGS)

glspeed_line:	build-dir	./src/glspeed_line.c
	gcc $(INCLUDE_FLAGS) src/glspeed_line.c -o ./build/glspeed_line $(LIB_FLAGS)

glspeed_line_dashed:	build-dir	./src/glspeed_line_dashed.c
	gcc $(INCLUDE_FLAGS) src/glspeed_line_dashed.c -o ./build/glspeed_line_dashed $(LIB_FLAGS)

glspeed_line_gradient:	build-dir	./src/glspeed_line_dashed.c
	gcc $(INCLUDE_FLAGS) src/glspeed_line_dashed.c -o ./build/glspeed_line_dashed $(LIB_FLAGS)

vgspeed:	build-dir	./src/vgspeed.c
	gcc $(INCLUDE_FLAGS) src/vgspeed.c -o ./build/vgspeed $(LIB_FLAGS)
