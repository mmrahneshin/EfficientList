// Copyright (C) Kamaledin Ghiasi-Shirazi, Ferdowsi Univerity of Mashhad, 2024 (1404 Hijri Shamsi)
//
// Author: Kamaledin Ghiasi-Shirazi, Mohammad Mahdi Rahneshin

#include <iostream>
#include <stack>
#include <sstream>
#include <optional>
#include <list>
#include <map>
#include <fstream>
#include <chrono>   // Add this for high precision timing
#include <iomanip>  // Add this for setprecision
#include <unistd.h> // sysconf, _SC_PAGE_SIZE
#include <malloc.h> // malloc_trim
#include "hybrid_list_v2.hpp"
#include "pbds_only_list.hpp"
#include <deque>
#include <random>

#include "../src/IndexedPPS23RBBinaryTreeNode.h"
#include "../src/IndexedPPS23RedBlackBinaryTree.h"
#include "../src/EfficientList.h"

using namespace std;
using namespace std::chrono; // Add this

int *values;
int *indexes;
int *deleteIndexes;
high_resolution_clock::time_point start_time, end_time; // Change these to chrono time points

double timeTaken;
double memoryUsed;

void insertToEfficientList(EfficientList<int> *ell, int size,
                           map<int, double> *timeTaken_map = nullptr)
{
    start_time = high_resolution_clock::now();
    for (int i = 0; i < size; i++)
    {
        // cout << "index: " << indexes[i] << "\t value:"
        //      << values[i] << endl;

        ell->insert(indexes[i], values[i]);
        // ell->drawTree();
        // ell->iterateInOrder();
        // cout << "===============" << endl;
    }
    end_time = high_resolution_clock::now();

    auto duration = duration_cast<nanoseconds>(end_time - start_time);
    timeTaken = duration.count() / 1e9; // Convert nanoseconds to seconds with high precision
    if (timeTaken_map != nullptr)
    {
        timeTaken_map->insert({size, timeTaken});
    }

    // cout << "insert time taken efficient list: " << timeTaken << endl;
}

void insertToPbdsDeque(HybridListV2<int> *pbds_deque, int size,
                       map<int, double> *timeTaken_map = nullptr)
{
    start_time = high_resolution_clock::now();
    int i;
    for (i = 0; i < size; i++)
    {
        pbds_deque->insert(indexes[i], values[i]);
    }
    end_time = high_resolution_clock::now();

    auto duration = duration_cast<nanoseconds>(end_time - start_time);
    timeTaken = duration.count() / 1e9; // Convert nanoseconds to seconds with high precision

    if (timeTaken_map != nullptr)
    {
        timeTaken_map->insert({size, timeTaken});
    }
    // cout << "insert time taken pbdstor: " << timeTaken << endl;
}

// void pushFrontToList(int i, list<int> *lst, int size)
// {
//     start = clock();
//     for (; i < size; i++)
//     {
//         lst->push_front(values[i]);
//     }
//     end_clock = clock();
//     timeTaken = double(end_clock - start) / double(CLOCKS_PER_SEC);
//     cout << "insert time taken list (push_front): " << timeTaken << endl;
// }

// void pushBackToList(int i, list<int> *lst, int size)
// {
//     start = clock();
//     for (; i < size; i++)
//     {
//         lst->push_back(values[i]);
//     }
//     end_clock = clock();
//     timeTaken = double(end_clock - start) / double(CLOCKS_PER_SEC);
//     cout << "insert time taken list (push_back): " << timeTaken << endl;
// }

bool valueCheck(EfficientList<int> *ell, HybridListV2<int> *pbds_deque, int size,
                map<int, double> *ell_timeTaken_map = nullptr, map<int, double> *pbds_timeTaken_map = nullptr)
{

    bool result = true;
    // cout << "\n**efficient list :\nYours\tCorrect" << endl;
    double ellTimeTaken = 0;
    double pbdstorTimeTaken = 0;
    int ellValue, pbdstorValue;
    for (int i = 0; i < size; i++)
    {
        if ((*ell)[i] != pbds_deque->get(i))
        {
            result = false;
            break;
        }
    }

    start_time = high_resolution_clock::now();
    for (int i = 0; i < size; i++)
    {
        ellValue = (*ell)[i];
    }
    end_time = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end_time - start_time);
    ellTimeTaken = duration.count() / 1e9;

    start_time = high_resolution_clock::now();
    for (int i = 0; i < size; i++)
    {
        pbdstorValue = pbds_deque->get(i);
    }
    end_time = high_resolution_clock::now();
    duration = duration_cast<nanoseconds>(end_time - start_time);
    pbdstorTimeTaken = duration.count() / 1e9;

    // cout << "ell get time taken: " << ellTimeTaken << endl;
    // cout << "pbdstor get time taken: " << pbdstorTimeTaken << endl;

    if (ell_timeTaken_map != nullptr)
    {
        ell_timeTaken_map->insert({size, ellTimeTaken});
    }

    if (pbds_timeTaken_map != nullptr)
    {
        pbds_timeTaken_map->insert({size, pbdstorTimeTaken});
    }

    return result;
}

