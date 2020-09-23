#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include <drawable.h>


//using namespace std;

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, GRASSSIDE, DIRT, STONE, ICE, WATER,
    REDWOOL, PINKWOOL, GREENWOOL, YELLOWWOOL, BLUEWOOL,
    PURPLEWOOL, ORANGEWOOL, WOOD, ENDSAND, OBSIDIAN
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

// TODO have Chunk inherit from Drawable
class Chunk : public Drawable {
private:

    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;



public:
    Chunk(OpenGLContext *mp_context, float x, float z);
    float worldX; // X-coordinate of lower left world position
    float worldZ; // Z-coordinate of lower left world position

    int maxHeight;
    int getworldX();
    int getworldZ();
    int getm_count();
    int gett_count();
    void setm_count(int m_count);
    void sett_count(int t_count);
    virtual void create();
    void populateVBOs(const std::vector<GLuint>& idx, const std::vector<glm::vec4>& posNorColTex);
    void populateTVBOs(const std::vector<GLuint> &idx, const std::vector<glm::vec4> &posNorCol);
    void modular(std::vector<GLuint>& idx, std::vector<glm::vec4>& posNorCol, int& i,
                        std::vector<GLuint>& t_idx, std::vector<glm::vec4>& t_posNorCol, int& t_i,
                        Chunk* c1, Chunk* c2, glm::vec4 norm1, glm::vec4 norm2,
                        BlockType b1, BlockType b2,
                        int x, int y, int z);

    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);
    void createEWFaces(std::vector<GLuint>& idx, std::vector<glm::vec4>& posNorCol, int& i,
                       std::vector<GLuint>& t_idx, std::vector<glm::vec4>& t_posNorCol, int& t_i);
    void createNSFaces(std::vector<GLuint>& idx, std::vector<glm::vec4>& posNorCol, int& i,
                       std::vector<GLuint>& t_idx, std::vector<glm::vec4>& t_posNorCol, int& t_i);
    void createUDFaces(std::vector<GLuint>& idx, std::vector<glm::vec4>& posNorCol, int& i,
                       std::vector<GLuint>& t_idx, std::vector<glm::vec4>& t_posNorCol, int& t_i);
};
