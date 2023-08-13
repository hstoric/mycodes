#include "map.hh"
#include <map>
#include <string>

int main() {
	Map<int, std::string> map;

	for (int i = 0; i < 64; i++) {
		int j = i & 1 ? i : 128 - i;
		printf("Inserting: %d\n", j);
		map.insert(j, std::to_string(i));


		// map.verify();
	}

	for (int i = 0; i < 64; i++) {
		int j = i & 1 ? i : 128 - i;
		printf("Removing: %d\n", j);

		map.remove(j);

		auto k = map.begin();
		while (k != map.end()) {
			printf("%d\n", (k++)->key);
		}

		// map.dump(std::cout);

		// map.verify();
	}

	return 0;
}
