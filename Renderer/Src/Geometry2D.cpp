/***********************************************************************\
|																		|
|	File:			SGD_Geometry.h  									|
|	Author:			Douglas Monroe                                      |
|   Modified By     Chen Lu                                             |
|	Last Modified:	2016-3-4											|
|																		|
|	Purpose:		To store rectangle components						|
|					left, top, right, bottom							|
|																		|
|					To store point components							|
|					x, y												|
|																		|
|					To store size components							|
|					width, height										|
|																		|
|					To store vector components							|
|					x, y												|
|																		|
|	© 2014 Full Sail, Inc. All rights reserved. The terms "Full Sail", 	|
|	"Full Sail University", and the Full Sail University logo are 	   	|
|	either registered service marks or service marks of Full Sail, Inc.	|
|																		|
\***********************************************************************/
#include "stdafx.h"
#include "../Inc/Geometry2D.h"

// Uses sqrtf
#include <cmath>

// Uses _isnan
#include <cfloat>

namespace G2D {
    //*****************************************************************//
    // Rotation is in radians (as a multiple of pi)
    const float PI = 3.1415926535f;

    //*****************************************************************//
    // RECTANGLE METHODS

    // Default constructor (empty)
    Rectangle::Rectangle()
        : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {
    }

    // Overloaded constructor
    Rectangle::Rectangle(float L, float T, float R, float B)
        : left(L), top(T), right(R), bottom(B) {
    }

    // Overloaded Point-Point constructor
    Rectangle::Rectangle(const Point& topleft, const Point& bottomright)
        : left(topleft.x), top(topleft.y), right(bottomright.x), bottom(bottomright.y) {
    }

    // Overloaded Point-Size constructor
    Rectangle::Rectangle(const Point& topleft, const Size& size)
        : left(topleft.x), top(topleft.y), right(topleft.x + size.width), bottom(topleft.y + size.height) {
    }


    // Accessors
    Point Rectangle::GetTopLeft() const { return{ left, top }; }
    Point Rectangle::GetTopRight() const { return{ right, top }; }
    Point Rectangle::GetBottomLeft() const { return{ left, bottom }; }
    Point Rectangle::GetBottomRight() const { return{ right, bottom }; }

    // Mutators
    void Rectangle::SetTopLeft(const Point& topleft) { left = topleft.x;		top = topleft.y; }
    void Rectangle::SetTopRight(const Point& topright) { right = topright.x;		top = topright.y; }
    void Rectangle::SetBottomLeft(const Point& bottomleft) { left = bottomleft.x;	bottom = bottomleft.y; }
    void Rectangle::SetBottomRight(const Point& bottomright) { right = bottomright.x;	bottom = bottomright.y; }


    // Helpers
    Point Rectangle::ComputeCenter() const { return{ (left + right) / 2, (top + bottom) / 2 }; }
    Size Rectangle::ComputeSize() const { return{ right - left, bottom - top }; }
    float Rectangle::ComputeWidth() const { return (right - left); }
    float Rectangle::ComputeHeight() const { return (bottom - top); }

    bool Rectangle::IsEmpty() const { return ((left >= right) || (top >= bottom)); }

    bool Rectangle::IsPointInRectangle(const Point& point) const {
        return ((point.x >= left && point.x < right) && (point.y >= top && point.y < bottom));
    }

    bool Rectangle::IsIntersecting(const Rectangle& other) const {
        return ((this->left < other.right) && (this->right > other.left) && (this->top < other.bottom) && (this->bottom > other.top)
            && (this->left < this->right) && (this->top < this->bottom) && (other.left < other.right) && (other.top < other.bottom));
    }

    Rectangle Rectangle::ComputeIntersection(const Rectangle& other) const {
        Rectangle overlap;
        overlap.top = (this->top	> other.top) ? this->top : other.top;		// max
        overlap.left = (this->left	> other.left) ? this->left : other.left;		// max
        overlap.bottom = (this->bottom < other.bottom) ? this->bottom : other.bottom;		// min
        overlap.right = (this->right	< other.right) ? this->right : other.right;		// min

        if ( overlap.IsEmpty() == false )
            return overlap;
        return Rectangle{};
    }

    Rectangle Rectangle::ComputeUnion(const Rectangle& other) const {
        Rectangle join;
        join.top = (this->top	< other.top) ? this->top : other.top;		// min
        join.left = (this->left	< other.left) ? this->left : other.left;		// min
        join.bottom = (this->bottom > other.bottom) ? this->bottom : other.bottom;		// max
        join.right = (this->right	> other.right) ? this->right : other.right;		// max
        return join;
    }

    // Normalize - does something
    void Rectangle::Normalize() {
        if ( left > right ) {
            float l = left;
            left = right;
            right = l;
        }

        if ( top > bottom ) {
            float t = top;
            top = bottom;
            bottom = t;
        }
    }

