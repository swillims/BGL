#pragma once

#include "singleton/gl_core.h"

#include <string>
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <ranges>

#include "util.h"
#include <glm/glm.hpp>

struct StaticDraw
{
    // draw window
    inline static GLFWwindow* window;

    // ints used by buffers
    inline static unsigned int VAOSimple, VBO, EBO;

    struct VAOInfo
    {
        std::string name;
        GLuint ref;
        int floatCount; // idk a good name for this each vertex has x amount of floats in side of it. X,Y,U,V is 4. X,Y,Z,U,V is 5

        VAOInfo(std::string name, GLuint ref, int floatCount) : name(name), ref(ref), floatCount(floatCount){}
    };

    inline static std::vector<VAOInfo> VAOs;

    struct UBOInfo
    {
        std::string name;
        GLuint ref;
        GLuint binding;

        UBOInfo(const std::string& name, GLuint ref, GLuint binding) : name(name), ref(ref), binding(binding) {};
    };

    inline static std::vector<UBOInfo> UBOs;

    // x and y
    inline static int w, h;
    inline static float aspectRatio;

    // shader(program) references
    inline static int simpleRef;
    inline static int currentRef;

    // loaded files
    inline static util::BiMap<unsigned int, std::string> shaderRefs;
    inline static util::BiMap<unsigned int, std::string> imageFileRefs;

    static void windowImply() { window = glfwGetCurrentContext(); }
    static void windowSpecify(GLFWwindow* win) { window = win; }

    static void resize(int newW, int newH);

    static void updateView();

