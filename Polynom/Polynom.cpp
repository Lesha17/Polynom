#include <iostream>
#include <windows.h>
#include <wincon.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// Константы для выбора действия
#define INPUT_FROM_FILE 1
#define MULTIPLY_WITH_NUMBER 2
#define MULTIPLY_WITH_POLYNOM 3
#define CHANGE_SIGN 4
#define ADD_POLYNOM_TO_POLYNOM 5
#define CHECK_EQUAL 6
#define ADD_POLYNOM_TO_LIST 7
#define WRITE_TO_FILE 8

// Полином
class Polynom
{
public:
	static Polynom fromFile(const char * file_name); // читает полином из файла

	Polynom(vector<double> polynom); // Создает полином из вектора с коэффициентами

	string toString(); // служит для вывода полинома на экран
	void save(const char * file_name); // сохраняет полином в файл
	bool operator == (Polynom p); // проверяет два полинома на равенство
	Polynom operator -(); // меняет знаки коэффициентов на противоположные
	Polynom operator + (Polynom p); // Вычисляет сумму двух полиномов
	Polynom operator * (Polynom p); // Вычисляет произведение двух полиномов
	Polynom operator * (double m); // Вычисляет произведение полинома на число

private:
	Polynom(double * polynom, int range); // служит для создания полинома из массива коэффициентов. 
	int _range; // Степень полинома
	double * q; // Коэффициенты полинома. q[i] - коэффициент при x^i. Длина массива на 1 больше степени
};

#pragma region Polynom class methods realization

Polynom Polynom::fromFile(const char * file_name) {
	ifstream file;
	file.open(file_name, ios::in);

	if (!file.is_open()) {
		throw - 1; // выброс исключения. Так программа не падает
	}

	int range;

	file >> range;

	if (range < 0) {
		throw - 1;
	}

	double * a = new double[range + 1];

	for (int i = 0; i < range + 1; ++i) {
		file >> a[i];
	}
	file.close();
	return Polynom(a, range);
}

Polynom::Polynom(vector<double> polynom) {
	_range = polynom.size() - 1;
	q = new double[polynom.size()];
	if (polynom.size() == 0)
		return;
	for (int i = 0; i < polynom.size(); ++i) {
		q[i] = polynom[i];
	}
}

Polynom::Polynom(double * polynom, int range) {
	_range = range;
	q = new double[range + 1];
	for (int i = 0; i < range + 1; ++i) {
		q[i] = 0;
	}
	memcpy(q, polynom, (range + 1) * sizeof(double));
}

string Polynom::toString() {
	string str = "";
	if (_range > 0) {
		str = (std::to_string(q[_range])).append("x^").append(std::to_string(_range));
	}
	else {
		return std::to_string(q[0]);
	}

	for (int i = _range - 1; i > 0; --i) {
		str = str.append(std::string(q[i] >= 0 ? "+" : "").append(std::to_string(q[i])).append("x^").append(std::to_string(i)));
	}
	return str.append(std::string(q[0] >= 0 ? "+" : "")).append(std::to_string(q[0]));
}

void Polynom::save(const char * file_name) {
	ofstream file;
	file.open(file_name);
	file.clear();
	file << _range << " ";
	for (int i = 0; i < _range + 1; ++i) {
		file << q[i] << " ";
	}
	file.close();
}

bool Polynom::operator==(Polynom p) {
	if (_range != p._range) {
		return false;
	}

	for (int i = 0; i < _range; ++i) {
		if (q[i] != p.q[i]) {
			return false;
		}
	}

	return true;
}

Polynom Polynom::operator-() {
	double * a = new double[_range + 1];

	for (int i = 0; i < _range + 1; ++i) {
		a[i] = -q[i];
	}

	return Polynom(a, _range);
}