    void Rectangle::MoveTo(float L, float T) {
        right += (L - left);
        bottom += (T - top);
        left = L;
        top = T;
    }

    void Rectangle::MoveTo(const Point& topleft) {
        right += (topleft.x - left);
        bottom += (topleft.y - top);
        left = topleft.x;
        top = topleft.y;
    }

    void Rectangle::Offset(float deltaX, float deltaY) {
        left += deltaX;
        right += deltaX;
        top += deltaY;
        bottom += deltaY;
    }

    void Rectangle::Offset(const Vector& offset) {
        left += offset.x;
        right += offset.x;
        top += offset.y;
        bottom += offset.y;
    }

    void Rectangle::Resize(float width, float height) {
        right = left + width;
        bottom = top + height;
    }

    void Rectangle::Resize(const Size& size) {
        right = left + size.width;
        bottom = top + size.height;
    }

    void Rectangle::Inflate(float width, float height) {
        left -= width;
        right += width;
        top -= height;
        bottom += height;
    }

    void Rectangle::Inflate(const Size& size) {
        left -= size.width;
        right += size.width;
        top -= size.height;
        bottom += size.height;
    }


    // Operators
    bool Rectangle::operator== (const Rectangle& other) const {
        return ((this->left == other.left) && (this->right == other.right) && (this->top == other.top) && (this->bottom == other.bottom));
    }
    bool Rectangle::operator!= (const Rectangle& other) const {
        return ((this->left != other.left) || (this->right != other.right) || (this->top != other.top) || (this->bottom != other.bottom));
    }

    Rectangle Rectangle::operator+ (const Vector& offset) const {
        return{ left + offset.x, top + offset.y, right + offset.x, bottom + offset.y };
    }

    Rectangle Rectangle::operator- (const Vector& offset) const {
        return{ left - offset.x, top - offset.y, right - offset.x, bottom - offset.y };
    }

    Rectangle& Rectangle::operator+= (const Vector& offset) {
        left += offset.x;
        right += offset.x;
        top += offset.y;
        bottom += offset.y;

        return *this;
    }

    Rectangle& Rectangle::operator-= (const Vector& offset) {
        left -= offset.x;
        right -= offset.x;
        top -= offset.y;
        bottom -= offset.y;

        return *this;
    }

    Rectangle Rectangle::operator+ (const Size& size) const {
        return{ left, top, right + size.width, bottom + size.height };
    }

    Rectangle Rectangle::operator- (const Size& size) const {
        return{ left, top, right - size.width, bottom - size.height };
    }

    Rectangle& Rectangle::operator+= (const Size& size) {
        right += size.width;
        bottom += size.height;

        return *this;
    }

    Rectangle& Rectangle::operator-= (const Size& size) {
        right -= size.width;
        bottom -= size.height;

        return *this;
    }
    //*****************************************************************//


    //*****************************************************************//
    // POINT METHODS

    // Default constructor (0, 0)
    Point::Point()
        : x(0.0f), y(0.0f) {
    }

    // Overloaded constructor
    Point::Point(float X, float Y)
        : x(X), y(Y) {
    }

    // Helpers
    bool Point::IsPointInRectangle(const Rectangle& rectangle) const {
        return ((x >= rectangle.left && x < rectangle.right) && (y >= rectangle.top && y < rectangle.bottom));
    }

    Vector Point::ComputeDifference(const Point& other) const {
        return{ other.x - this->x, other.y - this->y };
    }

    void Point::Offset(float deltaX, float deltaY) {
        x += deltaX;
        y += deltaY;
    }

    void Point::Offset(const Vector& offset) {
        x += offset.x;
        y += offset.y;
    }


    // Operators
    bool Point::operator== (const Point& other) const {
        return ((this->x == other.x) && (this->y == other.y));
    }

    bool Point::operator!= (const Point& other) const {
        return ((this->x != other.x) || (this->y != other.y));
    }

    Point Point::operator+ (const Vector& offset) const {
        return{ this->x + offset.x, this->y + offset.y };
    }

    Point Point::operator- (const Vector& offset) const {
        return{ this->x - offset.x, this->y - offset.y };
    }

    Point& Point::operator+= (const Vector& offset) {
        x += offset.x;
        y += offset.y;

        return *this;
    }
    Point& Point::operator-= (const Vector& offset) {
        x -= offset.x;
        y -= offset.y;

        return *this;
    }

    Point Point::operator- () const {
        return{ -x, -y };
    }

    Vector Point::operator- (const Point& other) const {
        return{ this->x - other.x, this->y - other.y };
    }
    //*****************************************************************//

    //*****************************************************************//
    // SIZE METHODS

