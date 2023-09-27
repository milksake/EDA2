#include "classes.h"
#include <iostream>
#include <cassert>
#include <vector>

Octree::Octree(const Point<double> &bottomL, double height) :
    points(nullptr), bottomLeft(bottomL), height(height), nPoints(0) {}

inline int getPos(const Point<int>& p, const Point<double>& bl, double h)
{
    return (p.x > (bl.x + h / 2)) | ((p.y > (bl.y + h / 2)) << 1) | ((p.z > (bl.z + h / 2)) << 2);
}

inline Point<double> getBottomL(const Point<double>& bl, double h, int pos)
{
    return Point<double>(
        bl.x + h / 2 * ((bool)(pos & 1)),
        bl.y + h / 2 * ((bool)(pos & 2)),
        bl.z + h / 2 * ((bool)(pos & 4))
    );
}

bool Octree::exist(const Point<int> &p) const
{
    const Octree* treeP = this;
    int pos = getPos(p, bottomLeft, height);
    while (treeP->children[pos])
    {
        treeP = treeP->children[pos];
        pos = getPos(p, treeP->bottomLeft, treeP->height);
    }
    return (treeP->points && *(treeP->points) == p);
}

bool Octree::insert(const Point<int> &p)
{
    if (nPoints == 0)
    {
        points = new Point<int>(p);
        nPoints++;
        return true;
    }
    if (nPoints == 1)
    {
        if ((*points) == p)
            return false;

        int pos1 = getPos(*points, bottomLeft, height);
        int pos2 = getPos(p, bottomLeft, height);

        Octree* treeP = this;
        while (pos1 == pos2)
        {
            Point<double> newBL = getBottomL(treeP->bottomLeft, treeP->height, pos1);
            treeP->children[pos1] = new Octree(newBL, treeP->height / 2);
            treeP->children[pos1]->nPoints = 2;
            treeP = treeP->children[pos1];

            pos1 = getPos(*points, treeP->bottomLeft, treeP->height);
            pos2 = getPos(p, treeP->bottomLeft, treeP->height);
        }

        Point<double> newBL1 = getBottomL(treeP->bottomLeft, treeP->height, pos1);
        treeP->children[pos1] = new Octree(newBL1, treeP->height / 2);
        treeP->children[pos1]->points = new Point<int>(*points);
        treeP->children[pos1]->nPoints++;

        delete points;
        points = nullptr;

        Point<double> newBL2 = getBottomL(treeP->bottomLeft, treeP->height, pos2);
        treeP->children[pos2] = new Octree(newBL2, treeP->height / 2);
        treeP->children[pos2]->points = new Point<int>(p);
        treeP->children[pos2]->nPoints++;

        nPoints++;
        return true;
    }

    Octree* treeP = this;
    std::vector<int*> updateVals;
    int pos = getPos(p, bottomLeft, height);
    while (treeP->children[pos] && treeP->children[pos]->nPoints > 1)
    {
        treeP = treeP->children[pos];
        updateVals.push_back(&treeP->nPoints);
        pos = getPos(p, treeP->bottomLeft, treeP->height);
    }
    if (!treeP->children[pos])
    {
        Point<double> newBL = getBottomL(treeP->bottomLeft, treeP->height, pos);
        treeP->children[pos] = new Octree(newBL, treeP->height / 2);
    }
    if (!treeP->children[pos]->insert(p))
        return false;
    for (auto p : updateVals)
    {
        (*p)++;
    }
    nPoints++;
    return true;
}

Point<int> Octree::find_closest(const Point<int> &p) const
{
    if (nPoints == 0) return Point<int>();

    const Octree* treeP = this;
    int pos = getPos(p, bottomLeft, height);
    while (treeP->children[pos])
    {
        treeP = treeP->children[pos];
        pos = getPos(p, treeP->bottomLeft, treeP->height);
    }

    if (treeP->points)
        return *(treeP->points);

    int minDist = 0xFFFFFFF;
    Point<int> pMinDist;
    treeP->processLeaves([&minDist, &pMinDist](const Point<int>& point, const Point<double>&, double) {
        int dist = point.x * point.x + point.y * point.y + point.z * point.z;
        if (dist < minDist)
        {
            minDist = dist;
            pMinDist = point;
        }
    });

    return pMinDist;
}

void Octree::processLeaves(const std::function<void(const Point<int>&, const Point<double>&, double)>& f) const
{
    if (points)
    {
        f(*(this->points), this->bottomLeft, this->height);
        return;
    }
    for (int i = 0; i < 8; i++)
    {
        if (children[i])
        {
            children[i]->processLeaves(f);
        }
    }
}

template<typename T>
bool Point<T>::operator==(const Point<T> &rhs)
{
    return x == rhs.x && y == rhs.y && z == rhs.z;
}