Polynom Polynom::operator+(Polynom p) {
	if (*this == (-p)) {
		double * a = new double[1];
		a[0] = 0;
		return Polynom(a, 0);
	}

	int max_size = max(_range, p._range) + 1;
	int min_size = min(_range, p._range) + 1;
	double * pmax = _range > p._range ? q : p.q;
	double * pmin = _range < p._range ? q : p.q;

	double * a = new double[max_size];

	for (int i = 0; i < min_size; ++i) {
		a[i] = pmax[i] + pmin[i];
	}

	for (int i = min_size; i < max_size; ++i) {
		a[i] = pmax[i];
	}

	return Polynom(a, max_size - 1);
}

Polynom Polynom::operator*(Polynom p) {
	int range = _range + p._range;

	double * a = new double[range + 1];
	for (int i = 0; i < range + 1; ++i) {
		a[i] = 0;
	}

	for (int i = 0; i <= _range; ++i) {
		for (int j = 0; j <= p._range; ++j) {
			a[i + j] += q[i] * p.q[j];
		}
	}

	return Polynom(a, range);
}

Polynom Polynom::operator*(double m) {
	if (m == 0) {
		double * a = new double[1];
		a[0] = 0;
		return Polynom(a, 0);
	}

	double * a = new double[_range + 1];

	for (int i = 0; i < _range + 1; ++i) {
		a[i] = q[i] * m;
	}

	return Polynom(a, _range);
}

#pragma endregion

std::vector<Polynom> polynoms; // хранит список полиномов
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // нужен для изменения цвета текста в консоли

#pragma region Supporting methods for interaction with user

												   // SetConsoleTextAttribute(hConsole, c) -  меняет цвет текста в консоли на c
												   // try - catch спасает программу от падений


void read_polynom() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Введите имя файла с полиномом" << "\n";
	char * file_name = new char[256];
	cin >> file_name;

	try {
		Polynom p = Polynom::fromFile(file_name);
		polynoms.push_back(p);
	}
	catch (int e) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "Ошибка чтения файла\n";
		SetConsoleTextAttribute(hConsole, 8);
		system("pause");
	}
}

// спрашивает пользоватея, следует ли записать ли операцию в файл
bool ask_save_to_file() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Сохранить операцию в файл? (y/n)";
	getchar();
	char c = getchar();
	if (c == 'y') {
		return true;
	}
	else if (c == 'n') {
		return false;
	}
	else
	{
		return ask_save_to_file();
	}
}

// спрашивает пользователя, следует ли добавить полином - результат операции в polynoms
bool ask_add_result() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Добавить результат в список? (y/n)";
	getchar();
	char c = getchar();
	if (c == 'y') {
		return true;
	}
	else if (c == 'n') {
		return false;
	}
	else
	{
		return ask_add_result();
	}
}

void multiply_with_number() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Введите номер полинома" << "\n";
	int i;
	cin >> i;
	if (i >= polynoms.size() || i < 0) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "полинома с таким номером нет" << "\n";
		system("pause");
		return;
	}
	double m;
	cout << "Введите число, на которое нужно умножить" << "\n";
	cin >> m;

	Polynom result = polynoms[i] * m;
	cout << m << " * (" << polynoms[i].toString() << ") = " << result.toString() << "\n";

	if (ask_save_to_file()) {
		SetConsoleTextAttribute(hConsole, 9);
		cout << "Введите имя файла" << "\n";
		SetConsoleTextAttribute(hConsole, 8);
		char * file_name = new char[256];
		cin >> file_name;

		ofstream file;
		file.open(file_name);
		file.clear();
		file << m << "*(" << polynoms[i].toString() << ")->" << result.toString();
		file.close();
	}
	if (ask_add_result()) {
		polynoms.push_back(result);
	}
}