    // Default constructor (0, 0)
    Size::Size()
        : width(0.0f), height(0.0f) {
    }

    // Overloaded constructor
    Size::Size(float W, float H)
        : width(W), height(H) {
    }

    // Helpers
    float Size::ComputeArea()	const {
        return width * height;
    }


    // Type-Casts
    Size::operator Vector () const {
        return{ width, height };
    }


    // Operators
    bool Size::operator== (const Size& other) const {
        return ((this->width == other.width) && (this->height == other.height));
    }

    bool Size::operator!= (const Size& other) const {
        return ((this->width != other.width) || (this->height != other.height));
    }

    Size Size::operator+ (const Size& offset) const {
        return{ this->width + offset.width, this->height + offset.height };
    }

    Size Size::operator- (const Size& offset) const {
        return{ this->width - offset.width, this->height - offset.height };
    }

    Size& Size::operator+= (const Size& offset) {
        width += offset.width;
        height += offset.height;

        return *this;
    }

    Size& Size::operator-= (const Size& offset) {
        width -= offset.width;
        height -= offset.height;

        return *this;
    }

    Size Size::operator* (float scale) const {
        return{ width * scale, height * scale };
    }

    Size Size::operator/ (float scale) const {
        return{ width / scale, height / scale };
    }

    Size& Size::operator*= (float scale) {
        width *= scale;
        height *= scale;

        return *this;
    }

    Size& Size::operator/= (float scale) {
        width /= scale;
        height /= scale;

        return *this;
    }

    Size Size::operator- () const {
        return{ -width, -height };
    }
    //*****************************************************************//


    //*****************************************************************//
    // VECTOR METHODS

    // Default constructor (0, 0)
    Vector::Vector()
        : x(0.0f), y(0.0f) {
    }

    // Overloaded constructor
    Vector::Vector(float X, float Y)
        : x(X), y(Y) {
    }

    // Helpers
    float Vector::ComputeLength() const {
        return sqrtf((x * x) + (y * y));
    }

    void Vector::Normalize() {
        float len = sqrtf((x * x) + (y * y));

        x /= len;
        y /= len;
    }

    Vector Vector::ComputeNormalized() const {
        float len = sqrtf((x * x) + (y * y));

        return{ x / len, y / len };
    }

    void Vector::Rotate(float radians) {
        float sin = sinf(radians);
        float cos = cosf(radians);

        float newX = (cos * x) - (sin * y);
        y = (sin * x) + (cos * y);
        x = newX;
    }

    Vector Vector::ComputeRotated(float radians) const {
        float sin = sinf(radians);
        float cos = cosf(radians);

        return{ (cos * x) - (sin * y), (sin * x) + (cos * y) };
    }

    float Vector::ComputeDotProduct(const Vector& other) const {
        return ((this->x * other.x) + (this->y * other.y));
    }

    float Vector::ComputeAngle(const Vector& other) const {
        float lenSq = (((this->x * this->x) + (this->y * this->y)) * ((other.x * other.x) + (other.y * other.y)));
        if ( lenSq == 0.0f )
            return 0.0f;

        float sqr = sqrtf(lenSq);
        if ( sqr == 0.0f )
            return 0.0f;

        float dot = ((this->x * other.x) + (this->y * other.y));

        float angle = acosf(dot / sqrtf(lenSq));
        if ( _isnan(angle) != 0 )
            return 0.0f;

        return angle;
    }

    float Vector::ComputeSteering(const Vector& other) const {
        return ((this->x * other.y) - (this->y * other.x));
    }


    // Operators
    bool Vector::operator== (const Vector& other) const {
        return ((this->x == other.x) && (this->y == other.y));
    }

    bool Vector::operator!= (const Vector& other) const {
        return ((this->x != other.x) || (this->y != other.y));
    }

    Vector Vector::operator+ (const Vector& offset) const {
        return{ this->x + offset.x, this->y + offset.y };
    }

    Vector Vector::operator- (const Vector& offset) const {
        return{ this->x - offset.x, this->y - offset.y };
    }

    Vector& Vector::operator+= (const Vector& offset) {
        x += offset.x;
        y += offset.y;

        return *this;
    }

    Vector& Vector::operator-= (const Vector& offset) {
        x -= offset.x;
        y -= offset.y;

        return *this;
    }

    Vector Vector::operator* (float scale) const {
        return{ x * scale, y * scale };
    }

    Vector Vector::operator/ (float scale) const {
        return{ x / scale, y / scale };
    }

    Vector& Vector::operator*= (float scale) {
        x *= scale;
        y *= scale;

        return *this;
    }

    Vector& Vector::operator/= (float scale) {
        x /= scale;
        y /= scale;

        return *this;
    }

    Vector Vector::operator- () const {
        return{ -x, -y };
    }
    //*****************************************************************//
}
