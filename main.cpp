#include "domain.hpp"
#include "random.hpp"
#include "model.hpp"

using namespace std;

/*
TODO:
x энтропия
x случайные числа
x правила (vector<array<4, Domain>>)
x основной цикл
- enum для индексов направлений
- симметрия
x парсинг данных из ввода
x ввод
x растянуть main по функциям (?)
- обработка ошибок
*/

int main(int , char* argv[]) {
	srand(time(nullptr));

	int n, m;
    cin >> n >> m;

	Model model(argv[1]);
	model.Generate(n, m);
	model.Print();
}