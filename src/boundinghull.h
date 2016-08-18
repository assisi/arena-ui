#ifndef BOUNDINGHULL_H
#define BOUNDINGHULL_H

#include<QVector>
#include<QPointF>
#include<QLineF>
#include<QtAlgorithms>
#include<QDebug>

#define PI 3.14159265

class BoundingHull
{
private:
    QVector<QPointF> pointList;
    QVector<QLineF> convexHull;
    QVector<QLineF> concaveHull;


    int orientation(QPointF a, QPointF b, QPointF c);

public:
    BoundingHull(QVector<QPointF> list);
    QVector<QLineF> calculateConvexHull();
    QVector<QLineF> calculateConcaveHull(double treshold);
};

#endif // BOUNDINGHULL_H
