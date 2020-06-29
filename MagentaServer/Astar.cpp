#include "Astar.h"

void Astar::PathFinding(int sx, int sy, int ex, int ey)
{
    path.clear();
    openNodeList.clear();
    closeNodeList.clear();

    startNode = gameworld->nodeMap[sy][sx];
    endNode = gameworld->nodeMap[ey][ex];

    if (startNode == endNode)
        return;
    closeNodeList.push_back(startNode);

    AddOpenNodeWithNextNode(startNode);
    CalcCoast(startNode, endNode);

    Node* nextNode = GetNextNode();

    while (nextNode != endNode)
    {
        closeNodeList.push_back(nextNode);

        AddOpenNodeWithNextNode(nextNode);

        CalcCoast(nextNode, endNode);

        nextNode = GetNextNode();
    }

    while (nextNode != startNode)
    {
        path.push_front(nextNode);
        nextNode = nextNode->parentNode;
    }
}

void Astar::AddOpenNodeWithNextNode(Node* nextNode)
{
    for (int i = 0; i < 4; ++i)
    {
        Node curNodeCoord = *nextNode + *offsetCoords[i];
        Node* curNode = gameworld->nodeMap[curNodeCoord.getY()][curNodeCoord.getX()];

        if (!gameworld->is_wall(curNode->getX(), curNode->getY())
            && curNode->obstacle == false)
        {
            auto iter = std::find(closeNodeList.begin(), closeNodeList.end(), curNode);
            if (iter == closeNodeList.end())
            {
                auto it = std::find(openNodeList.begin(), openNodeList.end(), curNode);
                if (it == openNodeList.end())
                {
                    openNodeList.push_back(curNode);

                    if (curNode != startNode)
                        //   && curNode != endNode)
                    {
                        curNode->parentNode = nextNode;
                    }
                }
            }
        }
    }
}

void Astar::CalcCoast(Node* nextNode, Node* endNode)
{
    for (auto& node : openNodeList)
    {
        if (node->parentNode == nextNode)
        {
            node->ResetCost();
            int G = nextNode->fromStartCost + 10;
            int H = node->distance(*endNode) * 10;

            node->SetCoast(G, H);
        }
    }
}

Node* Astar::GetNextNode()
{
    Node* minNode = openNodeList.front();
    int size = openNodeList.size();
    for (auto it = openNodeList.begin(); it != openNodeList.end();)
    {
        if (size != openNodeList.size())
            break;

        auto iter = std::find(closeNodeList.begin(), closeNodeList.end(), *it);
        if (iter == closeNodeList.end()) {
            if (*it == nullptr) break;
            if (minNode->cost > (*it)->cost)
                minNode = *it;
            it++;
        }
        else
        {
            it = openNodeList.erase(it);
        }
    }
    return minNode;
}