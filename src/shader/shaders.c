#define STRINGIFY(x) #x

static const char* FRAGMENT_SHADER_CODE = STRINGIFY(
		uniform vec4 color;
		void main() {
			gl_FragColor = vec4(color);
		}
	);

static const char* VERTEX_SHADER_CODE = STRINGIFY(
		attribute vec3 pos;
		void main() {
			gl_Position = vec4(pos, 1.0);
		}
	);
