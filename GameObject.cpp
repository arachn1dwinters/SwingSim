#include "definitions.hpp"

// Because of convenience this also decides the rope amplitude
void GameObject::DecideIncrement() {
    AngleIncrement = fabs(AngleIncrement);

    Point DistanceFromTarget = {TargetPos.X - Pos.X, TargetPos.Y - Pos.Y};
    double dx = DistanceFromTarget.X;
    double RopeLength = hypot(DistanceFromTarget.X, DistanceFromTarget.Y);

    // When DistanceFromTarget is the RopeLength we want the RopeAmplitude to be 2, when it is 0 we want it to be 0
    RopeAmplitude = dx / RopeLength * 2;

    if ((SwingingRight && dx < 0) || (!SwingingRight && dx > 0)) {
        // Decrease AngleIncrement
        AngleIncrement -= IncrementIncrement;
    } else if ((SwingingRight && dx > 0) || (!SwingingRight && dx < 0)) {
        // Increase AngleIncrement
        AngleIncrement += IncrementIncrement;
    }

    if (!SwingingRight) AngleIncrement = -AngleIncrement;

    RopeAmplitude = (dx / RopeLength) * 2;

    if (fabs(AngleIncrement) < 1e-10 && fabs(dx) < 10) {
        AngleIncrement = 0;
        Stationary = true;
    } else if (fabs(AngleIncrement) < 1e-10 && RopeLength - fabs(dx) < 0.001) {
        // Change Direction
        double DampingFactor = 0.99;
        AngleIncrement *= DampingFactor;

        SwingingRight = !SwingingRight;
    }
};

void GameObject::Swing() {
    if (!Stationary) {
        Point DistanceFromTarget = {TargetPos.X - Pos.X, TargetPos.Y - Pos.Y};
        double RopeLength = hypot(DistanceFromTarget.X, DistanceFromTarget.Y);
        
        DecideIncrement();
        CurrentAngle = DecideAngle(Pos, TargetPos) + AngleIncrement;
        Pos = DecidePoint(TargetPos, CurrentAngle, RopeLength);
    }
}

void GameObject::ApplyPhysics() {
    if (UsesPhysics) {
        if (!Swinging) {
            FallingVelocity += 1;
            Pos.Y += FallingVelocity;
        } else {
            Swing();
        }
    } 
};

void GameObject::StartSwinging() {
    if (!Swinging) {
        double dx = TargetPos.X - Pos.X;
        SwingingRight = dx > 0;

        Swinging = true;
    }
}
