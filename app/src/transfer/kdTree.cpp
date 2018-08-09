//---------------------------------------------------------------------------
#include "kdTree.h"
//---------------------------------------------------------------------------
using namespace kt;
const qreal Tuple::DistsqUndefined = std::numeric_limits< qreal >::infinity();
//---------------------------------------------------------------------------
Node* Tree::_insert(Node* node, const QPointF &pos, Data* data)
{
    qreal posVal, nodePosVal;
    if (node->dir == 0) {
        posVal     = pos.x();
        nodePosVal = node->pos.x();
    } else {
        posVal     = pos.y();
        nodePosVal = node->pos.y();
    }

    if (posVal < nodePosVal) {
        if (node->left == NULL) {
            node->addleft(new Node(pos, data));
            return node->left;
        }
        else {
            node->rect->extend(pos);
            return this->_insert(node->left, pos, data);
        }
    }
    else {
        if (node->right == NULL) {
            node->addright(new Node(pos, data));
            return node->right;
        }
        else {
            node->rect->extend(pos);
            return this->_insert(node->right, pos, data);
        }
    }
}
//---------------------------------------------------------------------------
Node* Tree::insert(const QPointF &pos, Data* data)
{
    if (this->root == NULL) {
        this->root  = new Node(pos, data);
        this->level = this->root->level;
        return this->root;
    }
    else {
        Node* node = this->_insert(this->root, pos, data);
        if(node->level > this->level) {
            this->level = node->level;
        }
        return node;
    }
}
//---------------------------------------------------------------------------
Tuple Tree::_nearest(Node* node, const QPointF &pos, bool checkempty, int level)
{
    this->count += 1;

    int dir = node->dir;
    qreal d = (dir == 0) ? (pos.x() - node->pos.x()) : (pos.y() - node->pos.y());

    Node* result   = node;
    qreal distsq   = Tuple::DistsqUndefined;
    Node* neartree = NULL;
    Node* fartree  = NULL;

    if (checkempty && node->data == NULL) {
        result = NULL;
    } else {
        distsq = node->distance_squared(pos);
    }

    if (d <= 0) {
        neartree = node->left;
        fartree  = node->right;
    } else {
        neartree = node->right;
        fartree  = node->left;
    }

    if (neartree != NULL) {
        Tuple n_ret = this->_nearest(neartree, pos, checkempty, level+1);
        if (result == NULL || (n_ret.distsq != Tuple::DistsqUndefined && n_ret.distsq < distsq)) {
            result = n_ret.node;
            distsq = n_ret.distsq;
        }
    }
    if (fartree != NULL) {
        if (result == NULL || (fartree->rect->distance_squared(pos) < distsq)) {
            Tuple f_ret = this->_nearest(fartree, pos, checkempty, level+1);
            if (result == NULL || (f_ret.distsq != Tuple::DistsqUndefined && f_ret.distsq < distsq)) {
                result = f_ret.node;
                distsq = f_ret.distsq;
            }
        }
    }
    return Tuple(result, distsq);
}
//---------------------------------------------------------------------------
Tuple Tree::nearest(const QPointF &pos, bool checkempty)
{
    this->nnearest += 1;
    if (this->root == NULL) {
        return Tuple(NULL, Tuple::DistsqUndefined);
    }
    this->root->count = 0;
    Tuple ret = this->_nearest(this->root, pos, checkempty, 0);
    this->count += this->root->count;
    return ret;
}
//---------------------------------------------------------------------------
