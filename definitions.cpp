#include "definitions.hpp"

// GameObject functions
double DecideAngle(Point Pos, Point Origin) {
    double Y = (Origin.Y - Pos.Y);
    double X = -(Origin.X - Pos.X);

    if (X == 0) {
        if (Y > 0) return M_PI / 2;
        else { return 3 * M_PI / 2; };
    }

    return atan2(Y, X);
};

Point DecidePoint(Point Origin, double Angle, double DistanceFromOrigin) {
    double X = DistanceFromOrigin * cos(Angle) + Origin.X;
    double Y = -DistanceFromOrigin * sin(Angle) + Origin.Y;
    return {X, Y};
};
