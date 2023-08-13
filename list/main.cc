#include <cstdio>
#include "list.hh"
#include "dynarray.hh"

int main() {
	DynArray<int> list;

	for (int i = 0; i < 100; ++i) {
		printf("Inserting: %d\n", i);
		list.prepend(list.end(), i);
	}

	{
		const auto &ls = list;
		for (size_t i = 0; i < 100; ++i)
			printf("%d\n", ls.get(i));
	}

	return 0;
}