// bool valueCheck(EfficientList<int> *ell, list<int> *lst, int size)
// {
//     bool result = true;
//     double ellTimeTaken = 0;
//     double listTimeTaken = 0;
//     int ellValue, listValue;

//     auto listIt = lst->begin(); // Iterator for the list

//     for (int i = 0; i < size; i++, ++listIt)
//     {
//         start = clock();
//         ellValue = (*ell)[i];
//         end_clock = clock();
//         ellTimeTaken += double(end_clock - start) / double(CLOCKS_PER_SEC);

//         start = clock();
//         listValue = *listIt;
//         end_clock = clock();
//         listTimeTaken += double(end_clock - start) / double(CLOCKS_PER_SEC);

//         if (ellValue != listValue)
//         {
//             result = false;
//             break;
//         }
//     }

//     cout << "ell get time taken: " << ellTimeTaken << endl;
//     cout << "list get time taken: " << listTimeTaken << endl;

//     return result;
// }

void removeFromEfficientList(EfficientList<int> *ell, int deleteIndexesSize,
                             map<int, double> *timeTaken_map = nullptr)
{
    start_time = high_resolution_clock::now();
    for (int i = 0; i < deleteIndexesSize; i++)
    {
        // cout << "index: " << indexes[i] << "\t value:"
        //      << values[i] << endl;

        ell->remove(deleteIndexes[i]);
        // ell->drawTree();
        // ell->iterateInOrder();
        // cout << "===============" << endl;
    }
    end_time = high_resolution_clock::now();
    malloc_trim(0);

    auto duration = duration_cast<nanoseconds>(end_time - start_time);
    timeTaken = duration.count() / 1e9;
    if (timeTaken_map != nullptr)
    {
        timeTaken_map->insert({deleteIndexesSize, timeTaken});
    }
    // cout << "remove time taken efficient list: " << timeTaken << endl;
}

void removeFromPbdsDeque(HybridListV2<int> *pbds_deque, int deleteIndexesSize,
                         map<int, double> *timeTaken_map = nullptr)
{

    start_time = high_resolution_clock::now();
    int i;
    for (i = 0; i < deleteIndexesSize; i++)
    {
        pbds_deque->erase(deleteIndexes[i]);
    }
    end_time = high_resolution_clock::now();

    malloc_trim(0);

    auto duration = duration_cast<nanoseconds>(end_time - start_time);
    timeTaken = duration.count() / 1e9;
    if (timeTaken_map != nullptr)
    {
        timeTaken_map->insert({deleteIndexesSize, timeTaken});
    }
    // cout << "remove time taken pbdstor: " << timeTaken << endl;
}

// void popFrontFromList(int i, list<int> *lst, int deleteIndexesSize)
// {
//     start = clock();
//     for (; i < deleteIndexesSize; i++)
//     {
//         lst->pop_front();
//     }
//     end_clock = clock();
//     timeTaken = double(end_clock - start) / double(CLOCKS_PER_SEC);
//     cout << "remove time taken list (pop_front): " << timeTaken << endl;
// }

// void popBackFromList(int i, list<int> *lst, int deleteIndexesSize)
// {
//     start = clock();
//     for (; i < deleteIndexesSize; i++)
//     {
//         lst->pop_back();
//     }
//     end_clock = clock();
//     timeTaken = double(end_clock - start) / double(CLOCKS_PER_SEC);
//     cout << "remove time taken list (pop_back): " << timeTaken << endl;
// }

