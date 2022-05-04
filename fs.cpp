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

size_t getCharStrLen(const char* str)
{
    // If char* is 20 bytes, this will break.
    size_t len = 0;
    while(str[len] != '\0'){
        len++;
    }
    return len;
}

size_t findIndexOfNodeWithName(const std::string& name,  std::array<char, 284>& nodes, size_t startIndex)
{
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
        index = nodes[index + NEXT_PTR_OFFSET];
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

/**
 * @param arquivoDaLista nome do arquivo em disco que contem a lista encadeada
 * @param novoNome nome a ser adicionado apos depoisDesteNome
 * @param depoisDesteNome um nome presente na lista
 */
void adiciona(std::string arquivoDaLista, std::string novoNome, std::string depoisDesteNome)
{
    // Steps:
    //  1. Read file into std::array
    //  2. Read first 4 bytes to find firstNodeIndex
    //  3. Find index of the node with name depoisDesteNome
    //  4. Find index of the node after depoisDesteNome
    //  5. Find index of empty space
    //  6. Write new node with name novoNome to empty space, and point to next node
    //  7. Go back and change the next of depoisDesteNome to the new node
    //  8. Store data back into file

    std::ifstream srcFile{arquivoDaLista, std::ios::binary};
    std::array<char, 284> nodes{};
    srcFile.read(nodes.data(), nodes.size());
    srcFile.close();

    size_t firstNodeIndex = nodes[0];
    size_t nodeDepoisDesteNomeIndex = findIndexOfNodeWithName(depoisDesteNome, nodes, firstNodeIndex);
    
    size_t emptySpaceIndex = findEmptySpaceIndex(nodes, FIRST_NODE_OFFSET);
}

#endif /* fs_h */