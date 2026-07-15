#pragma once
#include "singleton/staticDraw.h"
#include "singleton/staticWrite.h"

// main purpose of UIElement is to return a vector<float> list for batching
// it also has secondary functionality for appending to a staticWrite channel

struct UIElement
{
    /*
        nodes is implemented in UIElement instead of container to make chains work
        it is memory inefficient but it chains which is a big trade off
        parent[0][0][1] is doable
        the alternative is a series of castings which is hell
        if using something that doesn't have subnodes just pretend nodes doesn't exist
    */
    std::vector<std::unique_ptr<UIElement>> nodes;
    UIElement* previousNode = nullptr;

    float xMin = -1.0f;
    float yMin = -1.0f;
    float xSize = 2.0f;
    float ySize = 2.0f;

    int key = -1;

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
                {
                    return key;
                }
                for (int i = 0; i < nodes.size(); i++)
                {
                    if (nodes[i]->findOneHover(x, y) != -1)
                    {
                        return nodes[i]->findOneHover(x, y);
                    }
                }
            }
        }
        return -1;
    }

    virtual UIElement& findByKey(int findKey)
    {
        if (findKey == key)
        {
            return *this;
        }
        for (int i = 0; i < nodes.size(); i++)
        {
            int k = nodes[i]->findByKey(findKey).key;
            if (k == findKey)
            {
                return nodes[i]->findByKey(findKey);
            }
        }
        return *this;
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

    virtual UIElement& setKey(int newKey)
    {
        key = newKey;
        return *this;
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
        node->previousNode = this;
        nodes.push_back(std::move(node));
        return *nodes.back();
    }
    template<typename T, typename... Args>
    UIElement& appendType(Args&&... args)
    {
        nodes.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        nodes.back()->previousNode = this;
        return *nodes.back();
    }

    UIElement& back()
    {
        if (previousNode == nullptr) { return *this; }
        return *previousNode;
    }

    template<typename T, typename... Args>
    std::vector<std::unique_ptr<UIElement>>& appendSameType(int count, Args&&... args)
    {
        for (int i = 0; i < count; i++)
        {
            nodes.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        }
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

    virtual std::vector<float> getVerts()
    {
        return {
            xMin + xSize, yMin + ySize, 1, 1, // v0
            xMin + xSize, yMin,         1, 0, // v1
            xMin,         yMin,         0, 0, // v2

            xMin + xSize, yMin + ySize, 1, 1, // v0
            xMin,         yMin,         0, 0, // v2
            xMin,         yMin + ySize, 0, 1, // v3
        };
    }
};

struct UIEmpty : UIElement
{
    UIEmpty():UIElement(-1){}
};

struct UIContainer : UIElement
{
    UIContainer(int key = -1): UIElement(key) {}

    void renderVerts(std::vector<float>& vertices) override
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

    UIXRatio(float ratio, bool relativeToScreenSize = true, int key = -1)
        : ratio(ratio), relativeToScreenSize(relativeToScreenSize), UIContainer(key) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2) override
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
    float& xLeft; // 0 to 1.0
    float& xSubSize; // 0 to 1.0

    UIXShifter(float& xLeft, float& xSubSize, int key = -1)
        : UIContainer(key), xLeft(xLeft), xSubSize(xSubSize){}
    void adjustNode(float xMin, float yMin, float xSize, float ySize) override
    {
        UIElement::adjustNode(xMin, yMin, xSize, ySize);
        if (nodes.size() > 0)
        {
            nodes[0]->adjustNode(xMin + xSize * xLeft, yMin, xSize * xSubSize, ySize);
        }
    }
};

struct UIYShifter : UIContainer
{
    float& yBottom;   // 0.0 to 1.0
    float& ySubSize;  // 0.0 to 1.0

    UIYShifter(float& yBottom, float& ySubSize, int key = -1)
        : UIContainer(key), yBottom(yBottom), ySubSize(ySubSize) {}

    void adjustNode(float xMin, float yMin, float xSize, float ySize) override
    {
        UIElement::adjustNode(xMin, yMin, xSize, ySize);

        if (!nodes.empty())
        {
            nodes[0]->adjustNode(
                xMin,
                yMin + ySize * yBottom,
                xSize,
                ySize * ySubSize
            );
        }
    }
};

struct UIStack : UIContainer
{

    void adjustNode(float xMin, float yMin, float xSize, float ySize) override
    {
        UIElement::adjustNode(xMin, yMin, xSize, ySize);
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            UIElement& node = *nodePtr;
            node.adjustNode(xMin, yMin, xSize, ySize);
        }
    }
};

struct UIBase : UIStack
{
    UIBase()
    {
        xMin = -1;
        yMin = -1;
        xSize = 2;
        ySize = 2;
        key = -1;
    }
};

