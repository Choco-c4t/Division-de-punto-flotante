#include <iostream>
#include <bitset>
#include <cstdint>
#include <limits>

// "union" permite ver los mismos 32 bits de un float como un entero sin signo (uint32_t), para poder manipularlos con operaciones bit a bit.
union FloatUnion {
	float f;
	uint32_t u;
};

void printBinary(float num) {
	FloatUnion fu;
	fu.f = num;

	std::bitset<32> bits(fu.u);
	std::cout << "Número: " << num << " -> Bits: " << bits << std::endl;
}


float divideFloat(float a, float b) {

	FloatUnion A, B, Z;
	A.f = a;
	B.f = b;

	if (a == 0.0f) {
		return 0.0f;
	}
	if (b == 0.0f) {
		std::cerr << "División por cero. Se retorna infinito positivo." << std::endl;
		return std::numeric_limits<float>::infinity();
	}

	int signA = (A.u >> 31) & 0x1;
	int signB = (B.u >> 31) & 0x1;
	int expA = (A.u >> 23) & 0xFF; 
	int expB = (B.u >> 23) & 0xFF; 


	uint32_t fracA = (A.u & 0x7FFFFF) | 0x800000;
	uint32_t fracB = (B.u & 0x7FFFFF) | 0x800000;

	int signZ = signA ^ signB;

	int expZ = expA - expB + 127;

	uint64_t fracA_ext = ((uint64_t)fracA) << 23;
	uint64_t fracZ = fracA_ext / fracB;

	while (fracZ && ((fracZ & (1 << 23)) == 0)) {
		fracZ <<= 1;
		expZ--; 
	}


	if (expZ <= 0) {
		std::cerr << "Underflow en el exponente." << std::endl;
		return 0.0f;
	}
	if (expZ >= 255) {
		std::cerr << "Overflow en el exponente. Se retorna infinito." << std::endl;
		return std::numeric_limits<float>::infinity();
	}

	uint32_t fracResult = fracZ & 0x7FFFFF;


	Z.u = (signZ << 31) | (expZ << 23) | fracResult;
	return Z.f;
}

int main() {
	float num1, num2;

	std::cout << "Ingrese el dividendo (X): ";
	std::cin >> num1;
	std::cout << "Ingrese el divisor (Y): ";
	std::cin >> num2;


	float resultadoSimulado = divideFloat(num1, num2);

	float resultadoNativo = num1 / num2;

	std::cout << "\n--- Resultados ---" << std::endl;
	std::cout << "Resultado simulado (algoritmo manual):" << std::endl;
	printBinary(resultadoSimulado);
	std::cout << "\nResultado nativo (operador / de C++):" << std::endl;
	printBinary(resultadoNativo);

	return 0;
}
