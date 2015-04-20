#ifndef HSL_COLOR
#define HSL_COLOR

#include <algorithm>
#include <cmath>
#include <SFML/Graphics/Color.hpp>

struct HSL {
	private:
	double hueToRGB(double arg1, double arg2, double H);

	public:
	double hue{0.0};
	double saturation{0.0};
	double luminance{0.0};

	HSL();
	HSL(int H, int S, int L);
	HSL(const HSL& hsl);
	HSL& operator=(const HSL& rhs);
	bool operator==(const HSL rhs) const;
	bool operator!=(const HSL rhs) const;
	void modifyLuminance(double change);
	sf::Color turnToRGB();
};

HSL turnToHSL(const sf::Color& C);

#endif
