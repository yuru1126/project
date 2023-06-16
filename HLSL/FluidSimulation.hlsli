#define X_SIZE 12
#define Y_SIZE 12

//æZ—pdefine
#define XY_MU 13
#define YY_MU 12

struct Velocity
{
    float velocity;
    float after;
};

cbuffer CbFluid : register(b8)
{
    float deltaT;
    float omega;
    float re;
    
    float fluid_dummy;
};

//buffer‚Ì”z—ñ”Ô†ŒvZŠÖ”
uint XNArray(uint x, uint y)
{
    return x + y * XY_MU;
}
uint YNArray(uint x, uint y)
{
    return x + y * YY_MU;
}

uint NArray(uint x, uint y)
{
    return x + y * X_SIZE;
}