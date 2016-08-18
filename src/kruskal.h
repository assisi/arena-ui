#ifndef KRUSKAL_H
#define KRUSKAL_H

#include<QVector>
#include<QList>
#include<QLineF>
#include<QtAlgorithms>

class kruskal
{
private:
    QList<QLineF> allLines;
public:
    kruskal(QVector<QPointF> list);

};

#endif // KRUSKAL_H
