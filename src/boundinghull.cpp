#include "boundinghull.h"

#include<iostream>

int BoundingHull::orientation(QPointF a, QPointF b, QPointF c)
{
    double area = (b.y()-a.y())*(c.x()-b.x()) - (b.x()-a.x())*(c.y()-b.y());
    return (0<area) - (area<0);
}


BoundingHull::BoundingHull(QVector<QPointF> list) : pointList(list)
{
}

QVector<QLineF> BoundingHull::calculateConvexHull(){

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
        for (int k = 0; k<pointList.size(); k++){
            if (k==currentIndex || k==possibleIndex) continue;
            if (orientation(pointList[currentIndex], pointList[k], pointList[possibleIndex]) < 0)
                possibleIndex = k;
            else if (orientation(pointList[currentIndex], pointList[k], pointList[possibleIndex]) == 0){
                QLineF tempLine1(pointList[currentIndex], pointList[k]);
                QLineF tempLine2(pointList[currentIndex], pointList[possibleIndex]);
                if(tempLine1.length() < tempLine2.length())
                    possibleIndex = k;
            }
        }
        convexHull.append(QLineF(pointList[currentIndex],pointList[possibleIndex]));
        currentIndex=possibleIndex;
    } while (currentIndex != leftIndex);

    qDebug()<< "Convex hull:";
    foreach (QLineF line, convexHull) {
        qDebug()<<line;
    }

    return convexHull;
}

bool lineCompare(QLineF a, QLineF b){return a.length()<b.length();}
QVector<QLineF> BoundingHull::calculateConcaveHull(double treshold)
{
    QVector<QPointF> boundaryPoints;
    foreach (QLineF line, convexHull) boundaryPoints.append(line.p1());
    QVector<QLineF> tempList = convexHull;

    if(boundaryPoints.size()<2){
        qDebug()<<boundaryPoints;
        concaveHull=convexHull;
        return concaveHull;
    }

    while(!tempList.isEmpty()){
        qSort(tempList.begin(),tempList.end(),lessThan);
        QLineF tempLine = tempList.last();
        tempList.removeLast();

        qDebug() << "current:" << tempLine << tempLine.length();

        if(tempLine.length()<treshold){
            qDebug()<< "treshold:" << tempLine.length() ;
            qDebug()<< "SOLUTIONS:" <<tempLine;
            concaveHull.append(tempLine);
            continue;
        }

        double bestAngle = 360;
        double bestIndex = -1;
        for (int k = 0; k<pointList.size(); k++){
            if(boundaryPoints.indexOf(pointList[k])) continue;
            double angle1 = tempLine.angleTo(QLineF(tempLine.p1(),pointList[k]));
            double angle2 = tempLine.angleTo(QLineF(pointList[k],tempLine.p2()));
            angle1 = angle1 < 180 ? angle1 : 360 - angle1;
            angle2 = angle2 < 180 ? angle2 : 360 - angle2;
            angle1 = angle1 > angle2 ? angle1 : angle2;
            if(angle1 < bestAngle) {
                bestAngle = angle1;
                bestIndex = k;
            }
        }

        if(bestIndex == -1){
            concaveHull.append(tempLine);
            qDebug()<< "Best index = -1";
            qDebug()<< "SOLUTIONS:" <<tempLine;
            continue;
        }

        if(bestAngle >= 90){
            concaveHull.append(tempLine);
            qDebug()<< "bestangle:" << bestAngle << "point:" << pointList[bestIndex] ;
            qDebug()<< "SOLUTIONS:" <<tempLine;
            continue;
        }

        QLineF tempLine1(tempLine.p1(),pointList[bestIndex]);
        QLineF tempLine2(pointList[bestIndex],tempLine.p2());
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
            qDebug()<< "intersection:" << intersection ;
            qDebug()<< "SOLUTIONS:" <<tempLine;
            continue;
        }

        qDebug()<< "new lines:";
        qDebug()<<tempLine1;
        qDebug()<<tempLine2;

        tempList.append(tempLine1);
        tempList.append(tempLine2);
        boundaryPoints.append(pointList[bestIndex]);
    }

    for(int k=1; k<concaveHull.size();k++){
        for(int i=k;i<concaveHull.size();i++){
            if(concaveHull[k-1].p2()==concaveHull[i].p1()){
                QLineF temp = concaveHull[k];
                concaveHull.replace(k,concaveHull[i]);
                concaveHull.replace(i,temp);
                break;
            }
        }
    }
    return concaveHull;
}