    // sets indices to default
    static void cleanIndices()
    {
        unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };
        glBindVertexArray(VAOSimple);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
        glBindVertexArray(0);
    }

    static void init()
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        windowImply();

        const char* vertexShaderSource = R"(assets/shaders/simple.vs)";
        const char* fragmentShaderSource = R"(assets/shaders/simple.fs)";
        compileShader(vertexShaderSource, fragmentShaderSource, "simple");
        useShader(simpleRef);
        updateView();
        
        glGenVertexArrays(1, &VAOSimple);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        VAOs.emplace_back("VAOSimple",VAOSimple, 4);

        glBindVertexArray(VAOSimple);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        cleanIndices();
    }

    static unsigned int compileShader(const char* vertexSource, const char* fragmentSource, std::string shaderName = "")
    {

        std::cout << "Loading shader" << shaderName << "\n" << vertexSource << "\n" << fragmentSource << "\n";
        
        // Borrowed tutorial code starts here. This section is modified/reverse engineered from a book.

        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            //openfiles
            vShaderFile.open(vertexSource);
            fShaderFile.open(fragmentSource);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::File read ;-;" << std::endl;
            std::cout << vertexSource << fragmentSource << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
                infoLog << std::endl;
            glDeleteShader(vertex);
            return 0;
        };
        // non-specified fragment code
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
                infoLog << std::endl;
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return 0;
        };
        // ...
        int iD = glCreateProgram(); // I changed iD to use later
        glAttachShader(iD, vertex);
        glAttachShader(iD, fragment);
        glLinkProgram(iD);
        //printlinkingerrorsifany
        glGetProgramiv(iD, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(iD, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
                infoLog << std::endl;
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            glDeleteProgram(iD);
            return 0;
        }
        //deleteshaders;they�re linked into our program and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        
        // borrowed code gone
        // save iD and name pair
        if (shaderName == "") { shaderName = std::to_string(iD); }
        shaderRefs.insert(iD, shaderName);
        return iD;
    }

    static void useShader(unsigned int shaderRef)
    {
        currentRef = shaderRef;
        glUseProgram(shaderRef);
    }

    static void useShaderSimple(){ useShader(simpleRef); }

    static int getShader(const std::string& shaderString){return shaderRefs[shaderString];}
    static std::string getShader(const int shaderInt){return shaderRefs[shaderInt];}

    static bool hasShader(const std::string& shaderString) {return shaderRefs.contains(shaderString);}
    static bool hasShader(const int shaderInt){return shaderRefs.contains(shaderInt);}

    // there is no "use" call like there is with shader and texture because VAO is bound every draw

    static VAOInfo CreateVAO(const std::vector<int>& attributes, const std::string& vaoString)
    {
        GLuint vao;
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        int stride = 0;
        for (int size : attributes)
        {
            stride += size;
        }

        int offset = 0;
        for (int i = 0; i < attributes.size(); i++)
        {
            glVertexAttribPointer
            (
                i,
                attributes[i],
                GL_FLOAT,
                GL_FALSE,
                stride * sizeof(float),
                (void*)(offset * sizeof(float))
            );
            glEnableVertexAttribArray(i);

            offset += attributes[i];
        }
        glBindVertexArray(0);

        VAOs.emplace_back(vaoString, vao, stride);
        return VAOs.back();
    }

    static void deleteVAO(std::string name)
    {
        for (int i = 0; i < VAOs.size(); i++)
        {
            if (VAOs[i].name == name)
            {
                glDeleteVertexArrays(1, &VAOs[i].ref);
                VAOs.erase(VAOs.begin() + i);
                return;
            }
        }
    }

    static void deleteVAO(int ref)
    {
        for (int i = 0; i < VAOs.size(); i++)
        {
            if (VAOs[i].ref == ref)
            {
                glDeleteVertexArrays(1, &VAOs[i].ref);
                VAOs.erase(VAOs.begin() + i);
                return;
            }
        }
    }

    static VAOInfo getVAO(const std::string& vaoString)
    {
        for (int i = 0; i < VAOs.size(); i++)
        {
            if (VAOs[i].name == vaoString) { return VAOs[i];}
        }
        return {"ERROR: VAO NOT FOUND",0,0};
    }

    static VAOInfo getVAO(const int vaoInt)
    {
        for (int i = 0; i < VAOs.size(); i++)
        {
            if (VAOs[i].ref == vaoInt) { return VAOs[i];}
        }
        return {"ERROR: VAO NOT FOUND",0,0};
    }

    static bool hasVAO(const std::string& vaoString)
    {
        for (int i = 0; i < VAOs.size(); i++)
        {
            if (VAOs[i].name == vaoString) { return true;}
        }
        return false;
    }
    static bool hasVAO(const int vaoInt)
    {
        for (int i = 0; i < VAOs.size(); i++)
        {
            if (VAOs[i].ref == vaoInt) { return true;}
        }
        return false;
    }

    /*template<typename T>
    static UBOInfo createSharedShaderVariable(const T& data, int shaderRef, const std::string& varName, int binding = -1)
    {
        const GLuint blockIndex = glGetUniformBlockIndex(shaderRef, varName.c_str());

        if (blockIndex == GL_INVALID_INDEX)
        {
            std::cout<<"invalid shader var call\n";
            return {"",0,0};
        }

        if (binding == -1)
        {
            binding = UBOs.size();
        }

        glUniformBlockBinding(shaderRef, blockIndex, binding);

        GLuint ubo;

        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);

        glBufferData(
            GL_UNIFORM_BUFFER,
            sizeof(T),
            &data,
            GL_DYNAMIC_DRAW
        );

        glBindBufferBase(
            GL_UNIFORM_BUFFER,
            binding,
            ubo
        );

        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        UBOs.emplace_back(varName, ubo, binding);

        return UBOs.back();
    }*/
    template<typename T>
    static UBOInfo createSharedShaderVariable(const T& data, const std::string& name, int binding = -1)
    {
        if (binding == -1){binding = UBOs.size();}

        GLuint ubo;

        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);

        glBufferData(
            GL_UNIFORM_BUFFER,
            sizeof(T),
            &data,
            GL_DYNAMIC_DRAW
        );

        glBindBufferBase(
            GL_UNIFORM_BUFFER,
            binding,
            ubo
        );

        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        UBOs.emplace_back(name, ubo, binding);

        return UBOs.back();
    }

    static void bindSharedShaderVariable(int shaderRef, const std::string& varName, GLuint binding=-1)
    {
        GLuint blockIndex = glGetUniformBlockIndex(shaderRef, varName.c_str());
        if (blockIndex == GL_INVALID_INDEX)
        {
            std::cout << "shader bind is for invalid shader index ;-;\n";
            return;
        }
        if (binding == -1)
        {
            // this will fail silently which is fine
            binding = getSharedShaderVariable(varName).binding;
        }
        glUniformBlockBinding(shaderRef, blockIndex, binding);
    }

    static bool hasSharedShaderVariable(const std::string& ref)
    {
        for (const UBOInfo& ubo : UBOs){if (ubo.name == ref){return true;}}
        return false;
    }
    static UBOInfo getSharedShaderVariable(const std::string& ref)
    {
        for (const UBOInfo& ubo : UBOs){if (ubo.name == ref){return ubo;}}
        return {"",-0,0};
    }

    template<typename T>
    static void updateSharedShaderVariable(GLuint ref, const T& data)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, ref);

        glBufferSubData(
            GL_UNIFORM_BUFFER,
            0,
            sizeof(T),
            &data
        );

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    template<typename T>
    static void updateSharedShaderVariable(UBOInfo& ubo, const T& data)
    {
        updateSharedShaderVariable(ubo.ref, data);
    }



    // This one is a debug method. I recommend deleting it if you modify my engine and make a custom engine.
    static void printShaderUniforms(GLuint programID) {
        GLint uniformCount;
        glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &uniformCount);

        std::cout << "Active uniforms in program " << programID << ":\n";

        for (GLint i = 0; i < uniformCount; ++i) {
            char name[256];
            GLsizei length;
            GLint size;
            GLenum type;

            glGetActiveUniform(programID, i, sizeof(name), &length, &size, &type, name);
            GLint location = glGetUniformLocation(programID, name);

            std::cout << "  #" << i << ": " << name
                << " | location: " << location
                << " | type: " << std::hex << type << std::dec << "\n";
        }
    }

    static void unLoadShader(const std::string& shaderString)
    {
        shaderRefs.erase(shaderString);
    }
    static void unLoadShader(const int shaderInt)
    {
        shaderRefs.erase(shaderInt);
    }

    static bool hasImage(std::string stringRef){return imageFileRefs.contains(stringRef);}

    static int getImageRef(std::string stringRef) { return imageFileRefs[stringRef]; }

    static void loadImage(std::string fileName, std::string imageName = "", bool flip = false);

    static void unLoadImage(const std::string& ref);

    static void unLoadImage(unsigned int ref);

    static void loadFolder(const std::string& folderPath)
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(folderPath))
        {
            if (entry.is_regular_file())
            {
                std::string pathStr = entry.path().string();
                loadImage(pathStr); // use path as its own name //, "", clamp
            }
        }
    }

    static void checkCompileErrors(unsigned int shader, const std::string& type)
    {
        int success;
        char infoLog[512];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 512, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 512, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
            }
        }
    }

    // 0 is center for xCenter and yCenter
    static void halfDimImage(int imageRef, float xCenter, float yCenter, float halfWidth, float halfHeight)
    {
        // I tried both the elements and draw arrays methods. They are basically the same performance for non-batched.
        // If you have a preference, you do you. 
        //*
        float vertices[] = {
             xCenter + halfWidth, yCenter + halfHeight, 1.0f, 1.0f, // top right
             xCenter + halfWidth, yCenter - halfHeight, 1.0f, 0.0f, // bottom right
             xCenter - halfWidth, yCenter - halfHeight, 0.0f, 0.0f, // bottom left
             xCenter - halfWidth, yCenter + halfHeight, 0.0f, 1.0f  // top left
        };
        //*/ 
        glBindVertexArray(VAOSimple);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindTexture(GL_TEXTURE_2D, imageRef);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    static void halfDimImage(std::string stringRef, float xCenter, float yCenter, float halfWidth, float halfHeight) { halfDimImage(imageFileRefs[stringRef], xCenter, yCenter, halfWidth, halfHeight);}
    
    static void spriteImage(int imageRef, float xCenter, float yCenter, float halfWidth, float halfHeight, int frameX, int frameY, int xFrames, int yFrames)
    {
        float xRatio = 1.0f / xFrames;
        float yRatio = 1.0f / yFrames;
        float x1 = xRatio * frameX;
        float x2 = xRatio * (frameX + 1);
        float y1 = yRatio * frameY;
        float y2 = yRatio * (frameY + 1);
        float vertices[] = {
             xCenter + halfWidth, yCenter + halfHeight, x2, y1, // top right
             xCenter + halfWidth, yCenter - halfHeight, x2, y2, // bottom right
             xCenter - halfWidth, yCenter - halfHeight, x1, y2, // bottom left
             xCenter - halfWidth, yCenter + halfHeight, x1, y1  // top left
        };

        glBindVertexArray(VAOSimple);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindTexture(GL_TEXTURE_2D, imageRef);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }

    // I probably won't delete this. It works(on an older version of the engine). Rotating using vertex shader is a better alternative. If you want to do things the wrong way, go ahead and uncomment the next method.
    // -  CPU side rotational transformations are suboptimal and should be done gpu side
    /*
    static void spriteRotatedImage(int imageRef, float xCenter, float yCenter, float radians, float halfWidth, float halfHeight, int frameX, int frameY, int xFrames, int yFrames)
    {
        // UVs for the sprite frame
        float xRatio = 1.0f / xFrames;
        float yRatio = 1.0f / yFrames;
        float x1 = xRatio * frameX;
        float x2 = xRatio * (frameX + 1);
        float y1 = yRatio * frameY;
        float y2 = yRatio * (frameY + 1);

        // Corners relative to center
        float corners[4][2] = {
            { halfWidth,  halfHeight },  // top-right
            { halfWidth, -halfHeight },  // bottom-right
            {-halfWidth, -halfHeight },  // bottom-left
            {-halfWidth,  halfHeight }   // top-left
        };

        float c = cos(-radians);
        float s = sin(-radians);

        float vertices[16]; // 4 vertices * (x, y, u, v)

        for (int i = 0; i < 4; ++i)
        {
            // Pre-rotation distortion does a distotion that is opposite of the next bad but necessary thing
            float rx = aspectRatio * corners[i][0] * c - corners[i][1] * s;
            float ry = aspectRatio * corners[i][0] * s + corners[i][1] * c;
            
            // Post-rotation distortion stops it from distorting as it rotates but add it's own size distortion
            rx /= aspectRatio;

            // Translate to center
            vertices[i * 4 + 0] = xCenter + rx;
            vertices[i * 4 + 1] = yCenter + ry;

            switch (i)
            {
            case 0: vertices[i * 4 + 2] = x2; vertices[i * 4 + 3] = y1; break; // top-right
            case 1: vertices[i * 4 + 2] = x2; vertices[i * 4 + 3] = y2; break; // bottom-right
            case 2: vertices[i * 4 + 2] = x1; vertices[i * 4 + 3] = y2; break; // bottom-left
            case 3: vertices[i * 4 + 2] = x1; vertices[i * 4 + 3] = y1; break; // top-left
            }
        }

        glBindVertexArray(VAOSimple);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindTexture(GL_TEXTURE_2D, imageRef);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
    //*/
    static void fractionImage(int imageRef, float xCenter, float yCenter, float halfWidth, float halfHeight, float repeatX, float repeatY)
    {
        float vertices[] = {
             xCenter + halfWidth, yCenter + halfHeight, repeatX, repeatY, // top right
             xCenter + halfWidth, yCenter - halfHeight, repeatX, -repeatY, // bottom right
             xCenter - halfWidth, yCenter - halfHeight, -repeatX, -repeatY, // bottom left
             xCenter - halfWidth, yCenter + halfHeight, -repeatX, repeatY  // top left
        };

        glBindVertexArray(VAOSimple);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindTexture(GL_TEXTURE_2D, imageRef);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
    static void fractionImage(std::string stringRef, float xCenter, float yCenter, float halfWidth, float halfHeight, float repeatX, float repeatY) { fractionImage(imageFileRefs[stringRef], xCenter, yCenter, halfWidth, halfHeight, repeatX, repeatY); }
    static void backGroundImageRepeat(int imageRef, float repeat)
    {
        float x;
        float y;
        if (w > h)
        {
            y = repeat;
            x = (repeat * w) / h;
        }
        else
        {
            x = repeat;
            y = (repeat * h) / w;
        }

        float vertices[] = {
             1, 1, x, y, // top right
             1, -1, x, -y, // bottom right
             -1, -1, -x, -y, // bottom left
             -1, 1, -x, y  // top left
        };

        glBindVertexArray(VAOSimple);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindTexture(GL_TEXTURE_2D, imageRef);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
    static void backGroundImageRepeat(std::string stringRef, float repeat) { backGroundImageRepeat(imageFileRefs[stringRef], repeat); }

    static void multiDraw(int imageRef, const std::vector<float>& vertices, GLuint vao = VAOSimple, unsigned floatCount = 4)
    {
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(float),
            vertices.data(),
            GL_STATIC_DRAW
        );

        glBindTexture(GL_TEXTURE_2D, imageRef);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / floatCount);
        glBindVertexArray(0);
    }

    static void clear(glm::vec3 color)
    {
        glClearColor(color.r, color.g, color.b, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    static void clear(glm::vec4 color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    static void clear(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, const float a = 1.0f)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    static void set3DEnabled(const bool enable = true, const GLenum depth = GL_LEQUAL )
    {
        if (enable)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(depth);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }
    static void clear3D(const glm::vec3 color = {0,0,0})
    {
        glClearColor(color.r, color.g, color.b, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};