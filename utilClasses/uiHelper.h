#pragma once
#include "singleton/staticDraw.h" // used for ratio
#include "singleton/staticWrite.h" // used for appending text to a channel
//#include "vectorFUtil.h"

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
        if using something that doesn't have subnodes just pretend nodes doesn't exist
    */
    std::vector<std::unique_ptr<UIElement>> nodes;

    float xMin;
    float yMin;
    float xSize;
    float ySize;

    int key; // key is referenced

    UIElement(){}
    UIElement(int key = -1): key(key) {}
    UIElement(float xMin, float yMin, float xSize, float ySize, int key=-1) 
        : xMin(xMin), yMin(yMin), xSize(xSize), ySize(ySize), key(key){}

    virtual ~UIElement() = default;
    
    virtual int findOneHover(float x, float y)
    {
        if (x >= xMin && y >= yMin)
        {
            if (x <= xMin + xSize && y <= yMin + ySize)
            {
                if (key != -1)
                    return key;
                for (int i = 0; i < nodes.size(); i++)
                {
                    if (nodes[i]->findOneHover(x, y) != -1)
                        return nodes[i]->findOneHover(x, y);
                }
            }
        }
        return -1;
    }

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
    virtual void adjustNodeDefault() { adjustNode(xMin,yMin,xSize,ySize); }
    virtual void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2)
    {
        xMin = xMin2;
        yMin = yMin2;
        xSize = xSize2;
        ySize = ySize2;
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
    template<typename T, typename... Args>
    UIElement& appendType(Args&&... args)
    {
        /*
        //std::make_shared<A>(node);
        auto node = std::make_unique<T>(std::forward<Args>(args)...);
        nodes.push_back(std::move(node));
        //nodes.push_back(node.clone());
        //nodes.push_back(std::make_shared<UIElement>(node););
        return *nodes.back();
        */
        nodes.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return *nodes.back();
    }

    UIElement& appendSameNode(std::unique_ptr<UIElement> node, int count)
    {
        for (int i = 0; i < count; i++)
        {
            //nodes.push_back(node);
        }
        return *node;
    }

    template<typename T, typename... Args>
    std::vector<std::unique_ptr<UIElement>>& appendSameType(int count, Args&&... args)
    {
        for (int i = 0; i < count; i++)
        {
            //auto node = std::make_unique<T>(std::forward<Args>(args)...);
            //nodes.push_back(std::move(node));
            nodes.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        }
        std::cout << "here size nodes" << nodes.size() << "\n";
        return nodes;
    }

    int size()
    {
        return nodes.size();
    }

    void debugTreePrint()
    {
        std::cout << "node tree here\n";
        for (int i = 0; i < nodes.size(); i++)
        {
            std::cout << i << ": <-\n";
            nodes[i]->debugTreePrint();
        }
        std::cout << "node end here\n";
    }
};

struct UIEmpty : UIElement
{
    UIEmpty():UIElement(-1){}
};

struct UIContainer : UIElement
{
    //std::vector<std::unique_ptr<UIElement>> nodes; // moved to parent class to enable chaining
    UIContainer(float xMin = 0.f, float yMin = 0.f, float xSize = 1.f, float ySize = 1.f, int key = -1)
        : UIElement(xMin, yMin, xSize, ySize, key) {}
    UIContainer(int key = -1): UIElement(key) {}

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
    bool relativeToScreenSize;

    //UIXRatio() {}
    UIXRatio(float xMin, float yMin, float xSize, float ySize, float ratio, bool relativeToScreenSize = true, int key = -1)
        : UIContainer(xMin, yMin, xSize, ySize, key), ratio(ratio), relativeToScreenSize(relativeToScreenSize) {}
    UIXRatio(float ratio, bool relativeToScreenSize = true, int key = -1)
        : ratio(ratio), relativeToScreenSize(relativeToScreenSize), UIContainer(key) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2)
    {
        UIElement::adjustNode(xMin2, yMin2, xSize2, ySize2);
        float xxSize;
        float yySize;
        float xxMin;
        float yyMin;
        float r = ratio;
        float sizeRatio = xSize / ySize;
        if (relativeToScreenSize)
        {
            r /= StaticDraw::aspectRatio;
        }
        if (sizeRatio > r)
        {
            xxSize = r * ySize;
            xxMin = xMin + (xSize * .5) - (xxSize * .5);
            yyMin = yMin;
            yySize = ySize;
        }
        else
        {
            xxMin = xMin;
            xxSize = xSize;
            yySize = xSize / r;
            yyMin = yMin + (ySize * .5) - (yySize * .5);
        }
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            UIElement& node = *nodePtr;
            node.adjustNode(xxMin, yyMin, xxSize, yySize);
        }
    }
};

