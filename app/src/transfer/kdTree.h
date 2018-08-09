//---------------------------------------------------------------------------
// 最近傍探索クラス
//---------------------------------------------------------------------------
#ifndef KDTREE_H
#define KDTREE_H
//---------------------------------------------------------------------------
#include <QtCore>

namespace kt {
//---------------------------------------------------------------------------
class Data{
public:
    int  index;
    bool rev;

    Data() {
        this->index = 0;
        this->rev   = false;
    }

    Data(int index, bool rev) {
        this->index = index;
        this->rev   = rev;
    }
};
//---------------------------------------------------------------------------
class Hyperrectangle {
public:
    int     dim;
    QPointF min;
    QPointF max;

    Hyperrectangle(int dim, const QPointF &min, const QPointF &max) {
        this->dim = dim;
        this->min = min;
        this->max = max;
    }

    void extend(const QPointF &pos) {
        this->min.setX(qMin(pos.x(), this->min.x()));
        this->min.setY(qMin(pos.y(), this->min.y()));
        this->max.setX(qMax(pos.x(), this->max.x()));
        this->max.setY(qMax(pos.y(), this->max.y()));
    }

    qreal distance_squared(const QPointF &pos) {
        qreal result = 0.0;

        if (pos.x() < this->min.x()) {
            result += qPow(pos.x() - this->min.x(), 2.0);
        } else if (pos.x() > this->max.x()) {
            result += qPow(pos.x() - this->max.x(), 2.0);
        }

        if (pos.y() < this->min.y()) {
            result += qPow(pos.y() - this->min.y(), 2.0);
        } else if (pos.y() > this->max.y()) {
            result += qPow(pos.y() - this->max.y(), 2.0);
        }

        return result;
    }
};
//---------------------------------------------------------------------------
class Node {
public:
    QPointF pos;
    Data*   data;
    Node*   left;
    Node*   right;
    int     dim;
    int     dir;
    int     count;
    int     level;
    Hyperrectangle* rect;

    Node(const QPointF &pos, Data* data) {
        this->pos   = pos;
        this->data  = data;
        this->left  = NULL;
        this->right = NULL;
        this->dim   = 2;
        this->dir   = 0;
        this->count = 0;
        this->level = 0;
        this->rect  = new Hyperrectangle(this->dim, pos, pos);
    }

    ~Node() {
        if (data  != NULL) { delete data;  }
        if (left  != NULL) { delete left;  }
        if (right != NULL) { delete right; }
        if (rect  != NULL) { delete rect;  }
    }

    void addleft(Node* node) {
        this->left = node;
        this->rect->extend(node->pos);
        node->level = this->level + 1;
        node->dir   = (this->dir + 1) % this->dim;
    }

    void addright(Node* node) {
        this->right = node;
        this->rect->extend(node->pos);
        node->level = this->level + 1;
        node->dir   = (this->dir + 1) % this->dim;
    }

    qreal distance_squared(QPointF pos) {
        qreal d2 = 0.0;
        d2 += qPow(this->pos.x() - pos.x(), 2.0);
        d2 += qPow(this->pos.y() - pos.y(), 2.0);
        return d2;
    }
};
//---------------------------------------------------------------------------
class Tuple{
public:
    static const qreal DistsqUndefined;
    Node* node;
    qreal distsq = DistsqUndefined;

    Tuple(Node* node, qreal distsq) {
        this->node   = node;
        this->distsq = distsq;
    }
};
//---------------------------------------------------------------------------
class Tree
{
public:
    Node* root;
    qreal nnearest;
    int   count;
    int   level;
    int   dim;

    Tree(int dim = 2) {
        this->root     = NULL;
        this->nnearest = 0;
        this->count    = 0;
        this->level    = 0;
        this->dim      = dim;
    }

    ~Tree() {
        if (root != NULL) { delete root; }
    }

    void resetcounters() {
        this->nnearest = 0;
        this->count    = 0;
    }

    Node* _insert(Node* node, const QPointF &pos, Data* data);
    Node* insert(const QPointF &pos, Data* data);
    Tuple _nearest(Node* node, const QPointF &pos, bool checkempty, int level);
    Tuple nearest(const QPointF &pos, bool checkempty);
};
//---------------------------------------------------------------------------
}
#endif // KDTREE_H
