#include "Firefly.hpp"

Firefly::Firefly(HSL color, sf::Vector3f position, sf::Vector3f velocity)
: color(color), position{position}, velocity{velocity} {}

Firefly& Firefly::operator=(const Firefly& fly) {
	color = fly.color;
	position = fly.position;
	velocity = fly.velocity;
	return *this;
}

bool Firefly::operator!=(const Firefly& rhs) const{
	return color != rhs.color
	       || position != rhs.position
	       || velocity != rhs.velocity;
}

bool Firefly::operator==(const Firefly& rhs) const{
	return color == rhs.color
	       && position == rhs.position
	       && velocity == rhs.velocity;
}

bool Firefly::isCloser(const Firefly& one, const Firefly& two) {
	return one.position.z < two.position.z;
}

bool Firefly::isFarther(const Firefly& one, const Firefly& two) {
	return one.position.z > two.position.z;
}