struct UIXShifter : UIContainer
{
    // using non-recommended numbers it technically valid but it should make findOneHover not work relative to parent container
    float& xLeft; // 0 to 1.0
    float& xSubSize; // 0 to 1.0

    UIXShifter(float& xLeft, float& xSubSize, int key = -1)
        : UIContainer(key), xLeft(xLeft), xSubSize(xSubSize){}
    void adjustNode(float xMin, float yMin, float xSize, float ySize)
    {
        UIElement::adjustNode(xMin, yMin, xSize, ySize);
        if (nodes.size() > 0)
        {
            nodes[0]->adjustNode(xMin + xSize * xLeft, yMin, xSize * xSubSize, ySize);
        }
    }
};

struct UIStack : UIContainer
{
    UIStack(float xMin = 0.f, float yMin = 0.f, float xSize = 1.f, float ySize = 1.f, int key = -1)
        : UIContainer(xMin, yMin, xSize, ySize, key) {}

    void adjustNode(float xMin, float yMin, float xSize, float ySize)
    {
        UIElement::adjustNode(xMin, yMin, xSize, ySize);
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            UIElement& node = *nodePtr;
            node.adjustNode(xMin, yMin, xSize, ySize);
        }
    }
};

// this is broken and wrong. I haven't gotten around to fixing it
struct UIXHolder : UIContainer
{
    UIXHolder(float xMin = 0.f, float yMin = 0.f, float xSize = 1.f, float ySize = 1.f, int key = -1)
        : UIContainer(xMin, yMin, xSize, ySize, key) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2)
    {
        UIElement::adjustNode(xMin2, yMin2, xSize2, ySize2);
        int n = nodes.size();
        if (n == 0) { return; }
        float xxMin = xMin;
        float xxSize = xSize / n;
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            UIElement& node = *nodePtr;
            //node.xMin = xMin;
            //node.xSize = xSize;
            //node.yMin = yyMin;
            
            //node.ySize = yySize;
            node.adjustNode(xxMin, yMin2, xxSize, ySize2);
            xxMin += xxSize;
        }
    }
};

struct UIYHolder : UIContainer
{
    UIYHolder(float xMin = 0.f, float yMin = 0.f, float xSize = 1.f, float ySize = 1.f, int key = -1)
        : UIContainer(xMin, yMin, xSize, ySize, key) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2)
    {
        UIElement::adjustNode(xMin2, yMin2, xSize2, ySize2);
        int n = nodes.size();
        if (n == 0) { return; }
        float yyMin = yMin+ySize; // looks backwords but working backwords
        float yySize = ySize / n;
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            yyMin -= yySize;
            UIElement& node = *nodePtr;
            node.adjustNode(xMin, yyMin, xSize, yySize);
            //yyMin += yySize;
        }
    }
};

struct UIXSplits : UIContainer
{
    std::vector<float> splits;

    UIXSplits(float xMin, float yMin, float xSize, float ySize, std::vector<float> splits, int key = -1)
        : UIContainer(xMin, yMin, xSize, ySize, key), splits(splits){}
    UIXSplits(std::vector<float> splits, int key = -1)
        : UIContainer(key), splits(splits) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2)
    {
        UIElement::adjustNode(xMin2, yMin2, xSize2, ySize2);
        float xxSize;
        float xxMin = xMin2;

        for (int i = 0; i < nodes.size() && i < splits.size(); i++)
        {
            xxSize = xSize2 * splits[i];
            UIElement& node = *nodes[i];
            node.adjustNode(xxMin, yMin2, xxSize, ySize2);
            xxMin += xxSize;
        }
    }
};

struct UIBuffer : UIContainer
{
    float xLeftBuffer;
    float xRightBuffer;
    float yBottomBuffer;
    float yTopBuffer;
    
    UIBuffer(float all, int key = -1)
        : UIContainer(key), xLeftBuffer(all), xRightBuffer(all), yBottomBuffer(all), yTopBuffer(all) {}

    UIBuffer(float x, float y, int key = -1)
        : UIContainer(key), xLeftBuffer(x), xRightBuffer(x), yBottomBuffer(y), yTopBuffer(y) {}

    UIBuffer(float xL, float xR, float yB, float yT, int key = -1)
        : UIContainer(key), xLeftBuffer(xL), xRightBuffer(xR), yBottomBuffer(yB), yTopBuffer(yT) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2)
    {
        UIElement::adjustNode(xMin2, yMin2, xSize2, ySize2);
        std::cout << "pre math: xmin: " << xMin2 << " ysize: " << ySize2 << " ymin: " << yMin2 << " ysize: " << ySize2 << "\n";
        xMin2 += xSize2 * xLeftBuffer;
        xSize2 -= xSize2 * (xLeftBuffer + xRightBuffer);
        yMin2 += ySize2 * yBottomBuffer;
        ySize2 -= ySize2 * (yBottomBuffer + yTopBuffer);

        std::cout << "post math: xmin: " << xMin2 << " ysize: " << ySize2 << " ymin: " << yMin2 << " ysize: " << ySize2 << "\n";

        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            std::cout << "node exists\n";
            UIElement& node = *nodePtr;
            node.adjustNode(xMin2, yMin2, xSize2, ySize2);
        }
    }
};

