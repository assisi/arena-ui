#ifndef BOUNDINGHULL_H
#define BOUNDINGHULL_H

#include<QList>
#include<QPointF>
#include<QLineF>
#include<QtAlgorithms>

#define PI 3.14159265

class BoundingHull
{
private:
    QList<QPointF> pointList;
    QList<QLineF> convexHull;
    QList<QLineF> concaveHull;


    int orientation(QPointF a, QPointF b, QPointF c);

public:
    BoundingHull(QList<QPointF> list);
    QList<QLineF> calculateConvexHull();
    QList<QLineF> calculateConcaveHull(double treshold);
};

#endif // BOUNDINGHULL_H
