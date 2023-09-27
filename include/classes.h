#include <string>
#include <functional>
#ifndef OCTREE_H
#define OCTREE_H

template<typename T>
struct Point{
   T x;
   T y;
   T z;

   Point() : x(0), y(0), z(0) {}
   Point(T a, T b, T c) : x(a), y(b), z(c) {}
   bool operator== (const Point<T>& rhs);
};

class Octree {
private:
   Octree *children[8] = {nullptr};
   Point<int> *points;
   
   // bottomLeft y h definen el espacio(cubo más grande)
   Point<double> bottomLeft;
   double height;

   int nPoints; // puntos ingresados.

public:
   Octree(const Point<double>& bottomL, double height);
   bool exist( const Point<int> &p ) const;
   bool insert( const Point<int> &p );
   Point<int> find_closest( const Point<int> &p ) const;
   void processLeaves( const std::function<void(const Point<int>&, const Point<double>&, double)>& f ) const;
};

#endif
