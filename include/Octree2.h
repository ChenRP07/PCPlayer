#pragma once
#include <vector>
#include <iostream>
class Octree2 {
public:
    Octree2() {}
    ~Octree2() { std::cout << "delete"; destory(root); }
    struct Point
    {
        float x, y, z;
        float r, g, b;
        void setPoint(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        Point() {}
        Point(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        Point& operator = (const Point &b) {
            this->x = b.x;
            this->y = b.y;
            this->z = b.z;
            return *this;
        }
    };
    typedef struct Octree_Node {
        int count;
        int newCount;
        std::vector<Point> points;
        std::vector<Point> newPoints;
        Octree_Node *nodes[8];
        Point centel;
        Point color;
        float length;
        int level;
        Octree_Node()
        {
            for (int i = 0; i < 8; i++) {
                nodes[i] = NULL;
            }
            color.r = rand() % 100 / 100.0;
            color.g = rand() % 100 / 100.0;
            color.b = rand() % 100 / 100.0;
        }
    }Octree_Node, *Octree_Struct;

    Octree_Struct root;
    std::vector<Point> pointCloud;
    std::vector<Point> resPointCloud;
    float octLength;
    bool oldPoint = true;

    Octree2& operator+(const Octree2 &b) {
        Octree2 *newOctree = new Octree2();
        newOctree->root = NULL;
        resPointCloud.clear();
        newOctree->octLength = this->octLength;
        for (auto it = this->pointCloud.begin(); it != this->pointCloud.end(); it++) {
            newOctree->insertPoint(newOctree->root, (*it));
            resPointCloud.push_back((*it));
        }
        newOctree->oldPoint = false;
        for (auto it = b.pointCloud.begin(); it != b.pointCloud.end(); it++) {
            newOctree->insertPoint(newOctree->root, (*it));
            resPointCloud.push_back((*it));
        }
        return *newOctree;
    }


    Octree2& operator-(const Octree2 &b) {
        Octree2 *newOctree = new Octree2();
        newOctree->root = NULL;
        resPointCloud.clear();
        newOctree->octLength = this->octLength;
        for (auto it = this->pointCloud.begin(); it != this->pointCloud.end(); it++) {
            newOctree->insertPoint(newOctree->root, (*it));
        }
        newOctree->oldPoint = false;
        for (auto it = b.pointCloud.begin(); it != b.pointCloud.end(); it++) {
            newOctree->insertPoint(newOctree->root, (*it));
        }
        getSubResault(*newOctree, newOctree->root);
        return *newOctree;
    }

    int subLimt = 20;
    void getSubResault(Octree2 &root, Octree2::Octree_Struct node) {
        if (node->level > 1) {
            for (int i = 0; i < 8; i++) {
                if (node->nodes[i] != NULL) {
                    getSubResault(root, node->nodes[i]);
                }
            }
        }
        else {
            if (node->newCount + subLimt <= node->count)
            {
                int size = node->points.size();
                for (int i = 0; i < size; i++) {
                    root.resPointCloud.push_back(node->points.at(i));
                }
            }
        }
    }

    void insertPoint(Octree_Struct &root, Point point) {
        //第一个点
        if (root == NULL) {
            root = new Octree_Node();
            root->centel = point;
            root->length = octLength;
            root->level = 1;
            root->newCount = 0;
            root->count = 0;

            if (oldPoint == true) {
                root->count = 1;
                root->points.push_back(point);
            }
            else {
                root->newCount = 1;
                root->newPoints.push_back(point);
            }

            pointCloud.push_back(point);
            return;
        }

        //如果point在root范围内
        bool inRoot = false;
        if (point.x <= root->centel.x + root->length && point.x >= root->centel.x - root->length) {
            if (point.y <= root->centel.y + root->length && point.y >= root->centel.y - root->length) {
                if (point.z <= root->centel.z + root->length && point.z >= root->centel.z - root->length) {
                    inRoot = true;
                }
            }
        }

        //在里面
        if (inRoot == true) {
            //最下面那层
            if (root->level == 1) {

                if (oldPoint == true) {
                    root->count++;
                    root->points.push_back(point);
                }
                else {
                    root->newCount++;
                    root->newPoints.push_back(point);
                }
                pointCloud.push_back(point);
            }
            //中间层
            else {
                int i = 0;
                Point centel;
                //不用=
                if (point.x >= root->centel.x && point.y >= root->centel.y &&point.z >= root->centel.z) {
                    i = 0;
                    centel.x = root->centel.x + root->length / 2;
                    centel.y = root->centel.y + root->length / 2;
                    centel.z = root->centel.z + root->length / 2;
                }
                else if (point.x <= root->centel.x && point.y >= root->centel.y &&point.z >= root->centel.z) {
                    i = 1;
                    centel.x = root->centel.x - root->length / 2;
                    centel.y = root->centel.y + root->length / 2;
                    centel.z = root->centel.z + root->length / 2;
                }
                else if (point.x >= root->centel.x && point.y <= root->centel.y &&point.z >= root->centel.z) {
                    i = 2;
                    centel.x = root->centel.x + root->length / 2;
                    centel.y = root->centel.y - root->length / 2;
                    centel.z = root->centel.z + root->length / 2;
                }
                else if (point.x <= root->centel.x && point.y <= root->centel.y &&point.z >= root->centel.z) {
                    i = 3;
                    centel.x = root->centel.x - root->length / 2;
                    centel.y = root->centel.y - root->length / 2;
                    centel.z = root->centel.z + root->length / 2;
                }
                else if (point.x >= root->centel.x && point.y >= root->centel.y &&point.z <= root->centel.z) {
                    i = 4;
                    centel.x = root->centel.x + root->length / 2;
                    centel.y = root->centel.y + root->length / 2;
                    centel.z = root->centel.z - root->length / 2;
                }
                else if (point.x <= root->centel.x && point.y >= root->centel.y &&point.z <= root->centel.z) {
                    i = 5;
                    centel.x = root->centel.x - root->length / 2;
                    centel.y = root->centel.y + root->length / 2;
                    centel.z = root->centel.z - root->length / 2;
                }
                else if (point.x >= root->centel.x && point.y <= root->centel.y &&point.z <= root->centel.z) {
                    i = 6;
                    centel.x = root->centel.x + root->length / 2;
                    centel.y = root->centel.y - root->length / 2;
                    centel.z = root->centel.z - root->length / 2;
                }
                else if (point.x <= root->centel.x && point.y <= root->centel.y &&point.z <= root->centel.z) {
                    i = 7;
                    centel.x = root->centel.x - root->length / 2;
                    centel.y = root->centel.y - root->length / 2;
                    centel.z = root->centel.z - root->length / 2;
                }

                if (root->nodes[i] == NULL) {
                    root->nodes[i] = new Octree_Node();
                    root->nodes[i]->centel = centel;
                    root->nodes[i]->length = root->length / 2;
                    root->nodes[i]->level = root->level - 1;
                    root->nodes[i]->count = 0;
                    root->nodes[i]->newCount = 0;
                }
                else {
                    //root->nodes[i]->count++;
                }
                if (oldPoint == true) {
                    root->count++;
                }
                else {
                    root->newCount++;
                }

                insertPoint(root->nodes[i], point);

            }
        }
        else if (inRoot == false) {
            //      newRoot
            //     /               \
            //  root            insert
            Octree_Struct newRoot = new Octree_Node();

            newRoot->count = root->count;
            newRoot->newCount = root->newCount;

            Point centel;
            if (point.x >= root->centel.x && point.y >= root->centel.y &&point.z >= root->centel.z) {
                centel = Point(root->centel.x + root->length, root->centel.y + root->length, root->centel.z + root->length);
            }
            else if (point.x <= root->centel.x && point.y >= root->centel.y &&point.z >= root->centel.z) {
                centel = Point(root->centel.x - root->length, root->centel.y + root->length, root->centel.z + root->length);
            }
            else if (point.x >= root->centel.x && point.y <= root->centel.y &&point.z >= root->centel.z) {
                centel = Point(root->centel.x + root->length, root->centel.y - root->length, root->centel.z + root->length);
            }
            else if (point.x <= root->centel.x && point.y <= root->centel.y &&point.z >= root->centel.z) {
                centel = Point(root->centel.x - root->length, root->centel.y - root->length, root->centel.z + root->length);
            }
            else if (point.x >= root->centel.x && point.y >= root->centel.y &&point.z <= root->centel.z) {
                centel = Point(root->centel.x + root->length, root->centel.y + root->length, root->centel.z - root->length);
            }
            else if (point.x <= root->centel.x && point.y >= root->centel.y &&point.z <= root->centel.z) {
                centel = Point(root->centel.x - root->length, root->centel.y + root->length, root->centel.z - root->length);
            }
            else if (point.x >= root->centel.x && point.y <= root->centel.y &&point.z <= root->centel.z) {
                centel = Point(root->centel.x + root->length, root->centel.y - root->length, root->centel.z - root->length);
            }
            else if (point.x <= root->centel.x && point.y <= root->centel.y &&point.z <= root->centel.z) {
                centel = Point(root->centel.x - root->length, root->centel.y - root->length, root->centel.z - root->length);
            }

            newRoot->centel = centel;

            float length = root->length;
            newRoot->level = root->level;
            while (point.x > (centel.x + length) || point.x < (centel.x - length) ||
                point.y>(centel.y + length) || point.y < (centel.y - length) ||
                point.z>(centel.z + length) || point.z < (centel.z - length)
                ) {
                length *= 2;
                newRoot->level++;
            }

            newRoot->length = length;

            int i = 0;
            if (root->centel.x >= newRoot->centel.x && root->centel.y >= newRoot->centel.y &&root->centel.z >= newRoot->centel.z) {
                i = 0;
            }
            else if (root->centel.x <= newRoot->centel.x && root->centel.y >= newRoot->centel.y &&root->centel.z >= newRoot->centel.z) {
                i = 1;
            }
            else if (root->centel.x >= newRoot->centel.x && root->centel.y <= newRoot->centel.y &&root->centel.z >= newRoot->centel.z) {
                i = 2;
            }
            else if (root->centel.x <= newRoot->centel.x && root->centel.y <= newRoot->centel.y &&root->centel.z >= newRoot->centel.z) {
                i = 3;
            }
            else if (root->centel.x >= newRoot->centel.x && root->centel.y >= newRoot->centel.y &&root->centel.z <= newRoot->centel.z) {
                i = 4;
            }
            else if (root->centel.x <= newRoot->centel.x && root->centel.y >= newRoot->centel.y &&root->centel.z <= newRoot->centel.z) {
                i = 5;
            }
            else if (root->centel.x >= newRoot->centel.x && root->centel.y <= newRoot->centel.y &&root->centel.z <= newRoot->centel.z) {
                i = 6;
            }
            else if (root->centel.x <= newRoot->centel.x && root->centel.y <= newRoot->centel.y &&root->centel.z <= newRoot->centel.z) {
                i = 7;
            }
            newRoot->nodes[i] = root;
            root = newRoot;
            insertPoint(newRoot, point);
        }
    }

    void destory() {
        std::cout << "deleted";
        destory(root);
    }
    void destory(Octree_Struct &root) {
       /* if (root != NULL)
            for (int i = 0; i < 8; i++) {
                if (root->nodes[i] != NULL) {
                    destory(root->nodes[i]);
                }
            }
        if (root != NULL)
        {
            free(root);
            root = NULL;
        }*/
    }
};
