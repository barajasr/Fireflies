#include <cmath>
#include <iostream>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Event.hpp>
#include "Simulation.hpp"

void Simulation::displayFireflies() {
	window.clear();

	const float radius = 4.f;
	sf::CircleShape circle{radius, 40};
	circle.setOrigin(radius/2.f, radius/2.f);
	for (auto fly=fireflies.begin(), end=fireflies.end(); fly != end; ++fly) {
		const float factor = (depth - fly->position.z ) / depth;
		circle.setScale(factor, factor);
		circle.setPosition(fly->position.x, fly->position.y);
		circle.setFillColor(fly->color.turnToRGB());
		window.draw(circle);
	}

	window.display();
}

void Simulation::init() {
	window.create(sf::VideoMode(height, width),
	              "Fireflies",
	              sf::Style::Close | sf::Style::Titlebar);
	window.setPosition(sf::Vector2i(700, 100));
	window.setVerticalSyncEnabled(true);
	
	intDistribution rgb{0, 255};
	for (auto fly=fireflies.begin(), end=fireflies.end()-1; fly != end; ++fly) {
		fly->color = turnToHSL(sf::Color(rgb(mt), rgb(mt), rgb(mt)));
		fly->color.luminance = luminance(mt);
		fly->position = {coordinate(mt), coordinate(mt), coordinate(mt)};
		fly->velocity = {velComponent(mt), velComponent(mt), velComponent(mt)};
	}

	fireflies.back().color = turnToHSL(sf::Color::Red);
	fireflies.back().velocity = {0.f, 0.f, 0.f};
}

void Simulation::outOfBoundsCorrect(Firefly& fly) {
	if (fly.position.x < 0) {
		fly.position.x = 0;
		fly.velocity.x *= -1;
	} else if (fly.position.x > width) {
		fly.position.x = width;
		fly.velocity.x *= -1;
	}

	if (fly.position.y < 0) {
		fly.position.y = 0;
		fly.velocity.y *= -1;
	} else if (fly.position.y > height) {
		fly.position.y = height;
		fly.velocity.y *= -1;
	}

	if (fly.position.z < 0) {
		fly.position.z = 0;
		fly.velocity.z *= -1;
	} else if (fly.position.z > depth) {
		fly.position.z = depth;
		fly.velocity.z *= -1;
	}
}

void Simulation::run() {
	init();

	sf::Event event;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed)
				window.close();

		update();
		displayFireflies();
	}
}

void Simulation::sortByDepth() {
	// Last element is used for strictly average representation
	std::sort(fireflies.begin(), fireflies.end()-1, Firefly::isFarther);
}

void Simulation::update() {
	const auto flies = fireflies;
	for (size_t index=0; index != fireflyCount-1; ++index) {
		updateFly(index, flies);
	}

	updateAvgFly();
	sortByDepth();
}

void Simulation::updateAvgFly() {
	float luminance{0.0};
	sf::Vector3f position{0.f, 0.f, 0.f};
	for (auto fly=fireflies.begin(), end=fireflies.end()-1; fly != end; ++fly) {
		position += fly->position;
		luminance += fly->color.luminance;
	}
	
	auto& avgFly = fireflies.back();
	avgFly.position = position / static_cast<float>(fireflyCount-1);
	avgFly.color.luminance = luminance / (fireflyCount-1);
}

void Simulation::updateFly(const size_t index, const std::array<Firefly, fireflyCount>& flies) {
	bool noneBrighter{true};
	float lumAdjustment{0.f};
	Firefly& fly{fireflies[index]};
	sf::Vector3f acceleration{0.f, 0.f, 0.f};
	for (size_t _index=0; _index < fireflyCount-1; ++_index) {
		if (index != _index) {
			const Firefly& secondFly{flies[_index]};
			if (noneBrighter && secondFly.color.luminance > fly.color.luminance)
				noneBrighter = false;

			auto distance = fly.position - secondFly.position;
			const float distanceSquared = distance.x*distance.x + distance.y*distance.y + distance.z*distance.z;
			const float length = sqrt(distanceSquared);
			const float force = 100 / distanceSquared;

			distance /= length;       // Make unit vector
			distance *= force;        // Multiply by force=acceleration
			acceleration += distance;

			float adjustment = 1 / distanceSquared;
			lumAdjustment+= secondFly.color.luminance > fly.color.luminance ?
			                adjustment : -adjustment;
		}
	}
#ifdef LUM
std::cerr << "LUM: " << fly.color.luminance << " -> ";
#endif
	fly.color.modifyLuminance(lumAdjustment);
#ifdef LUM
std::cerr << fly.color.luminance << "\n";
#endif
	if (noneBrighter) {
		fly.velocity = sf::Vector3f{velComponent(mt),
		                            velComponent(mt),
		                            velComponent(mt)};
		acceleration = sf::Vector3f{0.f, 0.f, 0.f};
	}

	const float timePassed{fly.clock.getElapsedTime().asSeconds()};
	fly.position += fly.velocity * timePassed;
#ifdef ACC
std::cerr << "\tACC: (" << acceleration.x
          << ", " << acceleration.y
          << ", " << acceleration.z << ")\n";
#endif
#ifdef VEL
std::cerr << "\t\tVEL: (" << fly.velocity.x
          << ", " << fly.velocity.y
          << ", " << fly.velocity.z << ") -> ";
#endif
	fly.velocity += acceleration * timePassed;
#ifdef VEL
std::cerr << "(" << fly.velocity.x
          << ", " << fly.velocity.y
          << ", " << fly.velocity.z << ")\n";
#endif
	outOfBoundsCorrect(fly);
	fly.clock.restart();
}
