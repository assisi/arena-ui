#include "boundinghull.h"

int BoundingHull::orientation(QPointF a, QPointF b, QPointF c)
{
    double area = (b.y()-a.y())*(c.x()-b.x()) - (b.x()-a.x())*(c.y()-b.y());
    return 0<area - area<0;
}

BoundingHull::BoundingHull(QList<QPointF> list, double lenght) : pointList(list)
{
}

QList<QLineF> BoundingHull::convexHull(){
    QList<QLineF> result;
    if (pointList.size()<2) return result;
    if (pointList.size()==2){
        result.append(QLineF(pointList[0],pointList[1]));
        return result;
    }
    int leftIndex=0;
    for(int k = 1; k<pointList.size() ;k++)
        if(pointList[k].x()<pointList[leftIndex])
            leftIndex = k;

    int currentIndex = leftIndex;
    do {
        int possibleIndex = (currentIndex+1) % pointList.size();
        for (int k = 0; k<pointList.size(); k++)
            if (orientation(pointList[currentIndex], pointList[k], pointList[possibleIndex]) < 0)
                        possibleIndex = k;
        result.append(QLineF(pointList[currentIndex],pointList[possibleIndex]));
        possibleIndex=currentIndex;
    } while (currentIndex != leftIndex);

    return result;
}

QList<QLineF> BoundingHull::concaveHull(double treshold)
{

}

