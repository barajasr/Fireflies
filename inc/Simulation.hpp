#ifndef __SIMULATION_HPP__
#define __SIMULATION_HPP_

#include <array>
#include <functional>
#include <random>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Firefly.hpp"

using realDistribution = std::uniform_real_distribution<float>;
using intDistribution  = std::uniform_int_distribution<int>;

class Simulation {
	private:
	const size_t        height{500};
	const size_t        width{500};
	const size_t        depth{500};
	static const size_t fireflyCount{3}; // Additional 1 for average case

	std::mt19937        mt{201};
	realDistribution    coordinate{0.f, 500.f};
	intDistribution     luminance{0, 100};
	realDistribution    velComponent{-20.5f, 20.5f};

	std::array<Firefly, fireflyCount> fireflies;
	sf::RenderWindow                  window;

	void displayFireflies();
	void init();
	void outOfBoundsCorrect(Firefly& fly);
	void sortByDepth();
	void update();
	void updateFly(Firefly& fly, const float& luminance, const sf::Vector3f& position);

	public:
	Simulation() {}
	void run();
};

#endif
