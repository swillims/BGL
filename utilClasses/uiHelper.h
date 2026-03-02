#pragma once
//#include "singleton/staticDraw.h"
#include "singleton/staticWrite.h" // used for appending text to a channel

// main purpose of UIElement is to return a vector<float> list for batching
// - batching uses single texture and the intended use implies that with a reference to a batch
// it also has secondary functionality for a channel using staticWrite

// Non-contaienor elements should receive cord values after being adjustNode is called from parent node

struct UIElement
{
    /*
        nodes is implemented in UIElement instead of container to make chains work
        it is mem enefficient but it chains which is a big trade off
        parent[0][0][1] is doable
        the alternative is a series of castings which is hell
        if using something that doesn't use have subnodes just pretend nodes doesn't exist
    */
    std::vector<std::unique_ptr<UIElement>> nodes; 

    float xMin;
    float yMin;
    float xSize;
    float ySize;

    int key; // key is referenced

    UIElement(){}
    UIElement(float xMin, float yMin, float xSize, float ySize, int key=-1) 
        : xMin(xMin), yMin(yMin), xSize(xSize), ySize(ySize), key(key){}

    virtual ~UIElement() = default;
    
    virtual bool isPointIn(float x, float y)
    {
        if (x >= xMin && y >= yMin)
        {
            if (x <= xMin + xSize && y <= yMin + ySize)
            {
                return true;
            }
        }
        return false;
    }
    virtual void renderVerts(std::vector<float>& vertices)
    {}
    virtual void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2)
    {
        xMin = xMin;
        yMin = yMin;
        xSize = xSize;
        ySize = ySize;
    }

    UIElement& operator[](const int i)
    {
        return *nodes[i];
    }
    UIElement& appendNode(std::unique_ptr<UIElement> node)
    {
        nodes.push_back(std::move(node));
        return *nodes.back();
    }
};

struct UIContainer : UIElement
{
    //std::vector<std::unique_ptr<UIElement>> nodes; // moved to parent class to enable chaining
    UIContainer(float xMin = 0.f, float yMin = 0.f, float xSize = 1.f, float ySize = 1.f, int key = -1)
        : UIElement(xMin, yMin, xSize, ySize, key) {}

    void renderVerts(std::vector<float>& vertices)
    {
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            UIElement& node = *nodePtr;
            node.renderVerts(vertices);
        }
    }
};

struct UIXRatio : UIContainer
{
    float ratio;

    void adjustNode(float xMin, float yMin, float xSize, float ySize)
    {
        UIElement::adjustNode(xMin, yMin, xSize, ySize);
        float xxSize;
        float yySize;
        float xxMin;
        float yyMin;
        float sizeRatio = xSize / ySize;

        // goal is to maximize space consumed by internal node...
        // - while staying in parent node(this)...
        // - while maintaining xRatio
        // ratio is larger than sizeRatio Shrink x, else shrink y 
        if (sizeRatio > ratio)
        {
            //xxSize = xSize * ratio / sizeRatio;
            // sizeRatio = sSize / ySize 
            // xSize * ratio / sizeRatio = xSize * ratio / (sSize / ySize)
            // xSize * ratio / sizeRatio = ratio * xSize * (ySize / sSize)
            // xSize * ratio / sizeRatio = ratio * ySize
            xxSize = ratio * ySize;
            xxMin = xMin + (xSize * .5) - (xxSize * .5);
            yyMin = yMin;
            yySize = ySize;
        }
        else
        {
            xxMin = xMin;
            xxSize = xSize;
            yySize = xSize / ratio;
            yyMin = yMin + (ySize * .5) - (yySize * .5);
        }
    }
};

struct UIStack : UIContainer
{
    void adjustNode(float xMin, float yMin, float xSize, float ySize)
    {
        UIElement::adjustNode(xMin, yMin, xSize, ySize);
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            UIElement& node = *nodePtr;
            node.xMin = xMin;
            node.xSize = xSize;
            node.yMin = yMin;
            node.ySize = ySize;
            node.adjustNode(xMin, yMin, xSize, ySize);
        }
    }
};

struct UIXHolder : UIContainer
{
    void adjustNode(float xMin, float yMin, float xSize, float ySize)
    {
        UIElement::adjustNode(xMin, yMin, xSize, ySize);
        int n = nodes.size();
        if (n == 0) { return; }
        float yyMin = yMin;
        float yySize = ySize / n;
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            UIElement& node = *nodePtr;
            node.xMin = xMin;
            node.xSize = xSize;
            node.yMin = yyMin;
            yyMin += yySize;
            node.ySize = yySize;
            node.adjustNode(xMin, yMin, xSize, ySize);
        }
    }
};

struct UIYHolder : UIContainer
{
    UIYHolder(float xMin = 0.f, float yMin = 0.f, float xSize = 1.f, float ySize = 1.f, int key = -1)
        : UIContainer(xMin, yMin, xSize, ySize, key) {}

    void adjustNode(float xMin, float yMin, float xSize, float ySize)
    {
        UIElement::adjustNode(xMin, yMin, xSize, ySize);
        int n = nodes.size();
        if (n == 0) { return; }
        float yyMin = yMin;
        float yySize = ySize / n;
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            
            UIElement& node = *nodePtr;
            node.yMin = yyMin;
            yyMin += yySize;
            node.ySize = yySize;
            node.xMin = xMin;
            node.xSize = xSize;
            std::cout <<"ymin: "<< node.yMin << " ysize: " << node.ySize << " xmin: " << node.xMin << " xsize " << node.xSize << "\n";
            node.adjustNode(xMin, yMin, xSize, ySize);
        }
    }
};

struct OneLineTextNode : UIElement
{
    unsigned int textChannel;
};

struct TexUVNode : UIElement
{
    float xMinUV=0;
    float xMaxUv=1;
    float yMinUv=0;
    float yMaxUv=1;
    TexUVNode(float xMinUV=0, float xMaxUv = 1, float yMinUv = 0, float yMaxUv = 1)
    : xMinUV(xMinUV), xMaxUv(xMaxUv), yMinUv(yMinUv), yMaxUv(yMaxUv){}

    void renderVerts(std::vector<float>& vertices)
    {
        std::cout << "ymin" << yMin << "\n";
        vertices.insert(vertices.end(),
            {
            xMin + xSize, yMin + ySize, xMaxUv, yMaxUv, // v0
            xMin + xSize, yMin,         xMaxUv, yMinUv, // v1
            xMin,         yMin,         xMinUV, yMinUv, // v2

            xMin + xSize, yMin + ySize, xMaxUv, yMaxUv, // v0
            xMin,         yMin,         xMinUV, yMinUv, // v2
            xMin,         yMin + ySize, xMinUV, yMaxUv, // v3
            });
    }
};