void multiply_with_polynom() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Введите номер первого полинома" << "\n";
	int i1;
	cin >> i1;
	if (i1 >= polynoms.size() || i1 < 0) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "полинома с таким номером нет" << "\n";
		system("pause");
		return;
	}
	cout << '\n';
	cout << "Введите номер второго полинома" << "\n";
	int i2;
	cin >> i2;
	if (i2 >= polynoms.size() || i2 < 0) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "полинома с таким номером нет" << "\n";
		system("pause");
		return;
	}
	cout << '\n';

	Polynom result = polynoms[i1] * polynoms[i2];
	SetConsoleTextAttribute(hConsole, 10);
	cout << "(" << polynoms[i1].toString() << ")" << " * (" << polynoms[i2].toString() << ") = " << result.toString() << "\n";

	if (ask_save_to_file()) {
		SetConsoleTextAttribute(hConsole, 9);
		cout << "Введите имя файла" << "\n";
		SetConsoleTextAttribute(hConsole, 8);
		char * file_name = new char[256];
		cin >> file_name;

		ofstream file;
		file.open(file_name);
		file.clear();
		file << "(" << polynoms[i1].toString() << ")" << "*(" << polynoms[i2].toString() << ")->" << result.toString() << "\n";
		file.close();
	}
	if (ask_add_result()) {
		polynoms.push_back(result);
	}
}

void change_sign() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Введите номер полинома" << "\n";
	int i;
	cin >> i;
	if (i >= polynoms.size() || i < 0) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "полинома с таким номером нет" << "\n";
		system("pause");
		return;
	}

	Polynom result = -polynoms[i];
	cout << " - (" << polynoms[i].toString() << ") = " << result.toString() << "\n";

	if (ask_save_to_file()) {
		SetConsoleTextAttribute(hConsole, 9);
		cout << "Введите имя файла" << "\n";
		SetConsoleTextAttribute(hConsole, 8);
		char * file_name = new char[256];
		cin >> file_name;

		ofstream file;
		file.open(file_name);
		file.clear();
		file << "-(" << polynoms[i].toString() << ")->" << result.toString();
		file.close();
	}
	if (ask_add_result()) {
		polynoms.push_back(result);
	}
}

void add_polynom_to_polynom() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Введите номер первого полинома" << "\n";
	int i1;
	cin >> i1;
	if (i1 >= polynoms.size() || i1 < 0) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "полинома с таким номером нет" << "\n";
		system("pause");
		return;
	}
	cout << '\n';
	cout << "Введите номер второго полинома" << "\n";
	int i2;
	cin >> i2;
	if (i2 >= polynoms.size() || i2 < 0) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "полинома с таким номером нет" << "\n";
		system("pause");
		return;
	}
	cout << '\n';

	Polynom result = polynoms[i1] + polynoms[i2];
	SetConsoleTextAttribute(hConsole, 10);
	cout << "(" << polynoms[i1].toString() << ")" << " + (" << polynoms[i2].toString() << ") = " << result.toString() << "\n";

	if (ask_save_to_file()) {
		SetConsoleTextAttribute(hConsole, 9);
		cout << "Введите имя файла" << "\n";
		SetConsoleTextAttribute(hConsole, 8);
		char * file_name = new char[256];
		cin >> file_name;

		ofstream file;
		file.open(file_name);
		file.clear();
		file << "(" << polynoms[i1].toString() << ")" << "+(" << polynoms[i2].toString() << ")->" << result.toString() << "\n";
		file.close();
	}
	if (ask_add_result()) {
		polynoms.push_back(result);
	}
}

void check_equal() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Введите номер первого полинома" << "\n";
	int i1;
	cin >> i1;
	if (i1 >= polynoms.size() || i1 < 0) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "полинома с таким номером нет" << "\n";
		system("pause");
		return;
	}
	cout << '\n';
	cout << "Введите номер второго полинома" << "\n";
	int i2;
	cin >> i2;
	if (i2 >= polynoms.size() || i2 < 0) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "полинома с таким номером нет" << "\n";
		system("pause");
		return;
	}
	cout << '\n';

	bool result = polynoms[i1] == polynoms[i2];
	SetConsoleTextAttribute(hConsole, 10);
	cout << "Полиномы (" << polynoms[i1].toString() << ")" << " и (" << polynoms[i2].toString() << ") " << (result ? "равны" : "не равны") << "\n";
	SetConsoleTextAttribute(hConsole, 8);
	system("pause");
}

