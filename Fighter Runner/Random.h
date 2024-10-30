#ifndef RANDOM_H
#define RANDOM_H
#include <random>
#include <chrono>

class Random {
public:
	template<typename T>
	static T get(T min, T max) {
		return std::uniform_int_distribution{min, max}(mt);
	};

	static std::mt19937& mtValue() { return mt; }

private:
	static std::mt19937 generate() {
		std::random_device rd{};
		
		std::seed_seq ss{
			rd(), rd(), rd() , rd() , rd() , rd() , rd(),
			static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count())
		};

		return std::mt19937{ss};
	}

	static inline std::mt19937 mt{ generate() };



};


#endif
