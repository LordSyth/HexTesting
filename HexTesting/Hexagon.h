#pragma once
#include <ObjIdl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
using Gdiplus::PointF;
#include <vector>
#define PI 3.1415926535898

//Hexagon class
template <typename Number>
struct _Hex {
	const Number q, r, s;
	_Hex(Number q_, Number r_) : q(q_), r(r_), s(-r_ - q_) {}
	Number& operator[] (unsigned pos) { return (pos == U0) ? q : ((pos == U1) ? r : ((pos == U2) ? s : Number(0.0))); }
	bool operator==(_Hex<Number> h) { return q == h.q && r == h.r && s == h.s; }
	bool operator!=(_Hex<Number> h) { return q != h.q || r != h.r || s != h.s; }
	_Hex<Number> operator+(_Hex<Number> h) { return _Hex<Number>(q + h.q, r + h.r); }
	_Hex<Number> operator-(_Hex<Number> h) { return _Hex<Number>(q - h.q, r - h.r); }
	_Hex<Number> operator*(_Hex<Number> h) { return _Hex<Number>(q * h.q, r * h.r); }
	Number operator*() { return Number((abs(q) + abs(r) + abs(s)) / 2); }
	Number operator|(_Hex<Number> h) { return *_Hex<Number>(*this - h); }
};
typedef _Hex<int> Hex;
typedef _Hex<double> FractionalHex;

//Layout class
struct Orientation {
	const double f0, f1, f2, f3;
	const double b0, b1, b2, b3;
	const double start_angle;
};
const Orientation pointy{
	sqrt(3.0), sqrt(3.0) / 2, 0.0, 3.0 / 2.0,
	sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0,
	0.5 };
const Orientation flat{
	3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0),
	2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0,
	0.0 };
struct Layout {
	const Orientation orientation;
	const PointF size;
	const PointF origin;
};

//Hexagon utility functions
const std::vector<Hex> hexDirections = { Hex(1, 0), Hex(1, -1), Hex(0, -1), Hex(-1, 0), Hex(-1, 1), Hex(0, 1) };
Hex HexNeighbor(Hex h, int direction) { return h + hexDirections[direction]; }
PointF HexToPixel(Layout layout, Hex h) {
	return PointF((layout.orientation.f0 * h.q + layout.orientation.f1 * h.r) * layout.size.X + layout.origin.X,
		(layout.orientation.f2 * h.q + layout.orientation.f3 * h.r) * layout.size.Y + layout.origin.Y);
}
FractionalHex PixelToHex(Layout layout, PointF p) {
	return FractionalHex(
		layout.orientation.b0 * (p.X - layout.origin.X) / layout.size.X
		+ layout.orientation.b1 * (p.Y - layout.origin.Y) / layout.size.Y,
		layout.orientation.b2 * (p.X - layout.origin.X) / layout.size.X
		+ layout.orientation.b3 * (p.Y - layout.origin.Y) / layout.size.Y
	);
}
PointF HexCornerOffset(Layout layout, int corner) {
	return PointF(layout.size.X * cos(2.0 * PI * (layout.orientation.start_angle + corner) / 6),
		layout.size.Y * sin(2.0 * PI * (layout.orientation.start_angle + corner) / 6));
}
std::vector<PointF> PolygonCorners(Layout layout, Hex h) {
	std::vector<PointF> corners;
	PointF center = HexToPixel(layout, h);
	for (int i = 0; i < 6; i++)
		corners.push_back(PointF(center.X + HexCornerOffset(layout, i).X, center.Y + HexCornerOffset(layout, i).Y));
	return corners;
}
Hex HexRound(FractionalHex h) {
	int q = int(round(h.q));
	int r = int(round(h.r));
	int s = int(round(h.s));
	double q_diff = abs(q - h.q);
	double r_diff = abs(r - h.r);
	double s_diff = abs(s - h.s);
	if (q_diff > r_diff && q_diff > s_diff) q = -r - s;
	else if (r_diff > s_diff) r = -q - s;
	return Hex(q, r);
}
double Lerp(double a, double b, double t) { return a * (1 - t) + b * t; }
FractionalHex HexLerp(Hex a, Hex b, double t) { return FractionalHex(Lerp(a.q, b.q, t), Lerp(a.r, b.r, t)); }
std::vector<Hex> HexLinedraw(Hex a, Hex b) {
	int N = a | b;
	std::vector<Hex> results;
	double step = 1.0 / max(N, 1);
	for (int i = 0; i <= N; i++)
		results.push_back(HexRound(HexLerp(a, b, step * i)));
	return results;
}
