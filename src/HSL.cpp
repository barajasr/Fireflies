#include "HSL.hpp"

const double D_EPSILON = 0.00000000000001; 

HSL::HSL() : hue{0}, saturation{0}, luminance{0} {}

HSL::HSL(const HSL& hsl)
: hue{hsl.hue}, saturation{hsl.saturation}, luminance{hsl.luminance} {}

HSL& HSL::operator=(const HSL& rhs) {
	hue = rhs.hue;
	saturation = rhs.saturation;
	luminance = rhs.luminance;
	return *this;
}

bool HSL::operator==(const HSL rhs) const {
	return hue == rhs.hue
		   && saturation == rhs.saturation
		   && luminance == rhs.luminance;
}

bool HSL::operator!=(const HSL rhs) const {
	return hue != rhs.hue
		   || saturation != rhs.saturation
		   || luminance != rhs.luminance;
}

void HSL::modifyLuminance(double change) {
	luminance += change;
	if (luminance > 100) luminance = 100;
	else if (luminance < 0) luminance = 0;
}

HSL::HSL(int H, int S, int L) {
	///Range control for hue.
	if (H <= 360 && H >= 0) {
		hue = H;
	} else {
		if(H > 360) {
			hue = H%360;
		} else if(H < 0 && H > -360) {
			hue = -H; 
		} else if(H < 0 && H < -360) {
			hue = -(H%360);
		}
	}

	///Range control for saturation.
	if (S <= 100 && S >= 0) {
		saturation = S;
	} else {
		if(S > 100) {
			saturation = S%100;
		} else if(S < 0 && S > -100) {
			saturation = -S;
		} else if(S < 0 && S < -100) {
			saturation = -(S%100);
		}
	}

	///Range control for luminance
	if (L <= 100 && L >= 0) {
		luminance = L;
	} else {
		if(L > 100) { luminance = L%100;}
		if(L < 0 && L > -100) { luminance = -L; }
		if(L < 0 && L < -100) { luminance = -(L%100); }
	}
}

double HSL::hueToRGB(double arg1, double arg2, double H) {
	if (H < 0) {
		H += 1;
	} else if (H > 1) {
		H -= 1;
	}

	if (6*H < 1) {
		return arg1 + (arg2 - arg1)*6*H;
	} else if (2*H < 1) {
		return arg2;
	} else if (3*H < 2) {
		return arg1 + (arg2 - arg1) * ((2.0 / 3.0) - H)*6;
	}

	return arg1;
}

sf::Color HSL::turnToRGB() {
	///Reconvert to range [0,1]
	double H = hue/360.0;
	double S = saturation/100.0;
	double L = luminance/100.0;

	float arg1, arg2;

	if (S <= D_EPSILON) {
		sf::Color C(L*255, L*255, L*255);
		return C;
	}

	if (L < 0.5) {
		arg2 = L * (1 + S);
	} else {
		arg2 = (L + S) - (S * L);
	}

	arg1 = 2 * L - arg2;
	sf::Uint8 r =(255 * hueToRGB(arg1, arg2, (H + 1.0/3.0)));
	sf::Uint8 g =(255 * hueToRGB(arg1, arg2, H));
	sf::Uint8 b =(255 * hueToRGB(arg1, arg2, (H - 1.0/3.0)));
	sf::Color C(r,g,b);
	return C;
}

HSL turnToHSL(const sf::Color& C) {
	// Trivial cases
	if(C == sf::Color::White)  return HSL(0, 0, 100);
	if(C == sf::Color::Black)  return HSL(0, 0, 0);
	if(C == sf::Color::Red)    return HSL(0, 100, 50);
	if(C == sf::Color::Yellow) return HSL(60, 100, 50);
	if(C == sf::Color::Green)  return HSL(120, 100, 50);
	if(C == sf::Color::Cyan)   return HSL(180, 100, 50);
	if(C == sf::Color::Blue)   return HSL(240, 100, 50);
	if(C == sf::Color::Cyan)   return HSL(300, 100, 50);

	double R, G, B;
	R = C.r/255.0;
	G = C.g/255.0;
	B = C.b/255.0;
	// Nontrivial cases
	double max{0.0}, min{0.0}, l{0.0}, s{0.0};

	max = std::max(std::max(R,G),B);
	min = std::min(std::min(R,G),B);


	HSL A;
	l = ((max + min)/2.0);

	if (max - min <= D_EPSILON ) {
		A.hue = 0;
		A.saturation = 0;
	} else {
		double diff = max - min;
		if(A.luminance < 0.5) {
			s = diff/(max + min); }
		else {
			s = diff/(2 - max - min); 
		}

		double diffR = (((max - R) * 60) + (diff/2.0)) / diff;
		double diffG = (((max - G) * 60) + (diff/2.0)) / diff;
		double diffB = (((max - B) * 60) + (diff/2.0)) / diff;


		if (max - R <= D_EPSILON) {
			A.hue = diffB - diffG;
		} else if ( max - G <= D_EPSILON ) {
			A.hue = (1*360)/3.0 + (diffR - diffB);
		} else if ( max - B <= D_EPSILON ) {
			A.hue = (2*360)/3.0 + (diffG - diffR);
		}

		if (A.hue <= 0 || A.hue >= 360) fmod(A.hue, 360);
		s *= 100;
	}

	l *= 100;
	A.saturation = s;
	A.luminance = l;
	return A;
}
