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

#ifndef __GEOMETRY_HEADER_H__
#define __GEOMETRY_HEADER_H__

#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif
#include "stdafx.h"

namespace G2D {
    class Rectangle;
    class Point;
    class Size;
    class Vector;

    extern const float PI;

    //*****************************************************************//
    // Rectangle
    // - axis-aligned rectangle in 2D space with left, top, right, and bottom sides
    // - the right and bottom are exclusive (collision and rendering are < those sides)
    class RENDERER_API Rectangle {
    public:
        float left;     // left x position (inclusive)
        float top;      // top y position (inclusive)
        float right;    // right x position (exclusive)
        float bottom;   // bottom y position (exclusive)    

        Rectangle();                                                // Default constructor (0,0) -> (0,0)
        Rectangle(float L, float T, float R, float B);              // Overloaded constructor
        Rectangle(const Point& topleft, const Point& bottomright);  // Overloaded Point-Point constructor
        Rectangle(const Point& topleft, const Size& size);          // Overloaded Point-Size constructor


        Point GetTopLeft()        const;
        Point GetTopRight()       const;
        Point GetBottomLeft()     const;
        Point GetBottomRight()    const;

        void SetTopLeft(const Point& topleft);
        void SetTopRight(const Point& topright);
        void SetBottomLeft(const Point& bottomleft);
        void SetBottomRight(const Point& bottomright);


        Point ComputeCenter() const;
        Size  ComputeSize()   const;
        float ComputeWidth()  const;
        float ComputeHeight() const;

        bool  IsEmpty() const;     // empty if width < 1 || height < 1

        bool  IsPointInRectangle(const Point& point)  const;
        bool  IsIntersecting(const Rectangle& other) const; // intersecting if the rectangles overlap
        Rectangle ComputeIntersection(const Rectangle& other) const; // returns overlapping rectangle
        Rectangle ComputeUnion(const Rectangle& other) const; // returns rectangle surrounding both rectangles


        void  Normalize();       // corrects positions so topleft < bottomright

        void  MoveTo(float left, float top);   // keeps size
        void  MoveTo(const Point& topleft);

        void  Offset(float deltaX, float deltaY); // keeps size
        void  Offset(const Vector& offset);

        void  Resize(float width, float height);  // keeps top-left position
        void  Resize(const Size& size);

        void  Inflate(float width, float height);  // expands all 4 sides
        void  Inflate(const Size& size);


        bool  operator==   (const Rectangle& other) const; // comparison
        bool  operator!=   (const Rectangle& other) const;

        Rectangle operator+   (const Vector& offset) const; // keeps size
        Rectangle operator-   (const Vector& offset) const;
        Rectangle& operator+=   (const Vector& offset);
        Rectangle& operator-=   (const Vector& offset);

        Rectangle operator+   (const Size& size) const; // keeps top-left position
        Rectangle operator-   (const Size& size) const;
        Rectangle& operator+=   (const Size& size);
        Rectangle& operator-=   (const Size& size);

    }; // class Rectangle


    //*****************************************************************//
    // Point
    // - (x,y) position in 2D space
    class RENDERER_API Point {
    public:
        float x;
        float y;

        Point();              // Default constructor (0, 0)
        Point(float X, float Y);           // Overloaded constructor

        bool  IsPointInRectangle(const Rectangle& rectangle) const;

        Vector  ComputeDifference(const Point& other)  const; // vector to other point

        void  Offset(float deltaX, float deltaY);
        void  Offset(const Vector& offset);


        bool  operator==   (const Point& other)  const; // comparison
        bool  operator!=   (const Point& other)  const;

        Point  operator+   (const Vector& offset) const; // addition
        Point  operator-   (const Vector& offset) const; // subtraction
        Point&  operator+=   (const Vector& offset);
        Point&  operator-=   (const Vector& offset);

        Point  operator-   ()     const; // negation

        Vector  operator-   (const Point& other)  const; // difference between points

    }; // class Point


    //*****************************************************************//
    // Size
    // - width & height in 2D space
    class RENDERER_API Size {
    public:
        float width;
        float height;

        Size();              // Default constructor [0, 0]
        Size(float W, float H);           // Overloaded constructor

        float ComputeArea() const;


        operator Vector() const; // typecast to Vector


        bool  operator==   (const Size& other)  const; // comparison
        bool  operator!=   (const Size& other)  const;

        Size  operator+   (const Size& offset)  const; // addition
        Size  operator-   (const Size& offset)  const; // subtraction
        Size&  operator+=   (const Size& offset);
        Size&  operator-=   (const Size& offset);

        Size  operator* (float scale)    const; // scalar multiplication
        Size  operator/ (float scale)    const; // scalar division
        Size&  operator*=   (float scale);
        Size&  operator/=   (float scale);

        Size  operator-()     const; // negation

    }; // class Size

       //*****************************************************************//
       // Vector
       // - <x,y> offset in 2D space
    class RENDERER_API Vector {
    public:
        float x;
        float y;

        Vector(void);              // Default constructor <0, 0>
        Vector(float X, float Y);           // Overloaded constructor

        float  ComputeLength(void) const;

        void  Normalize(void);
        Vector  ComputeNormalized(void) const;

        void  Rotate(float radians);
        Vector  ComputeRotated(float radians) const;

        float  ComputeDotProduct(const Vector& other)  const;
        float  ComputeAngle(const Vector& other)  const;
        float  ComputeSteering(const Vector& other)  const;


        bool  operator==   (const Vector& other)  const; // comparison
        bool  operator!=   (const Vector& other)  const;

        Vector  operator+   (const Vector& offset) const; // addition
        Vector  operator-   (const Vector& offset) const; // subtraction
        Vector&  operator+=   (const Vector& offset);
        Vector&  operator-=   (const Vector& offset);

        Vector  operator*   (float scale)    const; // scalar multiplication
        Vector  operator/   (float scale)    const; // scalar division
        Vector& operator*=   (float scale);
        Vector& operator/=   (float scale);

        Vector operator-   (void)     const; // negation


    }; // class Vector
}

#endif