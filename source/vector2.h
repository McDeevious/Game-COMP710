// COMP710 GP Framework 2025
#ifndef VECTOR2_H
#define VECTOR2_H

// Forward declarations for operators
class Vector2;
bool operator==(const Vector2& veca, const Vector2& vecb);
bool operator!=(const Vector2& veca, const Vector2& vecb);
Vector2 operator+(const Vector2& veca, const Vector2& vecb);
Vector2 operator-(const Vector2& veca, const Vector2& vecb);
Vector2 operator*(float scalar, const Vector2& vec);
Vector2 operator*(const Vector2& vec, float scalar);

// Class declaration:
class Vector2
{
public:
	Vector2();
	Vector2(float inx, float iny);
	~Vector2();

	void Set(float inx, float iny);

	float LengthSquared() const; 
	float Length() const;

	void Normalise();

	friend Vector2 operator+(const Vector2& veca, const Vector2& vecb);
	friend Vector2 operator-(const Vector2& veca, const Vector2& vecb);
	friend Vector2 operator*(float scalar, const Vector2& vec);
	friend Vector2 operator*(const Vector2& vec, float scalar);
	friend bool operator==(const Vector2& veca, const Vector2& vecb);
	friend bool operator!=(const Vector2& veca, const Vector2& vecb);

	Vector2& operator*=(float scalar);
	Vector2& operator+=(const Vector2& vecRight);
	Vector2& operator-=(const Vector2& vecRight);
	
	static float DotProduct(const Vector2& veca, const Vector2& vecb);
	static Vector2 Lerp(const Vector2& veca, const Vector2& vecb, float time); 
	static Vector2 Reflect(const Vector2& veca, const Vector2& vecb);

	// Member data:
	float x; 
	float y;

private:
	//Vector2(const Vector2& vector2);
	//Vector2 & operator=(const Vector2 & vector2);
};

#endif // VECTOR2_H 