struct UIXHolder : UIContainer
{
    UIXHolder(int key = -1): UIContainer(key) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2) override
    {
        UIElement::adjustNode(xMin2, yMin2, xSize2, ySize2);
        int n = nodes.size();
        if (n == 0) { return; }
        float xxMin = xMin;
        float xxSize = xSize / n;
        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
            UIElement& node = *nodePtr;
            node.adjustNode(xxMin, yMin2, xxSize, ySize2);
            xxMin += xxSize;
        }
    }
};

struct UIYHolder : UIContainer
{
    UIYHolder(int key = -1): UIContainer(key) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2) override
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
        }
    }
};

struct UIXSplits : UIContainer
{
    std::vector<float> splits;

    UIXSplits(std::vector<float> splits, int key = -1)
        : UIContainer(key), splits(splits) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2) override
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

struct UIYSplits : UIContainer
{
    std::vector<float> splits;

    UIYSplits(std::vector<float> splits, int key = -1)
        : UIContainer(key), splits(splits) {}

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2)
    {
        UIElement::adjustNode(xMin2, yMin2, xSize2, ySize2);
        float yySize;
        float yyMin = yMin2 + ySize2;

        for (int i = 0; i < nodes.size() && i < splits.size(); i++)
        {
            yySize = ySize2 * splits[i];
            yyMin -= yySize;
            UIElement& node = *nodes[i];
            node.adjustNode(xMin2, yyMin, xSize2, yySize);
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

    void adjustNode(float xMin2, float yMin2, float xSize2, float ySize2) override
    {
        UIElement::adjustNode(xMin2, yMin2, xSize2, ySize2);
        xMin2 += xSize2 * xLeftBuffer;
        xSize2 -= xSize2 * (xLeftBuffer + xRightBuffer);
        yMin2 += ySize2 * yBottomBuffer;
        ySize2 -= ySize2 * (yBottomBuffer + yTopBuffer);

        for (std::unique_ptr<UIElement>& nodePtr : nodes)
        {
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
        int xAlign = XCENTER, int yAlign = YCENTER, bool relativeToScreenSize = true, int key = -1)
        : UIElement(key), textSource(textSource), textChannel(textChannel), fontPercent(fontPercent), 
        xAlign(xAlign), yAlign(yAlign), relativeToScreenSize(relativeToScreenSize) {}
    
    void renderVerts(std::vector<float>& vertices) override
    {
        float yScale = fontPercent * ySize;
        float xScale = yScale;
        if (relativeToScreenSize)
        {
            xScale /= StaticDraw::aspectRatio;
        }
        std::vector<float> textVerts = StaticWrite::GenerateVertices(textSource, xMin, yMin, xScale, yScale);

        if (textVerts.size() > 0 && textVerts.size()%4==0)
        {
            if (xAlign == XCENTER || xAlign == XRIGHT)
            {
                float xxMax = textVerts[0];
                for (int i = 4; i < textVerts.size(); i += 4)
                {
                    if (xxMax < textVerts[i]) { xxMax = textVerts[i]; }
                }
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
    }
};

struct UITextOneLineConst : UIElement
{
    unsigned int textChannel;
    float fontPercent;
    std::string textSource;
    bool relativeToScreenSize;
    int xAlign;
    int yAlign;

    UITextOneLineConst(unsigned int textChannel, std::string textSource, float fontPercent,
        int xAlign = XCENTER, int yAlign = YCENTER, bool relativeToScreenSize = true, int key = -1)
        : UIElement(key), textSource(textSource), textChannel(textChannel), fontPercent(fontPercent),
        xAlign(xAlign), yAlign(yAlign), relativeToScreenSize(relativeToScreenSize) {}

    void renderVerts(std::vector<float>& vertices) override
    {
        float yScale = fontPercent * ySize;
        float xScale = yScale;
        if (relativeToScreenSize)
        {
            xScale /= StaticDraw::aspectRatio;
        }
        std::vector<float> textVerts = StaticWrite::GenerateVertices(textSource, xMin, yMin, xScale, yScale);

        if (textVerts.size() > 0 && textVerts.size()%4==0)
        {
            if (xAlign == XCENTER || xAlign == XRIGHT)
            {
                float xxMax = textVerts[0];
                for (int i = 4; i < textVerts.size(); i += 4)
                {
                    if (xxMax < textVerts[i]) { xxMax = textVerts[i]; }
                }
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

    void renderVerts(std::vector<float>& vertices) override
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

    std::vector<float> getVerts() override
    {
        return
        {
        xMin + xSize, yMin + ySize, xMaxUv, yMaxUv, // v0
        xMin + xSize, yMin,         xMaxUv, yMinUv, // v1
        xMin,         yMin,         xMinUV, yMinUv, // v2

        xMin + xSize, yMin + ySize, xMaxUv, yMaxUv, // v0
        xMin,         yMin,         xMinUV, yMinUv, // v2
        xMin,         yMin + ySize, xMinUV, yMaxUv, // v3
        };
    }
};