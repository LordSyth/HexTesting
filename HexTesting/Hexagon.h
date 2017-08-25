#pragma once
#include <gdipluscolor.h>
#include <vector>

struct Hex {
	const union { int q, r, s; int vector[3]; };
	Hex(int q_, int r_) : q(q_), r(r_), s(-r_ - q_) {}
	int& operator[] (unsigned pos) { return vector[pos]; }
	bool operator==(Hex h) { return q == h.q && r == h.r && s == h.s; }
	bool operator!=(Hex h) { return q != h.q || r != h.r || s != h.s; }
	Hex operator+(Hex h) { return Hex(q + h.q, r + h.r); }
	Hex operator-(Hex h) { return Hex(q - h.q, r - h.r); }
	Hex operator*(Hex h) { return Hex(q * h.q, r * h.r); }
	int operator*() { return int((abs(q) + abs(r) + abs(s)) / 2); }
	int operator|(Hex h) { return *Hex(*this - h); }
};

const std::vector<Hex> hex_directions = {
	Hex(1, 0), Hex(1, -1), Hex(0, -1),
	Hex(-1, 0), Hex(-1, 1), Hex(0, 1)
};

Hex hex_neighbor(Hex h, int direction) { return h + hex_directions[direction]; }
