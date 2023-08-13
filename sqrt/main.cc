#include <cstdio>
#include <cmath>
#include <cstdint>
#include <ratio>
#include <immintrin.h>

double myCbrt(double a) {
	double x = a, lastResult;

	do {
		lastResult = x;

		// f(x) = x^3 - a = 0
		// f'(x) = 3x^2
		// x -= (x^3-a)/(3x^2)^2
		// x -= (x*x*x-a)/(3x*3x)^2
		// x -= (x*x*x-a)/(3x*3x*3x*3x)
		// x -= (x*x*x-a)/(81*x*x*x*x)
		// x -= x*x*x/(81*x*x*x*x) - a/(81*x*x*x*x)
		// x -= x*x*x/81/x/x/x/x - a/81/x/x/x/x
		// x -= (x^3 - a)/(81 * x^4)
		// x -= x^3/(81*x^4) - a/(81*x^4)
		// x -= (x*x*x)/(x*x*x)/81/x - a/(81*x*x*x*x)
		// x -= (1/81)/x - a/81/x/x/x/x
		// x -= (1 - a/x^3)/81*x
		// x -= 1/81*x - a/x/x/x/81*x
		// x -= x/81 - a/x/x/81
		// x -= (x - a/x/x) / 81
		x -= (x - a / x / x) * 0.012345679012345678;
	} while (x != lastResult);

	return x;
}

int main() {
	for (double i = 1.0; i <= 360.0; i += 1.0) {
		auto stdResult = cbrt(i);
		auto myResult = myCbrt(i);
		printf("i = %.3lf, std = %.3lf, myimpl = %.3lf, diff = %.19lf\n", i, stdResult, myResult, stdResult - myResult);
	}

	return 0;
}
