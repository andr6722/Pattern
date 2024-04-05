#include <iostream>
#include <vector>
#include <assert.h>


// Component
class Component {
public:
	virtual void add(Component* p) {}
	virtual int getBaggageWeight() { return 0; }
	virtual ~Component() {}
	int baggageWeight = 0;
};

// Primitive objects 
class Pilot : public Component {

};

class Stewardess : public Component {

};

class Passenger : public Component {
public:
	Passenger(int weight) {
		baggageWeight = weight;
	}
	int getBaggageWeight() override {
		return baggageWeight;
	}
};

// Composite
class Composite : public Component {
public:
	std::vector<Component*> children;
	void add(Component* component) override {
		children.push_back(component);
	}

	int getBaggageWeight() override {
		int totalWeight = 0;
		for (const auto& child : children) {
			totalWeight += child->getBaggageWeight();
		}
		return totalWeight;
	}

	virtual ~Composite() {
		for (auto& child : children) {
			delete child;
		}
	}
};

class FirstClass : public Composite {
};

class BusinessClass : public Composite {
};

class EconomyClass : public Composite {
};

class Airplane {
	Composite firstClass, businessClass, economyClass;
	std::vector<Pilot*> pilots;
	std::vector<Stewardess*> stewardesses;

	const int maxFirstClassPassengers = 10;
	const int maxBusinessClassPassengers = 20;
	const int maxEconomyClassPassengers = 150;
	;

public:
	void addPilot(Pilot* pilot) {
		pilots.push_back(pilot);
	}
	void addStewardess(Stewardess* stewardess) {
		stewardesses.push_back(stewardess);
	}
	void addPassenger(Component* passenger, const std::string& classType) {
		if (classType == "First" && firstClass.children.size() < maxFirstClassPassengers) {
			firstClass.add(passenger);
		}
		else if (classType == "Business" && businessClass.children.size() < maxBusinessClassPassengers) {
			if (passenger->baggageWeight <= 35) {
				businessClass.add(passenger);
			}
			else {
				std::cout << "Baggage weight exceeds Business class limit.\n";
				businessClass.add(passenger);
			}
		}
		else if (classType == "Economy" && economyClass.children.size() < maxEconomyClassPassengers) {
			if (passenger->baggageWeight <= 20) {
				economyClass.add(passenger);
			}
			else {
				std::cout << "Baggage weight exceeds Economy class limit.\n";
			}
		}
	}

	// Метод для проверки готовности к полету
	bool checkFlightReadiness() {
		if (pilots.size() < 2 || stewardesses.size() < 6) {
			std::cout << "Not enough crew.\n";
			return false;
		}
		return true;
	}
};

int main() {
	Airplane airplane;

	// Добавление пилотов
	airplane.addPilot(new Pilot());
	airplane.addPilot(new Pilot());

	// Добавление стюардесс
	for (int i = 0; i < 6; ++i) {
		airplane.addStewardess(new Stewardess());
	}

	// Добавление пассажиров
	for (int i = 0; i < 150; ++i) {
		airplane.addPassenger(new Passenger(18), "Economy");
	}

	for (int i = 0; i < 20; ++i) {
		airplane.addPassenger(new Passenger(30), "Business");
	}

	for (int i = 0; i < 10; ++i) {
		airplane.addPassenger(new Passenger(40), "First");
	}

	// не загрузит этих пассажиров так как количество уже максимально
	airplane.addPassenger(new Passenger(21), "Economy");
	airplane.addPassenger(new Passenger(25), "Business");
	airplane.addPassenger(new Passenger(10), "First");

	// Проверка готовности самолета к полету
	if (airplane.checkFlightReadiness()) {
		std::cout << "The airplane is ready for takeoff." << std::endl;
	}
	else {
		std::cout << "The airplane is not ready for takeoff." << std::endl;
	}
	return 0;
}