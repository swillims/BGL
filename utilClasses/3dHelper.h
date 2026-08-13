#pragma once
#include <vector>

enum
{
    // the enum values default at 0 and increment.
    // - I am being explicit because it is extremely important that they align with specific rows.
    paramX = 0,
    paramY = 1,
    paramZ = 2,
};

static std::vector<float> generateFlatGrid5Vao(int min1, int max1, int min2, int max2, int param1 = paramX, int param2 = paramY, float scale1 = 1.0f, float scale2 = 1.0f, float value3 = 0.0f )
{
    std::vector<float> grid;
    float cordsMin[3];
    std::fill(std::begin(cordsMin), std::end(cordsMin), value3);
    float cordsMax[3];
    std::fill(std::begin(cordsMax), std::end(cordsMax), value3);

    cordsMin[param1] = min1 * scale1;
    cordsMin[param2] = min2 * scale2;

    cordsMax[param1] = cordsMin[param1] + scale1;
    cordsMax[param2] = cordsMin[param2] + scale2;

    for (int i = min1; i <= max1; i++)
    {
        cordsMax[param1] = cordsMin[param1] + scale1;

        for (int j = min2; j <= max2; j++)
        {
            cordsMax[param2] = cordsMin[param2] + scale2;

            grid.insert(grid.end(),
                {
                    cordsMin[0], cordsMin[1], cordsMin[2], 0.0f, 0.0f,
                    cordsMax[0], cordsMin[1], cordsMin[2], 1.0f, 0.0f,
                    cordsMax[0], cordsMax[1], cordsMax[2], 1.0f, 1.0f,

                    cordsMin[0], cordsMin[1], cordsMin[2], 0.0f, 0.0f,
                    cordsMax[0], cordsMax[1], cordsMax[2], 1.0f, 1.0f,
                    cordsMin[0], cordsMax[1], cordsMax[2], 0.0f, 1.0f
                });

            cordsMin[param2] = cordsMax[param2];
        }
        cordsMin[param2] = min2 * scale2;
        cordsMin[param1] = cordsMax[param1];
    }
    return grid;
}
