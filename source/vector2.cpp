// COMP710 GP Framework 2025

// This include:
#include "vector2.h"

// Local includes:
#include "renderer.h"

// Library includes:
#include <cassert>
#include <cmath>

Vector2::Vector2()
	: x(0.0f)
	, y(0.0f)
{

}

Vector2::~Vector2()
{

}

Vector2::Vector2(float inx, float iny)
	: x(inx)
	, y(iny)
{

}

void
Vector2::Set(float inx, float iny)
{
	x = inx; y = iny;
}

float
Vector2::LengthSquared() const
{
	return ((x * x) + (y * y));
}

float
Vector2::Length() const
{
	return (sqrtf(LengthSquared()));
}

void Vector2::Normalise()
{
	float length = Length();
	if (length > 0.0f)
	{
		x = x / length; 
		y = y / length;
	}
}

float
Vector2::DotProduct(const Vector2& veca, const Vector2& vecb)
{
	return (veca.x * vecb.x) + (veca.y * vecb.y);
}

Vector2
Vector2::Lerp(const Vector2& veca, const Vector2& vecb, float time)
{
	return (veca + time * (vecb - veca));
}

Vector2
Vector2::Reflect(const Vector2& vec, const Vector2& normal)
{
	return (vec - 2.0f * Vector2::DotProduct(vec, normal) * normal);
}

// Operator implementations
Vector2& Vector2::operator*=(float scalar)
{
	x = x * scalar; 
	y = y * scalar; 
	return (*this);
}

Vector2& Vector2::operator+=(const Vector2& vecRight)
{
	x = x + vecRight.x; 
	y = y + vecRight.y; 
	return (*this);
}

Vector2& Vector2::operator-=(const Vector2& vecRight)
{
	x = x - vecRight.x; 
	y = y - vecRight.y; 
	return (*this);
}

// Friend function implementations
Vector2 operator+(const Vector2& veca, const Vector2& vecb)
{
	return Vector2(veca.x + vecb.x, veca.y + vecb.y);
}

Vector2 operator-(const Vector2& veca, const Vector2& vecb)
{
	return Vector2(veca.x - vecb.x, veca.y - vecb.y);
}

Vector2 operator*(float scalar, const Vector2& vec)
{
	return Vector2(vec.x * scalar, vec.y * scalar);
}

Vector2 operator*(const Vector2& vec, float scalar)
{
	return Vector2(vec.x * scalar, vec.y * scalar);
}

bool operator==(const Vector2& veca, const Vector2& vecb)
{
	return (veca.x == vecb.x && veca.y == vecb.y);
}

bool operator!=(const Vector2& veca, const Vector2& vecb)
{
	return !(veca == vecb);
}
