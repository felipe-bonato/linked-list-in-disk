#ifndef fs_h
#define fs_h
#include <string>

#include <fstream>
#include <array>

#include <iostream>
#include <iomanip>

#define NAME_OFFSET 4
#define NEXT_PTR_OFFSET 24
#define FIRST_NODE_OFFSET 4
#define NODE_SIZE 28
#define NODES_AMOUNT 10 // How many nodes are in the file
#define FILE_SIZE (NODES_AMOUNT * NODE_SIZE + FIRST_NODE_OFFSET)

size_t getCharStrLen(const char* str)
{
    // We cannot use the standard strlen because this is limited to 20 characters
    size_t len = 0;
    while(str[len] != '\0' && len < 20){
        len++;
    }
    return len;
}

size_t getNextNodeIndex(std::array<char, FILE_SIZE>& nodes, size_t curNodeIndex)
{
    return static_cast<size_t>(nodes[curNodeIndex + NEXT_PTR_OFFSET])
        | static_cast<size_t>(nodes[curNodeIndex + NEXT_PTR_OFFSET + 1] << 8)
        | static_cast<size_t>(nodes[curNodeIndex + NEXT_PTR_OFFSET + 2] << 16)
        | static_cast<size_t>(nodes[curNodeIndex + NEXT_PTR_OFFSET + 3] << 24);
}

size_t findIndexOfNodeWithName(
    const std::string& name,
    std::array<char, FILE_SIZE>& nodes,
    size_t startIndex
){
    size_t index = startIndex;
    while(true){
        // Convert node c-style char* to std::string
        std::string curName{
            &nodes[index + NAME_OFFSET],
            getCharStrLen(&nodes[index + NAME_OFFSET])
        };
        // If true, we found what we wanted
        if(curName == name){
            return index;
        }
        // If not, get next node index
        index = getNextNodeIndex(nodes, index);
    }
}

size_t findEmptySpaceIndex(std::array<char, 284>& nodes, size_t startIndex)
{
    for(size_t i = startIndex; i < NODES_AMOUNT - 1; i += NODE_SIZE){
        if(nodes[i] == '\0'){
            return i;
        }
    }
    throw new std::out_of_range("No empty space found in range");
}

std::array<char, FILE_SIZE> readFileIntoBufArray(const std::string& path)
{
    std::array<char, FILE_SIZE> buff;
    std::ifstream file(path, std::ios::binary);
    file.read(buff.data(), FILE_SIZE);
    file.close();
    return buff;
}

void writeToBuffArray(
        std::array<char, FILE_SIZE>& buff,
        size_t emptySpaceIndex,
        std::string name,
        size_t prevIndex,
        size_t nextIndex
){
    // is ocupied bit
    buff[emptySpaceIndex] = 0x01;
    // name
    for (
        size_t i = 0;
        i < 20 && i < name.size();
        i++)
    {
        buff[emptySpaceIndex + NAME_OFFSET + i] = name[i];
    }
    
    // next of new
    buff[emptySpaceIndex + NEXT_PTR_OFFSET] = nextIndex;
    buff[emptySpaceIndex + NEXT_PTR_OFFSET + 1] = nextIndex >> 8;
    buff[emptySpaceIndex + NEXT_PTR_OFFSET + 2] = nextIndex >> 16;
    buff[emptySpaceIndex + NEXT_PTR_OFFSET + 3] = nextIndex >> 24;

    // next of prev
    buff[prevIndex + NEXT_PTR_OFFSET] = emptySpaceIndex;
    buff[prevIndex + NEXT_PTR_OFFSET + 1] = emptySpaceIndex >> 8;
    buff[prevIndex + NEXT_PTR_OFFSET + 2] = emptySpaceIndex >> 16;
    buff[prevIndex + NEXT_PTR_OFFSET + 3] = emptySpaceIndex >> 24;
}

void writeBuffToFile(std::array<char, FILE_SIZE> buff, const std::string& path){
    std::ofstream file(path, std::ios::binary);
    file.write(buff.data(), FILE_SIZE);
}

/**
 * @param arquivoDaLista nome do arquivo em disco que contem a lista encadeada
 * @param novoNome nome a ser adicionado apos depoisDesteNome
 * @param depoisDesteNome um nome presente na lista
 */
void adiciona(std::string arquivoDaLista, std::string novoNome, std::string depoisDesteNome)
{
    // Steps:
    //  [x] 1. Read file into std::array
    //  [x] 2. Read first 4 bytes to find firstNodeIndex
    //  [x] 3. Find index of the node with name depoisDesteNome
    //  [x] 4. Find index of the node after depoisDesteNome
    //  [x] 5. Find index of empty space
    //  [] 6. Write new node with name novoNome to empty space, and point to next node
    //  [] 7. Go back and change the next of depoisDesteNome to the new node
    //  [] 8. Store data back into file

    auto nodes = readFileIntoBufArray(arquivoDaLista);

    auto firstNodeIndex = static_cast<size_t>(nodes[0]);
    auto nodeDepoisDesteNomeIndex = findIndexOfNodeWithName(
        depoisDesteNome, nodes, firstNodeIndex
    );
    
    auto nodeDepoisDesteNomeNextIndex = getNextNodeIndex(nodes, nodeDepoisDesteNomeIndex);

    auto emptySpaceIndex = findEmptySpaceIndex(nodes, FIRST_NODE_OFFSET);

    writeToBuffArray(
        nodes,
        emptySpaceIndex,
        novoNome,
        nodeDepoisDesteNomeIndex,
        nodeDepoisDesteNomeNextIndex
    );


    writeBuffToFile(nodes, arquivoDaLista);
}

#endif /* fs_h */