int main()
{
    // time taken results
    map<int, double> *pushBack_ellResult = new map<int, double>();
    map<int, double> *get_after_pushBack_ellResult = new map<int, double>();

    map<int, double> *popBackAfterPushBack_ellResult = new map<int, double>();
    map<int, double> *get_after_popBackAfterPushBack_ellResult = new map<int, double>();

    map<int, double> *popFrontAfterPushBack_ellResult = new map<int, double>();
    map<int, double> *get_after_popFrontAfterPushBack_ellResult = new map<int, double>();

    map<int, double> *removeRandomIndicesAfterPushBack_ellResult = new map<int, double>();
    map<int, double> *get_after_removeRandomIndicesAfterPushBack_ellResult = new map<int, double>();

    map<int, double> *pushBack_pbdsResult = new map<int, double>();
    map<int, double> *get_after_pushBack_pbdsResult = new map<int, double>();

    map<int, double> *popBackAfterPushBack_pbdsResult = new map<int, double>();
    map<int, double> *get_after_popBackAfterPushBack_pbdsResult = new map<int, double>();

    map<int, double> *popFrontAfterPushBack_pbdsResult = new map<int, double>();
    map<int, double> *get_after_popFrontAfterPushBack_pbdsResult = new map<int, double>();

    map<int, double> *removeRandomIndicesAfterPushBack_pbdsResult = new map<int, double>();
    map<int, double> *get_after_removeRandomIndicesAfterPushBack_pbdsResult = new map<int, double>();

    // push_front
    map<int, double> *pushFront_ellResult = new map<int, double>();
    map<int, double> *get_after_pushFront_ellResult = new map<int, double>();

    map<int, double> *popFrontAfterPushFront_ellResult = new map<int, double>();
    map<int, double> *get_after_popFrontAfterPushFront_ellResult = new map<int, double>();

    map<int, double> *popBackAfterPushFront_ellResult = new map<int, double>();
    map<int, double> *get_after_popBackAfterPushFront_ellResult = new map<int, double>();

    map<int, double> *pushFront_pbdsResult = new map<int, double>();
    map<int, double> *get_after_pushFront_pbdsResult = new map<int, double>();

    map<int, double> *popFrontAfterPushFront_pbdsResult = new map<int, double>();
    map<int, double> *get_after_popFrontAfterPushFront_pbdsResult = new map<int, double>();

    map<int, double> *popBackAfterPushFront_pbdsResult = new map<int, double>();
    map<int, double> *get_after_popBackAfterPushFront_pbdsResult = new map<int, double>();

    // insert random indices
    map<int, double> *insertRandomIndices_ellResult = new map<int, double>();
    map<int, double> *get_after_insertRandomIndices_ellResult = new map<int, double>();

    map<int, double> *removeRandomIndices_ellResult = new map<int, double>();
    map<int, double> *get_after_removeRandomIndices_ellResult = new map<int, double>();

    map<int, double> *insertRandomIndices_pbdsResult = new map<int, double>();
    map<int, double> *get_after_insertRandomIndices_pbdsResult = new map<int, double>();

    map<int, double> *removeRandomIndices_pbdsResult = new map<int, double>();
    map<int, double> *get_after_removeRandomIndices_pbdsResult = new map<int, double>();

    // push_front then push_back
    map<int, double> *pushFront_pushBack_ellResult = new map<int, double>();
    map<int, double> *get_after_pushFront_pushBack_ellResult = new map<int, double>();

    map<int, double> *popFront_popBack_ellResult = new map<int, double>();
    map<int, double> *get_after_popFront_popBack_ellResult = new map<int, double>();

    map<int, double> *pushFront_pushBack_pbdsResult = new map<int, double>();
    map<int, double> *get_after_pushFront_pushBack_pbdsResult = new map<int, double>();

    map<int, double> *popFront_popBack_pbdsResult = new map<int, double>();
    map<int, double> *get_after_popFront_popBack_pbdsResult = new map<int, double>();

    // push_back then push_front
    map<int, double> *pushBack_pushFront_ellResult = new map<int, double>();
    map<int, double> *get_after_pushBack_pushFront_ellResult = new map<int, double>();

    map<int, double> *popBack_popFront_ellResult = new map<int, double>();
    map<int, double> *get_after_popBack_popFront_ellResult = new map<int, double>();

    map<int, double> *pushBack_pushFront_pbdsResult = new map<int, double>();
    map<int, double> *get_after_pushBack_pushFront_pbdsResult = new map<int, double>();

    map<int, double> *popBack_popFront_pbdsResult = new map<int, double>();
    map<int, double> *get_after_popBack_popFront_pbdsResult = new map<int, double>();

    for (int step = 1; step <= 1000000; step *= 10)
    {
        for (int size = step; size < step * 10 && size <= 2000000; size += step)
        {
            cout << size << endl;
            values = new int[size];
            indexes = new int[size];
            deleteIndexes = new int[size / 2];
            for (int i = 0; i < size; i++)
            {
                values[i] = i + 1;
                indexes[i] = i;
            }

            bool result = true;
            EfficientList<int> *ell;
            HybridListV2<int> *pbds_deque;
            // list<int> *lst;

            // double ellUpdateTimeTaken = 0;
            // double ellRemoveTimeTaken = 0;

            // cout << "The test has started now with " << size << " insertions and " << size / 2 << " deletions:" << endl;
            for (int input = 0; input < 16; input++)
            {
                switch (input)
                {
                case 0: // insert tests
                    // cout << endl
                    //      << "1: test efficient list vs list push_back insert" << endl;
                    ell = new EfficientList<int>;
                    pbds_deque = new HybridListV2<int>();

                    insertToEfficientList(ell, size, pushBack_ellResult);
                    // process_mem_usage(rss);
                    // std::cout << " RSS(KB): " << rss << "\n";
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    insertToPbdsDeque(pbds_deque, size, pushBack_pbdsResult);
                    result = valueCheck(ell, pbds_deque, size, get_after_pushBack_ellResult, get_after_pushBack_pbdsResult);
                    break;
                case 1: // remove tests
                    // cout << endl
                    //      << "2: test efficient list vs list pop_back" << endl;
                    for (int i = 0; i < size / 2; i++)
                    {
                        deleteIndexes[i] = size - (i + 1);
                    }

                    removeFromEfficientList(ell, size / 2, popBackAfterPushBack_ellResult);

                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;

                    removeFromPbdsDeque(pbds_deque, size / 2, popBackAfterPushBack_pbdsResult);

                    result = valueCheck(ell, pbds_deque, size / 2,
                                        get_after_popBackAfterPushBack_ellResult,
                                        get_after_popBackAfterPushBack_pbdsResult);

                    break;
                case 2: // insert tests
                    // cout << endl
                    //      << "3: test efficient list vs pbdstor push_back insert" << endl;
                    ell = new EfficientList<int>;
                    pbds_deque = new HybridListV2<int>();
                    insertToEfficientList(ell, size);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    insertToPbdsDeque(pbds_deque, size);
                    result = valueCheck(ell, pbds_deque, size);
                    break;
                case 3: // remove tests
                    // cout << endl
                    //      << "4: test efficient list vs pbdstor pop_front" << endl;
                    for (int i = 0; i < size / 2; i++)
                    {
                        deleteIndexes[i] = 0;
                    }
                    removeFromEfficientList(ell, size / 2,
                                            popFrontAfterPushBack_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    removeFromPbdsDeque(pbds_deque, size / 2, popFrontAfterPushBack_pbdsResult);
                    result = valueCheck(ell, pbds_deque, size / 2,
                                        get_after_popFrontAfterPushBack_ellResult,
                                        get_after_popFrontAfterPushBack_pbdsResult);

                    break;
                case 4: // insert tests
                    // cout << endl
                    //      << "5: test efficient list vs list push_front" << endl;
                    ell = new EfficientList<int>;
                    pbds_deque = new HybridListV2<int>();

                    for (int i = 0; i < size; i++)
                    {
                        indexes[i] = 0;
                    }
                    insertToEfficientList(ell, size, pushFront_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    insertToPbdsDeque(pbds_deque, size, pushFront_pbdsResult);
                    result = valueCheck(ell, pbds_deque, size, get_after_pushFront_ellResult, get_after_pushFront_pbdsResult);
                    break;
                case 5: // remove tests
                    // cout << endl
                    //      << "6: test efficient list vs list pop_front" << endl;
                    removeFromEfficientList(ell, size / 2, popFrontAfterPushFront_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    removeFromPbdsDeque(pbds_deque, size / 2, popFrontAfterPushFront_pbdsResult);
                    result = valueCheck(ell, pbds_deque, size / 2,
                                        get_after_popFrontAfterPushFront_ellResult,
                                        get_after_popFrontAfterPushFront_pbdsResult);
                    break;
                case 6: // insert tests
                    // cout << endl
                    //      << "7: test efficient list vs list push_front" << endl;
                    ell = new EfficientList<int>;
                    pbds_deque = new HybridListV2<int>();

                    insertToEfficientList(ell, size);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    insertToPbdsDeque(pbds_deque, size);
                    result = valueCheck(ell, pbds_deque, size);
                    break;
                case 7: // remove tests
                    // cout << endl
                    //      << "8: test efficient list vs list begin pop_back" << endl;
                    for (int i = 0; i < size / 2; i++)
                    {
                        deleteIndexes[i] = size - (i + 1);
                    }
                    removeFromEfficientList(ell, size / 2, popBackAfterPushFront_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    removeFromPbdsDeque(pbds_deque, size / 2, popBackAfterPushFront_pbdsResult);
                    result = valueCheck(ell, pbds_deque, size / 2,
                                        get_after_popBackAfterPushFront_ellResult,
                                        get_after_popBackAfterPushFront_pbdsResult);
                    break;
                case 8: // insert tests
                    // cout << endl
                    //      << "9: test efficient list vs pbdstor push_back" << endl;
                    ell = new EfficientList<int>;
                    pbds_deque = new HybridListV2<int>();
                    for (int i = 0; i < size; i++)
                    {
                        indexes[i] = i;
                    }
                    insertToEfficientList(ell, size);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    insertToPbdsDeque(pbds_deque, size);
                    result = valueCheck(ell, pbds_deque, size);
                    break;
                case 9: // remove tests
                    // cout << endl
                    //      << "10: test efficient list vs pbdstor random indices remove" << endl;
                    for (int i = 0; i < size / 2; i++)
                    {
                        deleteIndexes[i] = rand() % (size - (i + 1));
                    }
                    removeFromEfficientList(ell, size / 2, removeRandomIndicesAfterPushBack_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    removeFromPbdsDeque(pbds_deque, size / 2, removeRandomIndicesAfterPushBack_pbdsResult);
                    result = valueCheck(ell, pbds_deque, size / 2,
                                        get_after_removeRandomIndicesAfterPushBack_ellResult,
                                        get_after_removeRandomIndicesAfterPushBack_pbdsResult);
                    break;
                case 10: // insert tests
                    // cout << endl
                    //      << "11: test efficient list vs pbdstor random indices insert" << endl;
                    ell = new EfficientList<int>;
                    pbds_deque = new HybridListV2<int>();
                    for (int i = 0; i < size; i++)
                    {
                        indexes[i] = rand() % (i + 1);
                    }
                    insertToEfficientList(ell, size, insertRandomIndices_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    insertToPbdsDeque(pbds_deque, size, insertRandomIndices_pbdsResult);
                    result = valueCheck(ell, pbds_deque, size,
                                        get_after_insertRandomIndices_ellResult, get_after_insertRandomIndices_pbdsResult);
                    break;
                case 11: // remove tests
                    // cout << endl
                    //      << "12: test efficient list vs pbdstor random indices remove" << endl;
                    for (int i = 0; i < size / 2; i++)
                    {
                        deleteIndexes[i] = rand() % (size - (i + 1));
                    }
                    removeFromEfficientList(ell, size / 2, removeRandomIndices_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    removeFromPbdsDeque(pbds_deque, size / 2, removeRandomIndices_pbdsResult);
                    result = valueCheck(ell, pbds_deque, size / 2,
                                        get_after_removeRandomIndices_ellResult, get_after_removeRandomIndices_pbdsResult);
                    break;

                case 12: // insert tests
                    // cout << endl
                    //      << "13: test efficient list vs list half push_front then push_back the other half" << endl;
                    ell = new EfficientList<int>;
                    pbds_deque = new HybridListV2<int>();

                    for (int i = 0; i < size / 2; i++)
                    {
                        indexes[i] = 0;
                        indexes[i + size / 2] = i + size / 2;
                    }

                    insertToEfficientList(ell, size, pushFront_pushBack_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    insertToPbdsDeque(pbds_deque, size, pushFront_pushBack_pbdsResult);

                    result = valueCheck(ell, pbds_deque, size,
                                        get_after_pushFront_pushBack_ellResult,
                                        get_after_pushFront_pushBack_pbdsResult);
                    break;
                case 13: // remove tests
                    // cout << endl
                    //      << "14: test efficient list vs list begin half pop_front then pop_back the other half" << endl;
                    for (int i = 0; i < size / 4; i++)
                    {
                        deleteIndexes[i] = 0;
                        deleteIndexes[i + size / 4] = size - (i + size / 4 + 1);
                    }
                    removeFromEfficientList(ell, size / 2, popFront_popBack_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    removeFromPbdsDeque(pbds_deque, size / 2, popFront_popBack_pbdsResult);

                    result = valueCheck(ell, pbds_deque, size / 2,
                                        get_after_popFront_popBack_ellResult,
                                        get_after_popFront_popBack_pbdsResult);
                    break;
                case 14: // insert tests
                    // cout << endl
                    //      << "15: test efficient list vs list half push_back then push_front the other half" << endl;
                    ell = new EfficientList<int>;
                    pbds_deque = new HybridListV2<int>();

                    for (int i = 0; i < size / 2; i++)
                    {
                        indexes[i] = i;
                        indexes[i + size / 2] = 0;
                    }

                    insertToEfficientList(ell, size, pushBack_pushFront_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    insertToPbdsDeque(pbds_deque, size, pushBack_pushFront_pbdsResult);

                    result = valueCheck(ell, pbds_deque, size,
                                        get_after_pushBack_pushFront_ellResult,
                                        get_after_pushBack_pushFront_pbdsResult);
                    break;
                case 15: // remove tests
                    // cout << endl
                    //      << "16: test efficient list vs list begin half pop_back then pop_front the other half" << endl;
                    for (int i = 0; i < size / 4; i++)
                    {
                        deleteIndexes[i] = size - (i + 1);
                        deleteIndexes[i + size / 4] = 0;
                    }
                    removeFromEfficientList(ell, size / 2, popBack_popFront_ellResult);
                    // cout << "a1LeftInsertion: " << ell->mIPPS23RBbt->a1LeftInsertion << endl;
                    // cout << "a1RightInsertion: " << ell->mIPPS23RBbt->a1RightInsertion << endl;
                    // cout << "a2LeftInsertion: " << ell->mIPPS23RBbt->a2LeftInsertion << endl;
                    // cout << "a2RightInsertion: " << ell->mIPPS23RBbt->a2RightInsertion << endl;
                    // cout << "b1LeftInsertion: " << ell->mIPPS23RBbt->b1LeftInsertion << endl;
                    // cout << "b1RightInsertion: " << ell->mIPPS23RBbt->b1RightInsertion << endl;
                    // cout << "b2LeftInsertion: " << ell->mIPPS23RBbt->b2LeftInsertion << endl;
                    // cout << "b2RightInsertion: " << ell->mIPPS23RBbt->b2RightInsertion << endl;
                    removeFromPbdsDeque(pbds_deque, size / 2, popBack_popFront_pbdsResult);

                    result = valueCheck(ell, pbds_deque, size / 2,
                                        get_after_popBack_popFront_ellResult,
                                        get_after_popBack_popFront_pbdsResult);
                    break;
                }

                if (result)
                    continue;
                // cout << "** That was correct!" << endl;
                else
                {
                    cout << "** Doesn't match." << endl;
                    cout << "Your code did not pass the tests." << endl;
                    // int dummy;
                    // cin >> dummy;
                    return 0;
                }
            }

            // cout << "Your code passed all the tests." << endl;
            // int dummy;
            // cin >> dummy;

            delete ell;
            delete pbds_deque;
        }
    }

    auto saveMapToCSV = [](const map<int, double> *data, const string &filename)
    {
        string fullPath = "/home/sepehr/uni/DS/paper/EfficientList/C++/timeTakenResults/half_remove/" + filename;
        ofstream file(fullPath);
        if (file.is_open())
        {
            file << "Size,Time\n";
            file << fixed << setprecision(9); // Set precision to 9 decimal places
            for (const auto &pair : *data)
            {
                file << pair.first << "," << pair.second << "\n";
            }
            file.close();
            cout << "Saved " << fullPath << endl;
        }
        else
        {
            cout << "Error: Could not create " << fullPath << endl;
            cout << "Make sure the results directory exists" << endl;
        }
    };

    auto saveMemoryMapToCSV = [](const map<int, double> *data, const string &filename)
    {
        string fullPath = "~/uni/DS/paper/EfficientList/C++/memoryResults/half_remove/" + filename;
        ofstream file(fullPath);
        if (file.is_open())
        {
            file << "Size,Time\n";
            file << fixed << setprecision(9); // Set precision to 9 decimal places
            for (const auto &pair : *data)
            {
                file << pair.first << "," << pair.second << "\n";
            }
            file.close();
            cout << "Saved " << fullPath << endl;
        }
        else
        {
            cout << "Error: Could not create " << fullPath << endl;
            cout << "Make sure the results directory exists" << endl;
        }
    };

    // Save all timing results
    saveMapToCSV(pushBack_ellResult, "pushBack_ell_results.csv");
    saveMapToCSV(get_after_pushBack_ellResult, "get_after_pushBack_ell_results.csv");

    saveMapToCSV(popBackAfterPushBack_ellResult, "popBackAfterPushBack_ell_results.csv");
    saveMapToCSV(get_after_popBackAfterPushBack_ellResult, "get_after_popBackAfterPushBack_ell_results.csv");

    saveMapToCSV(popFrontAfterPushBack_ellResult, "popFrontAfterPushBack_ell_results.csv");
    saveMapToCSV(get_after_popFrontAfterPushBack_ellResult, "get_after_popFrontAfterPushBack_ell_results.csv");

    saveMapToCSV(removeRandomIndicesAfterPushBack_ellResult, "removeRandomIndicesAfterPushBack_ell_results.csv");
    saveMapToCSV(get_after_removeRandomIndicesAfterPushBack_ellResult, "get_after_removeRandomIndicesAfterPushBack_ell_results.csv");

    saveMapToCSV(pushBack_pbdsResult, "pushBack_pbdsV2_results.csv");
    saveMapToCSV(get_after_pushBack_pbdsResult, "get_after_pushBack_pbdsV2_results.csv");

    saveMapToCSV(popBackAfterPushBack_pbdsResult, "popBackAfterPushBack_pbdsV2_results.csv");
    saveMapToCSV(get_after_popBackAfterPushBack_pbdsResult, "get_after_popBackAfterPushBack_pbdsV2_results.csv");

    saveMapToCSV(popFrontAfterPushBack_pbdsResult, "popFrontAfterPushBack_pbdsV2_results.csv");
    saveMapToCSV(get_after_popFrontAfterPushBack_pbdsResult, "get_after_popFrontAfterPushBack_pbdsV2_results.csv");

    saveMapToCSV(removeRandomIndicesAfterPushBack_pbdsResult, "removeRandomIndicesAfterPushBack_pbdsV2_results.csv");
    saveMapToCSV(get_after_removeRandomIndicesAfterPushBack_pbdsResult, "get_after_removeRandomIndicesAfterPushBack_pbdsV2_results.csv");

    saveMapToCSV(pushFront_ellResult, "pushFront_ell_results.csv");
    saveMapToCSV(get_after_pushFront_ellResult, "get_after_pushFront_ell_results.csv");

    saveMapToCSV(popFrontAfterPushFront_ellResult, "popFrontAfterPushFront_ell_results.csv");
    saveMapToCSV(get_after_popFrontAfterPushFront_ellResult, "get_after_popFrontAfterPushFront_ell_results.csv");

    saveMapToCSV(popBackAfterPushFront_ellResult, "popBackAfterPushFront_ell_results.csv");
    saveMapToCSV(get_after_popBackAfterPushFront_ellResult, "get_after_popBackAfterPushFront_ell_results.csv");

    saveMapToCSV(pushFront_pbdsResult, "pushFront_pbdsV2_results.csv");
    saveMapToCSV(get_after_pushFront_pbdsResult, "get_after_pushFront_pbdsV2_results.csv");

    saveMapToCSV(popFrontAfterPushFront_pbdsResult, "popFrontAfterPushFront_pbdsV2_results.csv");
    saveMapToCSV(get_after_popFrontAfterPushFront_pbdsResult, "get_after_popFrontAfterPushFront_pbdsV2_results.csv");

    saveMapToCSV(popBackAfterPushFront_pbdsResult, "popBackAfterPushFront_pbdsV2_results.csv");
    saveMapToCSV(get_after_popBackAfterPushFront_pbdsResult, "get_after_popBackAfterPushFront_pbdsV2_results.csv");

    saveMapToCSV(insertRandomIndices_ellResult, "insertRandomIndices_ell_results.csv");
    saveMapToCSV(get_after_insertRandomIndices_ellResult, "get_after_insertRandomIndices_ell_results.csv");

    saveMapToCSV(removeRandomIndices_ellResult, "removeRandomIndices_ell_results.csv");
    saveMapToCSV(get_after_removeRandomIndices_ellResult, "get_after_removeRandomIndices_ell_results.csv");

    saveMapToCSV(insertRandomIndices_pbdsResult, "insertRandomIndices_pbdsV2_results.csv");
    saveMapToCSV(get_after_insertRandomIndices_pbdsResult, "get_after_insertRandomIndices_pbdsV2_results.csv");

    saveMapToCSV(removeRandomIndices_pbdsResult, "removeRandomIndices_pbdsV2_results.csv");
    saveMapToCSV(get_after_removeRandomIndices_pbdsResult, "get_after_removeRandomIndices_pbdsV2_results.csv");

    saveMapToCSV(pushFront_pushBack_ellResult, "pushFront_pushBack_ell_results.csv");
    saveMapToCSV(get_after_pushFront_pushBack_ellResult, "get_after_pushFront_pushBack_ell_results.csv");

    saveMapToCSV(popFront_popBack_ellResult, "popFront_popBack_ell_results.csv");
    saveMapToCSV(get_after_popFront_popBack_ellResult, "get_after_popFront_popBack_ell_results.csv");

    saveMapToCSV(pushFront_pushBack_pbdsResult, "pushFront_pushBack_pbdsV2_results.csv");
    saveMapToCSV(get_after_pushFront_pushBack_pbdsResult, "get_after_pushFront_pushBack_pbdsV2_results.csv");

    saveMapToCSV(popFront_popBack_pbdsResult, "popFront_popBack_pbdsV2_results.csv");
    saveMapToCSV(get_after_popFront_popBack_pbdsResult, "get_after_popFront_popBack_pbdsV2_results.csv");

    saveMapToCSV(pushBack_pushFront_ellResult, "pushBack_pushFront_ell_results.csv");
    saveMapToCSV(get_after_pushBack_pushFront_ellResult, "get_after_pushBack_pushFront_ell_results.csv");

    saveMapToCSV(popBack_popFront_ellResult, "popBack_popFront_ell_results.csv");
    saveMapToCSV(get_after_popBack_popFront_ellResult, "get_after_popBack_popFront_ell_results.csv");

    saveMapToCSV(pushBack_pushFront_pbdsResult, "pushBack_pushFront_pbdsV2_results.csv");
    saveMapToCSV(get_after_pushBack_pushFront_pbdsResult, "get_after_pushBack_pushFront_pbdsV2_results.csv");

    saveMapToCSV(popBack_popFront_pbdsResult, "popBack_popFront_pbdsV2_results.csv");
    saveMapToCSV(get_after_popBack_popFront_pbdsResult, "get_after_popBack_popFront_pbdsV2_results.csv");

    // Clean up time taken maps
    delete pushBack_ellResult;
    delete get_after_pushBack_ellResult;

    delete popBackAfterPushBack_ellResult;
    delete get_after_popBackAfterPushBack_ellResult;

    delete popFrontAfterPushBack_ellResult;
    delete get_after_popFrontAfterPushBack_ellResult;

    delete removeRandomIndicesAfterPushBack_ellResult;
    delete get_after_removeRandomIndicesAfterPushBack_ellResult;

    delete pushBack_pbdsResult;
    delete get_after_pushBack_pbdsResult;

    delete popBackAfterPushBack_pbdsResult;
    delete get_after_popBackAfterPushBack_pbdsResult;

    delete popFrontAfterPushBack_pbdsResult;
    delete get_after_popFrontAfterPushBack_pbdsResult;

    delete removeRandomIndicesAfterPushBack_pbdsResult;
    delete get_after_removeRandomIndicesAfterPushBack_pbdsResult;

    delete pushFront_ellResult;
    delete get_after_pushFront_ellResult;

    delete popFrontAfterPushFront_ellResult;
    delete get_after_popFrontAfterPushFront_ellResult;

    delete popBackAfterPushFront_ellResult;
    delete get_after_popBackAfterPushFront_ellResult;

    delete pushFront_pbdsResult;
    delete get_after_pushFront_pbdsResult;

    delete popFrontAfterPushFront_pbdsResult;
    delete get_after_popFrontAfterPushFront_pbdsResult;

    delete popBackAfterPushFront_pbdsResult;
    delete get_after_popBackAfterPushFront_pbdsResult;

    delete insertRandomIndices_ellResult;
    delete get_after_insertRandomIndices_ellResult;

    delete removeRandomIndices_ellResult;
    delete get_after_removeRandomIndices_ellResult;

    delete insertRandomIndices_pbdsResult;
    delete get_after_insertRandomIndices_pbdsResult;

    delete removeRandomIndices_pbdsResult;
    delete get_after_removeRandomIndices_pbdsResult;

    delete pushFront_pushBack_ellResult;
    delete get_after_pushFront_pushBack_ellResult;

    delete popFront_popBack_ellResult;
    delete get_after_popFront_popBack_ellResult;

    delete pushFront_pushBack_pbdsResult;
    delete get_after_pushFront_pushBack_pbdsResult;

    delete popFront_popBack_pbdsResult;
    delete get_after_popFront_popBack_pbdsResult;

    delete pushBack_pushFront_ellResult;
    delete get_after_pushBack_pushFront_ellResult;

    delete popBack_popFront_ellResult;
    delete get_after_popBack_popFront_ellResult;

    delete pushBack_pushFront_pbdsResult;
    delete get_after_pushBack_pushFront_pbdsResult;

    delete popBack_popFront_pbdsResult;
    delete get_after_popBack_popFront_pbdsResult;
    // delete lst;
    return 0;
}
