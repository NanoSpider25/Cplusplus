#include <vector>
#include "any"
#include <iostream>
#include <string>
#pragma once
using namespace std;
class DS1D
{
    public:

        DS1D(int size, int offLoadFrom, int recursiveSplitEvery, int recursiveSplitHowManySplits, int recursiveSplitMaxAmountOfSplits, float nestedSplit, int zeroLinkedToY, float concurrentRecursion, float openStructure, bool copyPasteInfinitely);
        DS1D();
        DS1D(int size);
        DS1D(float openStructure);
        DS1D(int size, float openStructure);
        DS1D(int size, int offLoadFrom, float openStructure);
        virtual ~DS1D();
        void display();
        void recursive_index_sub(std::any value);
        void addLayer();
		void add(std::any value);

    protected:

    private://             < size default 0                             > 0                            > 1
        int size = 1; int offLoadFrom = 0; int recursiveSplitEvery = -1; int recursiveSplitHowManySplits = -1; int recursiveSplitMaxAmountOfSplits = -1; float nestedSplit = false; float concurrentRecursion = false; float openStructure = true; bool copyPasteInfinitely = false;// vec[i]
        std::vector<std::any> vec;// = { 1, std::vector<std::any>{ std::string("hello") }, std::vector<std::any>{std::vector<std::any>{1,2,3},3,4} };
        // 1 1 < 1 1 < 1 1 1 1 1 1 1
        int depth = 1;
        int splitCounter = 0;
		int recursiveSplitCounter = 0;
        std::vector<int> indicies = { 0 };
		bool firstNest = true;
        int getSizeAt(std::vector<int> indicies);
        bool findValidIndex(std::vector<int>& indicies, int depth, int maxSplit, std::vector<int>& current);
};
