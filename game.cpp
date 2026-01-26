#include <cstdlib>

#include "definitions.hpp"

GameObject Player;
Laser ActiveLaser;

void update();
void TimerEvents();
void FireLaser();
void draw(ALLEGRO_FONT *Font);

Point MousePos = {0, 0};

Point ScreenSize = {1480, 800};

int main()
{
    if (!al_init() || !al_install_mouse()) {
        std::cerr << "Failed to initialize Allegro.\n";
        return -1;
    }

    al_install_keyboard();
    
    const double FPS = 60.0;
    ALLEGRO_TIMER* Timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE* Queue = al_create_event_queue();
    ALLEGRO_DISPLAY* Disp = al_create_display(ScreenSize.X, ScreenSize.Y);
    ALLEGRO_FONT* Font = al_create_builtin_font();

    if (!Timer || !Queue || !Disp || !Font) {
        std::cerr << "Failed to create Allegro objects.\n";
        return -1;
    }

    al_register_event_source(Queue, al_get_keyboard_event_source());
    al_register_event_source(Queue, al_get_display_event_source(Disp));
    al_register_event_source(Queue, al_get_timer_event_source(Timer));

    bool Redraw = true;
    bool Running = true;
    ALLEGRO_EVENT Event;

    al_start_timer(Timer);

    // Keyboard Input Stuff
    #define KEY_SEEN    1 // Key is either currently down or has been down but hasn't been dealt with by game logic
    #define KEY_DOWN    2 // Key is currently down

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    while (Running)
    {
        al_wait_for_event(Queue, &Event);
        ALLEGRO_MOUSE_STATE state;

        switch (Event.type) {
            case ALLEGRO_EVENT_TIMER:
                al_get_mouse_state(&state);

                Update();
                Redraw = true;

                // Check if a key was pressed
                if(key[ALLEGRO_KEY_Z])
                    Player.StartSwinging();
                else if (Player.Swinging)
                    Player.StopSwinging();

                if(key[ALLEGRO_KEY_X] && Player.CanJump)
                    Player.Jump();

                if(key[ALLEGRO_KEY_R])
                {
                    Player.Pos = PlayerSpawn;
                    Player.Swinging = false;
                    Player.LastMovement = {0, 0};
                    Player.Momentum = {0, 0};
                    Player.FallingVelocity = 0;
                }

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= ~KEY_SEEN;

                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                key[Event.keyboard.keycode] = KEY_SEEN | KEY_DOWN;

                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[Event.keyboard.keycode] &= ~KEY_DOWN;

                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                Running = false;

                break;
        }

        if (Redraw && al_is_event_queue_empty(Queue))
        {
            Draw(Font, state);
            Redraw = false;
        }
    }

    al_destroy_font(Font);
    al_destroy_display(Disp);
    al_destroy_timer(Timer);
    al_destroy_event_queue(Queue);

    return 0;
}

void Update()
{
    Player.ApplyPhysics();

    TimerEvents();
}

int laserTimer = 0;
int activeLaserTimer = 0;
void TimerEvents()
{
    // Fire a laser?
    if (laserTimer == 0)
    {
        FireLaser();
        laserTimer = (rand() % 31) + 10; // Randum num between 10 and 40
    } else if (!ActiveLaser.Active)
    {
        laserTimer--;
    }

    // If a laser is active
    if (ActiveLaser.Active) {
        switch (activeLaserTimer) {
            case 3:
                ActiveLaser.Dangerous = true;
                break;
            case 10:
                ActiveLaser.Active = false;
                break;
        }

        activeLaserTimer++;
    }
}

void FireLaser() {
    ActiveLaser.Active = true;
    ActiveLaser.Dangerous = false;
    int orientation = rand() % 3; // 2: Vertical; 1: Horizontal
    ActiveLaser.Orientation = orientation;
    ActiveLaser.Pos = rand() % 71 + 15;
}

void Draw(ALLEGRO_FONT *Font, ALLEGRO_MOUSE_STATE state)
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    // Top left text
    al_draw_text(Font, al_map_rgb(255, 255, 255), 5, 5, 0, (std::to_string(Player.AngleIncrement) + "; ").c_str());

    // Draw Player
    al_draw_filled_rounded_rectangle(Player.Pos.X - 15, Player.Pos.Y - 15,
                                     Player.Pos.X + 15, Player.Pos.Y + 15,
                                     5, 5, al_map_rgb(255, 255, 255));
    
    // Draw Target
    al_draw_filled_rounded_rectangle(TargetPos.X - 15, TargetPos.Y - 15,
                                     TargetPos.X + 15, TargetPos.Y + 15,
                                     5, 5, al_map_rgb(191, 63, 82));

    // Draw Laser
    if (ActiveLaser.Active) {
        switch (ActiveLaser.Orientation) {
            case 1:
                // Vertical
                {
                    float xPos = ActiveLaser.Pos * ScreenSize.X;
                    al_draw_line(xPos, 0, xPos, ScreenSize.Y, al_map_rgb(191, 63, 82), 4);
                    break;
                }
            case 2:
                // Horizontal
                float yPos = ActiveLaser.Pos * ScreenSize.Y;
                al_draw_line(0, yPos, ScreenSize.X, yPos, al_map_rgb(191, 63, 82), 4);
                break;
        } 
    }

    // Draw curved line
    if (Player.Swinging) {
        // Calculate the midpoint and distance
        float dx = TargetPos.X - Player.Pos.X;
        float dy = TargetPos.Y - Player.Pos.Y;
        float distance = sqrt(dx * dx + dy * dy);

        // Define the arc height (adjust this to control how curved the arc is)
        float arc_height = distance * 0.3f; // 30% of distance, adjust as needed

        // Number of segments for smooth curve
        int num_segments = 20;

        // Draw the parabolic arc using connected line segments
        for (int i = 0; i < num_segments; i++) {
            // Parameter t goes from 0 to 1
            float t1 = (float)i / num_segments;
            float t2 = (float)(i + 1) / num_segments;
            
            // Linear interpolation for x and y
            float x1 = Player.Pos.X + dx * t1;
            float y1 = Player.Pos.Y + dy * t1;
            float x2 = Player.Pos.X + dx * t2;
            float y2 = Player.Pos.Y + dy * t2;
            
            // Add parabolic offset perpendicular to the line
            // Parabola peaks at t=0.5
            float offsetAmplitude = Player.RopeAmplitude;
            float offset1 = offsetAmplitude * arc_height * t1 * (1 - t1); // Parabola formula: 4h*t*(1-t)
            float offset2 = offsetAmplitude * arc_height * t2 * (1 - t2);
            
            // Calculate perpendicular direction (rotate 90 degrees)
            float perpX = -dy / distance;
            float perpY = dx / distance;
            
            // Apply offset perpendicular to the line
            x1 += perpX * offset1;
            y1 += perpY * offset1;
            x2 += perpX * offset2;
            y2 += perpY * offset2;
            
            // Draw line segment
            al_draw_line(x1, y1, x2, y2, al_map_rgb(255, 255, 255), 3);
        }
    }

    al_flip_display();
}
