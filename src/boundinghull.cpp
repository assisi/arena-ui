#include "boundinghull.h"

int BoundingHull::orientation(QPointF a, QPointF b, QPointF c)
{
    double area = (b.y()-a.y())*(c.x()-b.x()) - (b.x()-a.x())*(c.y()-b.y());
    return (0<area) - (area<0);
}


BoundingHull::BoundingHull(QList<QPointF> list) : pointList(list)
{
}

QList<QLineF> BoundingHull::calculateConvexHull(){

    if (pointList.size()<2) return convexHull;
    if (pointList.size()==2){
        convexHull.append(QLineF(pointList[0],pointList[1]));
        return convexHull;
    }
    int leftIndex=0;
    for(int k = 1; k<pointList.size() ;k++)
        if(pointList[k].x()<pointList[leftIndex].x())
            leftIndex = k;

    int currentIndex = leftIndex;
    do {
        int possibleIndex = (currentIndex+1) % pointList.size();
        for (int k = 0; k<pointList.size(); k++)
            if (orientation(pointList[currentIndex], pointList[k], pointList[possibleIndex]) < 0)
                        possibleIndex = k;
        convexHull.append(QLineF(pointList[currentIndex],pointList[possibleIndex]));
        currentIndex=possibleIndex;
    } while (currentIndex != leftIndex);

    return convexHull;
}

bool lessThan(QLineF a, QLineF b){
    return a.length()<b.length();
}

QList<QLineF> BoundingHull::calculateConcaveHull(double treshold)
{
    QList<QPointF> boundaryPoints;
    foreach (QLineF line, convexHull) {
        boundaryPoints.append(line.p1());
    }
    QList<QLineF> tempList = convexHull;
    qSort(tempList.begin(),tempList.end(),lessThan);
    while(!tempList.isEmpty()){
        qSort(tempList.begin(),tempList.end(),lessThan);
        QLineF tempLine = tempList[0];
        tempList.removeFirst();

        if(tempList.length()<treshold){
            concaveHull.append(tempLine);
            continue;
        }

        double bestAngle = 360;
        double bestIndex = -1;
        for (int k = 0; k<pointList.size(); k++){
            double angle1 = tempLine.angleTo(QLineF(tempLine.p1(),pointList[bestIndex]));
            double angle2 = tempLine.angleTo(QLineF(tempLine.p2(),pointList[bestIndex]));
            angle1 = angle1 < 180 ? angle1 : 360 - angle1;
            angle2 = angle2 < 180 ? angle2 : 360 - angle2;
            angle1 = angle1 > angle2 ? angle1 : angle2;
            if(angle1 < bestAngle) {
                bestAngle = angle1;
                bestIndex = k;
            }
        }

        if(bestAngle > 90 || boundaryPoints.indexOf(pointList[bestIndex]) > -1){
            concaveHull.append(tempLine);
            continue;
        }

        QLineF tempLine1(tempLine.p1(),pointList[bestIndex]);
        QLineF tempLine2(tempLine.p1(),pointList[bestIndex]);
        bool goodEdges = true;
        QPointF intersection;
        foreach (QLineF line, tempList) {
            if(line.intersect(tempLine1, &intersection) == 1)
                if(boundaryPoints.indexOf(intersection) == -1) goodEdges = false;
            if(line.intersect(tempLine2, &intersection) == 1)
                if(boundaryPoints.indexOf(intersection) == -1) goodEdges = false;
            if(!goodEdges) break;
        }
        foreach (QLineF line, concaveHull) {
            if(line.intersect(tempLine1, &intersection) == 1)
                if(boundaryPoints.indexOf(intersection) == -1) goodEdges = false;
            if(line.intersect(tempLine2, &intersection) == 1)
                if(boundaryPoints.indexOf(intersection) == -1) goodEdges = false;
            if(!goodEdges) break;
        }
        if(!goodEdges){
            concaveHull.append(tempLine);
            continue;
        }

        concaveHull.append(tempLine1);
        concaveHull.append(tempLine1);
        boundaryPoints.append(pointList[bestIndex]);
    }

    return concaveHull;
}

