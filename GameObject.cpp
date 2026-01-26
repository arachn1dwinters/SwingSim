#include "definitions.hpp"

void MoveTowardsZero(double* number, double changeBy);

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

    if (fabs(AngleIncrement) < 1e-10) {
        if (fabs(dx) < 10) {
            AngleIncrement = 0;
            Stationary = true;
        } else {
            // Change Direction
            SwingingRight = !SwingingRight;
        }
    }
};

void GameObject::Swing() {
    if (!Stationary) {
        Point DistanceFromTarget = {TargetPos.X - Pos.X, TargetPos.Y - Pos.Y};
        double RopeLength = hypot(DistanceFromTarget.X, DistanceFromTarget.Y);
        
        DecideIncrement();

        CurrentAngle = DecideAngle(Pos, TargetPos) + AngleIncrement;
        Point NewPos = DecidePoint(TargetPos, CurrentAngle, RopeLength);

        if (NewPos.Y < 740 && NewPos.Y > 60 && NewPos.X < 1420 && NewPos.X > 60)
        {
            Pos = NewPos;
        } else {
            Swinging = false;
            // Long story. couldn't figure out clamp.

            if (NewPos.X < 60) 
                NewPos.X = 60;
            if (NewPos.X > 1420)
                NewPos.X = 1420;
            if (NewPos.Y < 60)
                NewPos.Y = 60;
            if (NewPos.Y > 740)
                NewPos.Y = 740;
            Pos = NewPos;
        }
    }
}

void GameObject::ApplyPhysics() {
    Point OldPos = Pos;

    if (UsesPhysics) {
        if (!Swinging) {
            if (Pos.Y < 740) {
                FallingVelocity += 1;
                Pos.Y += FallingVelocity;
            }

            if (Momentum.X != 0 || Momentum.Y != 0) {
                double changeMomentumBy = 0.5;

                if (Pos.X < 1420 && Pos.X > 60) {
                    Pos.X += Momentum.X;
                }

                MoveTowardsZero(&Momentum.X, changeMomentumBy);
                if (Pos.Y < 740 && Pos.Y > 60) {
                    Pos.Y += Momentum.Y;
                }
                MoveTowardsZero(&Momentum.Y, changeMomentumBy);
            }

            if (JumpCharge < 1.0)
                JumpCharge += 0.02;
            else
                CanJump = true;
        } else {
            Swing();
        }
    }

    LastMovement = {Pos.X - OldPos.X, Pos.Y - OldPos.Y};
};

void GameObject::StartSwinging() {
    if (!Swinging) {
        double dx = TargetPos.X - Pos.X;
        SwingingRight = dx > 0;
        Swinging = true;
        FallingVelocity = 0;
        AngleIncrement = 0.013 * M_PI;
    }
}

void GameObject::StopSwinging() {
    if (Swinging) {
        Momentum = {LastMovement.X, LastMovement.Y};
        Swinging = false;
    }
}

void MoveTowardsZero(double* number, double changeBy)
{
    if (*number > 0) {
        *number -= *number > changeBy ? changeBy : *number;
    } else {
        *number += abs(*number) > changeBy ? changeBy : abs(*number);
    }
}

void GameObject::Jump() {
    Momentum.Y -= 30;
    FallingVelocity = 0;
    JumpCharge = 0;
    CanJump = false;
}
