#include <cstdint>
#include <cstdio>
#include <cfloat>

float abc(float x) {
	return 1.14f + x;
}

int main() {
	printf("%f\n", abc(5.14f));
}