void add_polynom_to_list() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Введены степень полинома:\n";
	int range;
	SetConsoleTextAttribute(hConsole, 8);
	cin >> range;
	vector<double> pol;
	pol.reserve(range + 1);

	SetConsoleTextAttribute(hConsole, 9);
	cout << "Вводите коэффициенты полинома по одному" << "\n";
	SetConsoleTextAttribute(hConsole, 8);
	for (int i = 0; i < range + 1; ++i) {
		double q;
		cin >> q;
		pol.push_back(q);
	}
	Polynom p(pol);
	polynoms.push_back(p);
}

void write_to_file() {
	SetConsoleTextAttribute(hConsole, 9);
	cout << "Введите номер полинома" << "\n";
	int i;
	cin >> i;
	if (i >= polynoms.size() || i < 0) {
		SetConsoleTextAttribute(hConsole, 12);
		cout << "полинома с таким номером нет" << "\n";
		system("pause");
		return;
	}

	SetConsoleTextAttribute(hConsole, 9);
	cout << "Введите имя файла" << "\n";
	SetConsoleTextAttribute(hConsole, 8);
	char * file_name = new char[256];
	cin >> file_name;
	polynoms[i].save(file_name);
}

#pragma endregion

// Выводит начальное состояние интерфейса, осуществляет взаимодействие с пользователем
int start() {
	system("cls");

	if (polynoms.size() > 0) {
		SetConsoleTextAttribute(hConsole, 9);
		cout << "Введены следующие полиномы:\n";
		SetConsoleTextAttribute(hConsole, 10);
		for (int i = 0; i < polynoms.size(); ++i) {
			cout << i << "\t" << polynoms[i].toString() << "\n";
		}
	}
	else {
		cout << "В программу ещё не введены полиномы" << "\n";
	}

	SetConsoleTextAttribute(hConsole, 9);
	cout << "Что вы хотите сделать?\n";
	SetConsoleTextAttribute(hConsole, 8);
	cout << INPUT_FROM_FILE << "\t-\t" << "Прочитать полином из файла" << "\n";
	cout << MULTIPLY_WITH_NUMBER << "\t-\t" << "Умножить полином на число" << "\n";
	cout << MULTIPLY_WITH_POLYNOM << "\t-\t" << "Умножить полином на полином" << "\n";
	cout << CHANGE_SIGN << "\t-\t" << "Измененить знаки коэффициентов полинома на противоположные" << "\n";
	cout << ADD_POLYNOM_TO_POLYNOM << "\t-\t" << "Сложить два полинома" << "\n";
	cout << CHECK_EQUAL << "\t-\t" << "Проверить два полинома на равенство" << "\n";
	cout << ADD_POLYNOM_TO_LIST << "\t-\t" << "Добавить полином" << "\n";
	cout << WRITE_TO_FILE << "\t-\t" << "Записать полином в файл" << "\n";
	cout << 0 << "\t-\t" << "Закрыть программу" << "\n";

	int what = getchar() - '1' + 1;

	switch (what)
	{
	case INPUT_FROM_FILE:
		read_polynom();
		break;
	case MULTIPLY_WITH_NUMBER:
		multiply_with_number();
		break;
	case MULTIPLY_WITH_POLYNOM:
		multiply_with_polynom();
		break;
	case CHANGE_SIGN:
		change_sign();
		break;
	case ADD_POLYNOM_TO_POLYNOM:
		add_polynom_to_polynom();
		break;
	case CHECK_EQUAL:
		check_equal();
		break;
	case ADD_POLYNOM_TO_LIST:
		add_polynom_to_list();
		break;
	case WRITE_TO_FILE:
		write_to_file();
		break;
	case 0:
		return 0;
	default:
		break;
	}
	return 1;
}

int main()
{
	while (start());

	system("pause");

	return 0;
}

