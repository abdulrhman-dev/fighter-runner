#include <iostream>
#include <string>
#include <array>
#include "Random.h"


class Creature {
public:
	Creature(
		std::string_view name,
		char symbol,
		int health, int attack, int gold
	)
		: m_name{ name }
		, m_symbol{ symbol }
		, m_health{ health }, m_atk{ attack }, m_gold{ gold }
	{}

	const std::string& getName() { return m_name; }
	char getSymbol() const { return m_symbol; }
	int getHealth() const { return m_health; }
	int getAtk() const { return m_atk; }
	int getGold() const { return m_gold; }

	void reduceHealth(int amount) { m_health -= amount; }
	void addGold(int amount) { m_gold += amount; }
	bool isDead() const { return m_health <= 0; }
protected:
	std::string m_name{};
	char m_symbol{};
	int m_health{};
	int m_atk{};
	int m_gold{};
};


class Player : public Creature {
private:
	int m_level{ 1 };

public:
	Player(std::string_view name)
		: Creature(name, '@', 10, 1, 0)
	{}

	void addHealth(int amount) { m_health += amount; };
	void addAttack(int amount) { m_atk += amount; };


	int getLevel() const { return m_level; }

	void levelUp() {
		m_level += 1;
		m_atk += 1;
	}

	bool hasWon() { return m_level >= 20; }
};

class Monster : public Creature {
public:
	enum Type {
		dragon,
		orc,
		slime,
		max_types
	};

	static inline std::array<Creature, max_types> monsters{ {
		{"dragon", 'D', 20, 4, 100},
		{"orc", 'o', 4, 2, 25},
		{"slime", 's', 1, 1, 10},
	} };


	Monster(Type type)
		: Creature(monsters[type])
	{}

	static Type getRandomMonster() {
		return static_cast<Type>(Random::get(0, max_types - 1));
	}
};


class Potion {
public:
	enum Type {
		health,
		strength,
		poison,
		max_types
	};

	enum Level {
		small,
		medium,
		large,
		max_levels
	};

	static constexpr std::array type_names{ "Health","Strength","Poision" };
	static constexpr std::array level_names{ "Small","Medium","Large" };
	static inline int effects[max_types][max_levels] = {
		{2,2,5},
		{1,1,1},
		{1,1,1}
	};

	Potion()
		: m_type{ static_cast<Type>(Random::get(0, max_types - 1)) }
		, m_level{ static_cast<Level>(Random::get(0, max_levels - 1)) }
	{}


	void applyStatus(Player& p) {
		int amount = effects[m_type][m_level];
		switch (m_type)
		{
		case Potion::health:
			p.addHealth(amount);
			break;
		case Potion::strength:
			p.addAttack(amount);
			break;
		case Potion::poison:
			p.reduceHealth(amount);
			break;
		default:
			break;
		}
	}

	friend std::ostream& operator<<(std::ostream& out, Potion& p) {
		std::cout << p.level_names[p.m_level] << " of " << p.type_names[p.m_type];

		return out;
	}

private:
	Type m_type{};
	Level m_level{};
};


void playerAttack(Player& p, Monster& m) {
	m.reduceHealth(p.getAtk());
	std::cout << "You hit the " << m.getName() << " for " << p.getAtk() << " damage.\n";
}

void monsterAttack(Monster& m, Player& p) {
	p.reduceHealth(m.getAtk());
	std::cout << "The " << m.getName() << " hit you for " << m.getAtk() << " damage.\n";
}

void handlePotion(Player& p) {
	std::cout << "You found a mythical potion! Do you want to drink it? [y/n]: ";
	char takePotion{};
	std::cin >> takePotion;

	if (std::tolower(takePotion) == 'y') {
		Potion potion;
		potion.applyStatus(p);
		std::cout << "You drank a " << potion << '\n';
	}
}

void playEncounter(Player& p) {
	Monster m{ Monster::getRandomMonster() };
	std::cout
		<< "You have encountered a " << m.getName()
		<< " (" << m.getSymbol() << ")\n";

	while (!m.isDead() && !p.isDead()) {
		std::cout << p.getSymbol() << "[" << p.getHealth() << "HP," << p.getAtk() << "ATK]\n";
		std::cout << "(R)un or (F)ight: ";
		char choice{};
		std::cin >> choice;

		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		choice = tolower(choice);

		if (choice == 'f') {
			playerAttack(p, m);

			if (!m.isDead())
				monsterAttack(m, p);
		}
		else if (choice == 'r') {
			bool successRun{ static_cast<bool>(Random::get(0,1)) };

			if (successRun) {
				std::cout << "You successfully fled.\n";
				return;
			}

			std::cout << "You failed to flee.\n";
			monsterAttack(m, p);
		}
	}

	if (m.isDead()) {
		std::cout << "You killed the " << m.getName() << ".\n";
		p.levelUp();
		p.addGold(m.getGold());
		std::cout << "You are now level " << p.getLevel() << '\n';

		bool foundPotion{ 1 == Random::get(1, 3) };

		if (foundPotion) {
			handlePotion(p);
		}
	}
}


int main()
{
	std::cout << "Enter your name: ";
	std::string name{};
	std::getline(std::cin >> std::ws, name);
	Player p{ name };
	std::cout << "welcome, " << name << '\n';

	while (!p.isDead() && !p.hasWon()) {
		playEncounter(p);
	}

	if (p.isDead())
	{
		std::cout << "You died at level " << p.getLevel() << " and with " << p.getGold() << " gold.\n";
		return 0;
	}

	std::cout << "You won with " << p.getGold() << " gold.\n";

	return 0;
}