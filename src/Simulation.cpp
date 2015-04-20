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
	for (auto fly=fireflies.begin(); fly != fireflies.end(); ++fly) {
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
	
	auto color = turnToHSL(sf::Color::Yellow);
	for (auto fly=fireflies.begin(), end=fireflies.end()-1; fly != end; ++fly) {
		fly->color = color;
		fly->color.luminance = luminance(mt);
		fly->position = {coordinate(mt), coordinate(mt), coordinate(mt)};
		fly->velocity = {velComponent(mt), velComponent(mt), velComponent(mt)};

#ifdef DEBUG
std::cerr << "(" << fly->color.hue
          << ", " << fly->color.saturation
          << ", " << fly->color.luminance << ")\n";
#endif
	}

#ifdef DEBUG
std::cerr << "===================================================\n";
#endif

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

	while (window.isOpen()) {
		for (sf::Event event; window.pollEvent(event); )
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
	float luminance{0.0};
	sf::Vector3f position{0.f, 0.f, 0.f};
	for (auto fly=fireflies.begin(), end=fireflies.end()-1; fly != end; ++fly) {
		position += fly->position;
		luminance += fly->color.luminance;
	}
	
	auto& avgFly = fireflies.back();
	avgFly.position = position / static_cast<float>(fireflyCount-1);
	avgFly.color.luminance = luminance / (fireflyCount-1);

	for (auto fly=fireflies.begin(), end=fireflies.end()-1; fly != end; ++fly) {
		updateFly(*fly, luminance, position);
	}
	

	sortByDepth();
#ifdef DEBUG
std::cerr << "++++++++++++++++++++++++++++++++++\n";
#endif
}

void Simulation::updateFly(Firefly& fly, const float& luminance, const sf::Vector3f& position) {
	static const size_t count = fireflyCount - 2;
	const float avgX = (position.x - fly.position.x) / count;
	const float avgY = (position.y - fly.position.y) / count;
	const float avgZ = (position.z - fly.position.z) / count;
	const float avgLuminance = (luminance - fly.color.luminance) / count;

#ifdef DEBUG
std::cerr << "fly.color.luminance: " << fly.color.luminance
		  << ", avgLuminance: " <<  avgLuminance << std::endl;
#endif

	if (avgLuminance > fly.color.luminance) {
		auto distance = fly.position - sf::Vector3f(avgX, avgY, avgZ);
		const float distanceSquared = distance.x*distance.x + distance.y*distance.y + distance.z*distance.z;
		const float length = sqrt(distanceSquared);
		const float force = 100 / distanceSquared;

		distance /= length; // Make unit vector
		distance *= force;  // Multiply by force
#ifdef DEBUG
std::cerr << "\tdistance: (" << distance.x
		  << ", " << distance.y
		  << ", " << distance.z << ")\n"
		  << "\tfly.velocity: (" << fly.velocity.x
		  << ", " << fly.velocity.y
		  << ", " << fly.velocity.z <<") -> ";
#endif
		fly.velocity += distance; // Apply force
		fly.color.modifyLuminance(1/ (avgLuminance - fly.color.luminance));
#ifdef DEBUG
std::cerr << "(" << fly.velocity.x
		  << ", " << fly.velocity.y
		  << ", " << fly.velocity.z << ")\n";
#endif
	} else {
		auto newVelocity = sf::Vector3f{velComponent(mt),
										velComponent(mt),
										velComponent(mt)};
#ifdef DEBUG
std::cerr << "\tnewVelocity: (" << newVelocity.x
		  << ", " << newVelocity.y
		  << ", " << newVelocity.z << ")\n";
#endif
		fly.velocity = newVelocity;
		fly.color.modifyLuminance(1/ (avgLuminance - fly.color.luminance));

	}
#ifdef DEBUG
std::cerr << "HSL: (" << fly.color.hue
	  << ", " << fly.color.saturation
	  << ", " << fly.color.luminance << ")\n";
std::cerr << "----------------------------------\n";
#endif

	fly.position += fly.velocity * fly.clock.getElapsedTime().asSeconds();
	outOfBoundsCorrect(fly);
	fly.clock.restart();
}