enum TextXAlign
{
    XLEFT = -1,
    XCENTER = 0,
    XRIGHT = 1
};
enum TextYAlign
{
    YBOTTOM = -1,
    YCENTER = 0,
    YTOP = 1
};

struct UITextOneLine : UIElement
{
    unsigned int textChannel;
    float fontPercent;
    std::string& textSource;
    bool relativeToScreenSize;
    int xAlign;
    int yAlign;

    UITextOneLine(unsigned int textChannel, std::string& textSource, float fontPercent, 
        int xAlign = XCENTER, int yAlign = YCENTER, bool relativeToScreenSize = true, int key = -1) // you really shouldn't use key with text
        : UIElement(key), textSource(textSource), textChannel(textChannel), fontPercent(fontPercent), 
        xAlign(xAlign), yAlign(yAlign), relativeToScreenSize(relativeToScreenSize) {}
    
    void renderVerts(std::vector<float>& vertices)
    {
        float yScale = fontPercent * ySize;
        float xScale = yScale;
        if (relativeToScreenSize)
        {
            xScale /= StaticDraw::aspectRatio;
        }
        std::vector<float> textVerts = StaticWrite::GenerateVertices(textSource, xMin, yMin, xScale, yScale);

        
        // this code is scapped becuase it is centering relative to total text size and not window size
        /*
        // magic number 4 is X,Y,U,V
        if (xAlign == XCENTER)
        {
            VectorFUtil::centerAlign(textVerts, 4);
        }
        else if (xAlign == XRIGHT)
        {
            VectorFUtil::rightAlign(textVerts, 4);
        }
        */
        // magic number 4 is X,Y,U,V
        // check if textVerts has vertices and is formatted correctly
        //std::cout << "size: " << textVerts.size() << "\n";
        //std::cout << "%: " << textVerts.size() % 4 << "\n";
        if (textVerts.size() > 0 && textVerts.size()%4==0)
        {
            // check if needs to alignX
            if (xAlign == XCENTER || xAlign == XRIGHT)
            {
                // calc min and max xValues
                //float xxMin = textVerts[0];
                float xxMax = textVerts[0];
                for (int i = 4; i < textVerts.size(); i += 4)
                {
                    //code changed because xxMin does not need to be solved, it should be same as xMin
                    //if (xxMin > textVerts[i]) { xxMin = textVerts[i]; }
                    //else if (xxMax < textVerts[i]) { xxMax = textVerts[i]; }
                    if (xxMax < textVerts[i]) { xxMax = textVerts[i]; }
                }
                //std::cout << "Text: " << textSource << "\nxMin: " << xxMin << "\nxMax" << xxMax << "\n";
                //float xxRange = xxMax - xxMin;
                //float shiftAmount = xSize - xxRange;
                // xxMin and xMin are same number
                float shiftAmount = xSize + xMin - xxMax;
                if (xAlign == XCENTER)
                {
                    shiftAmount /= 2;
                }
                for (int i = 0; i < textVerts.size(); i += 4)
                {
                    textVerts[i] += shiftAmount;
                }
            }
            if (yAlign == YCENTER || YTOP)
            {
                float yyMax = textVerts[1];
                for (int i = 5; i < textVerts.size(); i += 4)
                {
                    if (yyMax < textVerts[i]) { yyMax = textVerts[i]; }
                }
                float shiftAmount = ySize + yMin - yyMax;
                if (yAlign == XCENTER)
                {
                    shiftAmount /= 2;
                }
                for (int i = 1; i < textVerts.size(); i += 4)
                {
                    textVerts[i] += shiftAmount;
                }
            }
            StaticWrite::AppendChannel(textChannel, textVerts);
        }
        //StaticWrite::AppendChannel(textChannel, textVerts);
        //StaticWrite::AppendText(textChannel, textSource, xMin, yMin, xScale, yScale);
    }
};

struct TexUVNode : UIElement
{
    float xMinUV=0;
    float xMaxUv=1;
    float yMinUv=0;
    float yMaxUv=1;
    TexUVNode(float xMinUV=0, float xMaxUv = 1, float yMinUv = 0, float yMaxUv = 1, int key = -1)
    : xMinUV(xMinUV), xMaxUv(xMaxUv), yMinUv(yMinUv), yMaxUv(yMaxUv), UIElement(key){}

    void renderVerts(std::vector<float>& vertices)
    {
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