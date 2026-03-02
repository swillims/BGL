

struct vectorFUtil
{
	// Intended Usage: This function write creates a quad(for batching) and then draws it from bottom left cornor
	// getVectorFromCornor can be used with rightAlign and centerAlign to shift X or Y axis. The naming is unintuitive but these function work on y axis as well.
	// It is better for performance to manually shift using shiftVert instead of align if possible.
	// This function does not handle sprites.
	static std::vector<float> getVectorFromCornor(float x, float y, float w, float h)
	{
		float xw = x + w;
		float yh = y + h;
		return
		{
			xw, yh, 1.0f, 1.0f, // v0
			xw, y, 1.0f, 0.0f, // v1
			x, y, 0.0f, 0.0f, // v2

			xw, yh, 1.0f, 1.0f, // v0
			x, y, 0.0f, 0.0f, // v2
			x, yh, 0.0f, 1.0f, // v3
		};
	}

	// untested
	static void shiftVert(std::vector<float>& target, float shiftAmount, int vertSize, int attributeNumber)
	{
		for (int i = attributeNumber; i < target.size(); i += vertSize)
		{
			target[i] += shiftAmount;
		}
	}
	// untested
	// right align a batched vector (assumed to be default left aligned before function)
	// 0 is default location for X values in a batch / buffer. Function can also be reused to align other axis backwards.
	static void rightAlign(std::vector<float>& target, int vertSize, int attributeNumber = 0) // 0 is default X
	{
		if (target.size() < vertSize || target.size() < attributeNumber) { return; }
		float min = target[attributeNumber];
		float max = min;
		for (int i = attributeNumber + vertSize; i < target.size(); i += vertSize)
		{
			if (min > target[i]) { min = target[i]; }
			else if (max < target[i]) { max = target[i]; }
		}
		shiftVert(target, min - max, vertSize, attributeNumber);
	}
	// untested
	// center align a batched vector (assumed to be default left aligned before function)
	// 0 is default location for X values in a batch / buffer. Function can also be reused to align other axis.
	static void centerAlign(std::vector<float>& target, int vertSize, int attributeNumber = 0) // 0 is default X
	{
		if (target.size() < vertSize || target.size() < attributeNumber) { return; }
		float min = target[attributeNumber];
		float max = min;
		for (int i = attributeNumber + vertSize; i < target.size(); i += vertSize)
		{
			if (min > target[i]) { min = target[i]; }
			else if (max < target[i]) { max = target[i]; }
		}
		shiftVert(target, -(min + max) / 2, vertSize, attributeNumber);
	}
};