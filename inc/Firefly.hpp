#ifndef __FIREFLY_HPP__
#define __FIREFLY_HPP__

#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include "HSL.hpp"

class Firefly{
	public:
	HSL          color{};
	sf::Clock    clock{};
	sf::Vector3f position{0.f, 0.f, 0.f};
	sf::Vector3f velocity{0.f, 0.f, 0.f};
	
	Firefly() {};
	Firefly(HSL color, sf::Vector3f position, sf::Vector3f velocity);
	Firefly& operator=(const Firefly& fly);
	bool operator!=(const Firefly& rhs) const;
	bool operator==(const Firefly& rhs) const;
	static bool isCloser(const Firefly& one, const Firefly& two);
	static bool isFarther(const Firefly& one, const Firefly& two);
};

#endif
