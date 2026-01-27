#ifndef DEFINITIONS
#define DEFINITIONS

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <math.h>
using namespace std;

// Game-wide structs etc.
struct Point {
    double X, Y;
};
inline Point PlayerSpawn = {1300, 0};
inline Point TargetPos = {740, 100};
/* Note to future Isaac: "inline" means that these variables can be defined in every file that includes
 definitions.hpp*/

// Game functions
void Update();
void Draw(ALLEGRO_FONT *font, ALLEGRO_MOUSE_STATE state);

// GameObject class, functions
class GameObject {
    public:
        Point Pos = PlayerSpawn;
        Point Momentum = {0, 0};
        Point LastMovement = {0, 0};
        void ApplyPhysics();
        void DecideIncrement();
        void Swing();
        void StartSwinging();
        void StopSwinging();
        void Jump();
        bool UsesPhysics = true;
        bool Swinging = false;
        double FallingVelocity = 0; // only used on falling physics objects
        double AngleIncrement = 0.013 * M_PI; // Radians, only used on swinging physics objects
        double CurrentAngle; // Testing purposes
        float RopeAmplitude = 0;
        bool Stationary = false;
        bool BlockSwing = false;
        double IncrementIncrement = 0.0005 * M_PI;
        bool SwingingRight;
        bool CanJump;
        double JumpCharge = 1.0;
};

class Laser {
    public:
        bool Active = false;

        int Orientation; // 1: Vertical; 2: Horizontal
        double Pos; // 0 <= Pos <= 1; The percentage of the screen width or height that the laser is at
        bool Dangerous;
};

enum SwingTypes {
    RIGHT,
    LEFT,
    HANG
};

// Utility Functions
double DecideAngle(Point Pos, Point Origin);
Point DecidePoint(Point Origin, double Angle, double DistanceFromOrigin);

#endif
