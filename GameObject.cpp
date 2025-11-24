#include "definitions.hpp"

// Because of convenience this also decides the rope amplitude
void GameObject::DecideIncrement(double RopeLength) {
    int FinalIncrementMultiplier = CurrentlySwingingRight ? 1 : -1;
    
    if (AngleIncrement == 0) {
        // Flip direction
        FinalIncrementMultiplier = -FinalIncrementMultiplier;
        CurrentlySwingingRight = !CurrentlySwingingRight;
    }

    int RopeAmplitudeMultipler = FinalIncrementMultiplier;

    bool DirectionChanged = (AngleIncrement > 0 && FinalIncrementMultiplier == -1) || 
                            (AngleIncrement < 0 && FinalIncrementMultiplier == 1);

    Point DistanceFromTarget = {TargetPos.X - Pos.X, TargetPos.Y - TargetPos.Y};
    int IncrementIncrementMultiplier;
    if ((DistanceFromTarget.X > 0 && !CurrentlySwingingRight) || (DistanceFromTarget.X < 0 && CurrentlySwingingRight)) {
        IncrementIncrementMultiplier = -1;
    } else if ((DistanceFromTarget.X > 0 && CurrentlySwingingRight) || (DistanceFromTarget.X < 0  && !CurrentlySwingingRight)) {
        IncrementIncrementMultiplier = 1;
    }

    // When DistanceFromTarget is the RopeLength we want the RopeAmplitude to be 2, when it is 0 we want it to be 0
    RopeAmplitude = DistanceFromTarget.X / RopeLength * 2;

    double LocalIncrementIncrement = IncrementIncrement * M_PI * IncrementIncrementMultiplier;
    AngleIncrement = !Stationary ? FinalIncrementMultiplier * (AngleIncrement + LocalIncrementIncrement) : 0;

    if (DirectionChanged) {
        double DampingFactor = 0.99;
        AngleIncrement *= DampingFactor;
    }

    if (fabs(AngleIncrement) < 0.001 && fabs(DistanceFromTarget.X) < 10) {
        AngleIncrement = 0;
        Stationary = true;
    }
};

void GameObject::Swing() {
    if (!Stationary) {
        Point DistanceFromTarget = {TargetPos.X - Pos.X, TargetPos.Y - Pos.Y};
        double RopeLength = sqrt(pow(DistanceFromTarget.X, 2) + pow(DistanceFromTarget.Y, 2));
        
        DecideIncrement(RopeLength);
        CurrentAngle = DecideAngle(Pos, TargetPos);
        CurrentAngle = CurrentAngle + AngleIncrement;
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
